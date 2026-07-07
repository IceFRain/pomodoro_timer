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

    //设置时钟秒数值
    void set_clock_value(int value);
    //设置时钟秒范围
    void set_clock_range(int min,int max);
    //时钟进度条显示状态切换
    void set_bar_clock_show(bool status);

    //设置饮水量
    void set_drink_value(int value);
    //设置饮水进度条范围
    void set_drink_range(int min,int max);
    //饮水量进度条显示状态切换
    void set_bar_drink_show(bool status);

public slots:
    //时钟进度条右键点击槽
    void slot_bar_clock_right_clicked(const QPoint &pos);
    //饮水量进度条右键点击槽
    void slot_bar_drink_right_clicked(const QPoint &pos);

signals:
    //开始循环信号
    void sig_clock_start_loop(void);
    //暂停循环信号
    void sig_clock_pause_loop(void);
    //继续循环信号
    void sig_clock_continue_loop(void);

    //记录一杯饮水信号
    void sig_drink_record_cup(void);

    //双击事件
    void sig_double_clicked(void);

protected:
    //鼠标点击事件
    void mousePressEvent(QMouseEvent *event) override;
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;
    //鼠标双击事件
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    Ui::FloatingWindow *ui;
    //记录鼠标位置,移动悬浮窗使用
    QPoint m_drag_position{0,0};
    //记录当前显示的进度条数量
    uint8_t m_bar_show_count;
    //刷新窗口显示
    void refresh_windows(void);

};

#endif // FLOATING_WINDOW_H
