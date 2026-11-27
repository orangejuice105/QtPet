#ifndef PETWINDOW_H
#define PETWINDOW_H

#include <QWidget>
#include <QPixmap>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QTimer>
#include "animation/frameplayer.h"

class PetWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PetWindow(QWidget *parent = nullptr);
    ~PetWindow() override;

protected:
    void paintEvent(QPaintEvent *event) override; //重绘窗口
    void contextMenuEvent(QContextMenuEvent *event) override; //右键菜单
    void mousePressEvent(QMouseEvent *event) override; //鼠标按下
    void mouseMoveEvent(QMouseEvent *event) override; //鼠标移动
    void mouseReleaseEvent(QMouseEvent *event) override; //鼠标松开


private:
    bool isOnTop_ = true; //默认置顶
    void setAlwaysOnTop(bool enabled);

    bool isPressed_ = false; //左键是否按下
    bool isDragging_ = false; //是否进入拖动状态
    QPoint pressGlobalPos_; //鼠标刚按下时的全局坐标
    QPoint dragOffset_; //鼠标相对窗口左上角的偏移量

    QString greetingText_; //当前要显示的问候语
    QTimer greetingTimer_; //让气泡自动消失的定时器
    void showGreeting(const QString& text); //触发显示问候并开始倒计时

    FramePlayer framePlayer_; //帧播放器
    QSize petCanvasSize_ = {120,120}; //角色逻辑画布上限尺寸
    QPoint footAnchor_ = {90,156}; //窗口内的脚底锚点坐标

};
#endif // PETWINDOW_H
