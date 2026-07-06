#ifndef FLOATING_WINDOW_H
#define FLOATING_WINDOW_H

#include <QWidget>
#include <QDebug>
#include <QMouseEvent>

namespace Ui {
class FloatingWindow;
}

class FloatingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingWindow(QWidget *parent = nullptr);
    ~FloatingWindow();

    //设置秒数值
    void set_value(int value);
    //设置秒范围
    void set_range(int min,int max);

private:
    Ui::FloatingWindow *ui;
    QPoint dragPosition{0,0};

protected:
    //鼠标点击事件
    void mousePressEvent(QMouseEvent *event) override;
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;
    //鼠标双击事件
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    //开始循环信号
    void sig_start_loop(void);
    //暂停循环信号
    void sig_pause_loop(void);
    //继续循环信号
    void sig_continue_loop(void);
    //双击事件
    void sig_double_clicked(void);
};

#endif // FLOATING_WINDOW_H
