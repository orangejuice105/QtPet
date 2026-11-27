#include "frameplayer.h"
#include <QImageReader>

FramePlayer::FramePlayer(QObject *parent) : QObject(parent){
    frameTimer_.setSingleShot(true); //开启单次触发模式
    connect(&frameTimer_,&QTimer::timeout,this,&FramePlayer::advanceFrame); //把定时器到期和推进下一帧连起来
}

bool FramePlayer::loadClip(PetClip clip,const QString& resourcePath){
    QImageReader reader(resourcePath); //读图器
    ClipData clipData; //存解析出来的图和时长

    while(true){
        QImage frameImage = reader.read(); //读取当前帧并进行到下一帧
        //读取结束退出循环
        if(frameImage.isNull()){
            break;
        }
        //否则转成QPixmap并保存进frames
        else{
            //将解码得到的QImage转为适合屏幕高速绘制的QPixmap
            clipData.frames.append(QPixmap::fromImage(frameImage));

            int delayMs = reader.nextImageDelay(); //取出这一帧的停留毫秒数
            //读取帧间隔，对缺失或过小的异常值做下限兜底，防止高频重绘占满CPU
            if(delayMs < 20)delayMs = 100;
            clipData.delaysMs.append(delayMs);
        }
    }

    //校验解码帧非空；失败时直接返回，不覆盖已有有效片段
    if(clipData.frames.isEmpty()){
        return false;
    }
    //存入QMap字典
    clips_.insert(clip,clipData);
    return true;
}

// 返回当前激活片段的当前帧；若片段未加载或帧索引越界，返回空 QPixmap
QPixmap FramePlayer::currentFrame() const{
    // 使用常量迭代器只读查找当前片段，满足 const 约束并避免二次查表
    auto it = clips_.constFind(currentClip_);
    if(it == clips_.constEnd()){
        return {}; // 查找未命中：目标动作片段尚未加载
    }
    // 边界检查（0 基索引，合法范围为 [0, size - 1]），防止容器越界访问
    if(currentFrameIndex_ < 0 || currentFrameIndex_ >= it->frames.size()){
        return {};
    }
    return it->frames[currentFrameIndex_];
}

void FramePlayer::play(PetClip clip){
    auto it = clips_.constFind(clip);
    //如果未查找到动画片段
    if(it == clips_.constEnd() || it->frames.isEmpty()){
        return;
    }

    //先停旧片段
    frameTimer_.stop();
    //切换为新片段
    currentClip_ = clip;
    currentFrameIndex_ = 0; //从第0帧开始播

    emit frameChanged(); //发出信号通知外面立刻画新片段第0帧

    //如果帧数大于1则提前准备
    if(it->frames.size() > 1){
        frameTimer_.start(it->delaysMs[currentFrameIndex_]);
    }
}

void FramePlayer::advanceFrame(){
    auto it = clips_.constFind(currentClip_);
    //防御性校验：片段无效或不足两帧时停止调度，同时防止下方取模除零
    if(it == clips_.constEnd() || it->frames.size() <= 1){
        frameTimer_.stop();
        return;
    }

    currentFrameIndex_ = (currentFrameIndex_ + 1)%it->frames.size(); //取模推进下标循环

    emit frameChanged();

    frameTimer_.start(it->delaysMs[currentFrameIndex_]);
}

void FramePlayer::pause(){
    frameTimer_.stop();
}

PetClip FramePlayer::currentClip() const{
    return currentClip_;
}





