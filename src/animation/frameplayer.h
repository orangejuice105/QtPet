#ifndef FRAMEPLAYER_H
#define FRAMEPLAYER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QPixmap>
#include <QString>
#include <QTimer>

//动作片段类型
enum class PetClip{
    Idle,
    Walk,
    Sleep
};

//单段动画数据
struct ClipData{
    QList<QPixmap> frames; //解出来的所有帧图片
    QList<int> delaysMs; //每帧对应的停留毫秒数
};

class FramePlayer : public QObject
{
    Q_OBJECT
public:
    explicit FramePlayer(QObject *parent = nullptr);

    bool loadClip(PetClip clip,const QString& resourcePath); //加载一段GIF动画
    QPixmap currentFrame() const; //获取当前这一帧画面

    void play(PetClip clip); //切换并播指定动作片段
    void pause(); //暂停播放
    PetClip currentClip() const; //查询当前正在播哪个动作

signals:
    void frameChanged(); //当前帧已变化通知信号

private:
    QMap<PetClip,ClipData> clips_; //按动作类型存放所有动画包的字典表
    PetClip currentClip_ = PetClip::Idle; //当前正在播放的动作
    int currentFrameIndex_ = 0; //当前播到第几帧

    QTimer frameTimer_; //全类唯一的帧调度计时器
    void advanceFrame(); //定时器到期时推进到下一帧
};

#endif // FRAMEPLAYER_H
