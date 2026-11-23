#include "petwindow.h"
#include <QPainter>
#include <QApplication>

PetWindow::PetWindow(QWidget *parent)
    : QWidget(parent)
{
    //设置窗口为无边框
    setWindowFlag(Qt::FramelessWindowHint,true);
    //设置窗口置顶
    setWindowFlag(Qt::WindowStaysOnTopHint,isOnTop_);

    setAttribute(Qt::WA_TranslucentBackground); //设置当前窗口为透明背景
    setFixedSize(180,180); //设置窗口尺寸
    petPixmap_.load(QStringLiteral(":/pet/placeholder.gif")); //创建窗口时加载图片
}

PetWindow::~PetWindow() = default;

void PetWindow::paintEvent(QPaintEvent *){
    QPainter painter(this);
    //规定图片绘画区域
    QRect target(30,30,120,120);
    //绘图，将已经加载好的图片画到指定区域
    painter.drawPixmap(target,petPixmap_);
}

void PetWindow::contextMenuEvent(QContextMenuEvent *event){
    QMenu rightMenu(this); //右击菜单

    QAction *stayOnTopAction = rightMenu.addAction(QStringLiteral("置顶"));
    stayOnTopAction->setCheckable(true); //设置这一项为可以勾选
    stayOnTopAction->setChecked(isOnTop_); //设置勾选状态

    rightMenu.addSeparator(); //分割线

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







