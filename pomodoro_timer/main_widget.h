#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QDebug>
#include "floating_window.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private slots:
    //定时器超时槽
    void slot_timer_timeout();
    //运行按钮点击槽
    void slot_pb_ok_clicked();
    //暂停按钮点击槽
    void slot_pb_pause_clicked();
    //继续按钮点击槽
    void slot_pb_continue_clicked();
    //显示悬浮窗按钮点击槽
    void slot_cb_floating_window_show_toggled(bool status);

private:
    Ui::MainWidget *ui;

    //悬浮窗界面
    FloatingWindow m_floating_w;

    //状态栏图标
    QSystemTrayIcon *m_tray_icon;
    //刷新显示定时器
    QTimer m_timer;
    //运行状态,0初始化,1工作,2休息
    uint8_t m_run_status;
    //工作时间
    uint32_t m_work_time;
    //休息时间
    uint32_t m_rest_time;
    //当前剩余时间
    uint32_t m_loop_last;

    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWIDGET_H
