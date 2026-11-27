#include "petwindow.h"
#include <QPainter>
#include <QApplication>
#include <QDebug>

PetWindow::PetWindow(QWidget *parent)
    : QWidget(parent)
{
    //设置窗口为无边框
    setWindowFlag(Qt::FramelessWindowHint,true);
    //设置窗口置顶
    setWindowFlag(Qt::WindowStaysOnTopHint,isOnTop_);

    setAttribute(Qt::WA_TranslucentBackground); //设置当前窗口为透明背景
    setFixedSize(180,180); //设置窗口尺寸

    //加载每张gif图并连接和播放
    framePlayer_.loadClip(PetClip::Idle,QStringLiteral(":/pet/idle.gif"));
    framePlayer_.loadClip(PetClip::Sleep,QStringLiteral(":/pet/sleep.gif"));
    framePlayer_.loadClip(PetClip::Walk,QStringLiteral(":/pet/walk.gif"));
    connect(&framePlayer_,&FramePlayer::frameChanged,this,[this](){update();});
    framePlayer_.play(PetClip::Idle);


    //初始化单次定时器
    greetingTimer_.setSingleShot(true); //单次触发模式
    connect(&greetingTimer_,&QTimer::timeout,this,[this](){
        greetingText_.clear();
        update();
    });
}

PetWindow::~PetWindow() = default;

void PetWindow::paintEvent(QPaintEvent *){
    QPainter painter(this);
    //规定图片绘画区域

    //先将图片区域清成全透明，防止上一帧留下残影
    painter.setCompositionMode(QPainter::CompositionMode_Source); //切换成覆盖模式
    painter.fillRect(rect(),Qt::transparent); //刷成全透明
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver); //切回正常叠加模式

    //取当前帧
    QPixmap currentFrame = framePlayer_.currentFrame();
    if(currentFrame.isNull())return;

    //保持纵横比缩放后的绘制尺寸
    QSize drawSize = currentFrame.size().scaled(petCanvasSize_,Qt::KeepAspectRatio);
    //目标矩形的左上角X
    int drawX = footAnchor_.x() - drawSize.width() / 2; //左右居中对齐脚底点
    //目标矩形的左上角Y
    int drawY = footAnchor_.y() - drawSize.height(); //让图片刚好踩在脚底线上

    QRect target(drawX,drawY,drawSize.width(),drawSize.height());

    painter.setRenderHint(QPainter::SmoothPixmapTransform,true); //开启平滑缩放抗锯齿
    //绘图，将已经加载好的图片画到指定区域
    painter.drawPixmap(target,currentFrame);

    //画气泡
    if(!greetingText_.isEmpty()){
        QRect bubbleRect(20,2,140,26);
        painter.setRenderHint(QPainter::Antialiasing,true); //开启几何图形抗锯齿
        painter.setBrush(QColor(255,255,255,230)); //设置填充画刷
        painter.setPen(QColor(180,180,180,200)); //设置边框画笔
        painter.drawRoundedRect(bubbleRect,10,10); //画出圆角气泡，10分别为X/Y圆角半径
        painter.setPen(QColor(50,50,50)); //画笔换成深色
        painter.drawText(bubbleRect,Qt::AlignCenter,greetingText_); //写上问候语
    }
}

void PetWindow::contextMenuEvent(QContextMenuEvent *event){
    QMenu rightMenu(this); //右击菜单

    QAction *stayOnTopAction = rightMenu.addAction(QStringLiteral("置顶"));
    stayOnTopAction->setCheckable(true); //设置这一项为可以勾选
    stayOnTopAction->setChecked(isOnTop_); //设置勾选状态

    rightMenu.addSeparator(); //分割线

    QAction *idleAction = rightMenu.addAction(QStringLiteral("待机"));
    rightMenu.addSeparator();

    QAction *walkAction = rightMenu.addAction(QStringLiteral("行走"));
    rightMenu.addSeparator();

    QAction *sleepAction = rightMenu.addAction(QStringLiteral("睡觉"));
    rightMenu.addSeparator();

    QAction *exitAction = rightMenu.addAction(QStringLiteral("退出"));

    //exec()弹出菜单
    QAction *selectAction = rightMenu.exec(event->globalPos()); //event->globalPos()：取得鼠标右击时的坐标

    //根据选中的菜单项执行对应操作
    //选中事件为退出时执行
    if(selectAction == exitAction){
        QCoreApplication::quit();
    }
    else if(selectAction == stayOnTopAction){
        setAlwaysOnTop(!isOnTop_);
    }
    else if(selectAction == idleAction){
        framePlayer_.play(PetClip::Idle);
    }
    else if(selectAction == walkAction){
        framePlayer_.play(PetClip::Walk);
    }
    else if(selectAction == sleepAction){
        framePlayer_.play(PetClip::Sleep);
    }
}

void PetWindow::setAlwaysOnTop(bool enabled){
    //检查窗口目前是否显示
    bool wasVisible = isVisible();
    //记录原位置
    QPoint oldPos = pos();

    setWindowFlag(Qt::WindowStaysOnTopHint,enabled);

    isOnTop_ = enabled;

    //如果窗口原本处于可见状态，则需重新显示，调用setWindowFlag(...)修改置顶后Qt会把窗口变成隐藏状态
    if(wasVisible){
        show();
    }
    move(oldPos); //恢复原逻辑位置，防止切换置顶后偏移或跳回主屏
}

void PetWindow::mousePressEvent(QMouseEvent *event){
    //不是左键直接返回
    if(event->button() != Qt::LeftButton){
        QWidget::mousePressEvent(event);
        return;
    }

    isPressed_ = true;
    isDragging_ = false;

    pressGlobalPos_ = event->globalPosition().toPoint();

    dragOffset_ = pressGlobalPos_ - pos(); //计算偏移量

    event->accept(); //表示事件处理完了不再向上传递
}

void PetWindow::mouseMoveEvent(QMouseEvent *event){
    //如果左键没按着
    if(!isPressed_ || !(event->buttons() & Qt::LeftButton)){
        QWidget::mouseMoveEvent(event);
        return;
    }

    QPoint currentGlobalPos = event->globalPosition().toPoint(); //移动中鼠标的全局坐标

    if(!isDragging_){
        if((currentGlobalPos - pressGlobalPos_).manhattanLength() >= QApplication::startDragDistance()){
            isDragging_ = true;
        }
        else {
            event->accept();
            return;
        }
    }

    move(currentGlobalPos - dragOffset_);

    event->accept();
}

void PetWindow::mouseReleaseEvent(QMouseEvent *event){
    if(!(event->button() == Qt::LeftButton && isPressed_)){
        QWidget::mouseReleaseEvent(event);
        return;
    }

    bool wasDragging = isDragging_;
    isDragging_ = false;
    isPressed_ = false;

    if(!wasDragging){
        showGreeting(QStringLiteral("你好呀~"));
    }

    event->accept();
}

void PetWindow::showGreeting(const QString& text){
    greetingText_ = text;
    greetingTimer_.start(2000); //启动2秒倒计时
    update();
}





