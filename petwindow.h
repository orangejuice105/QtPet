#ifndef PETWINDOW_H
#define PETWINDOW_H

#include <QWidget>
#include <QPixmap>
#include <QMenu>
#include <QContextMenuEvent>

class PetWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PetWindow(QWidget *parent = nullptr);
    ~PetWindow() override;

protected:
    void paintEvent(QPaintEvent *event) override; //重绘窗口
    void contextMenuEvent(QContextMenuEvent *event) override; //右键菜单

private:
    QPixmap petPixmap_; //保存加载好的图片
    bool isOnTop_ = true; //默认置顶
    void setAlwaysOnTop(bool enabled);
};
#endif // PETWINDOW_H
