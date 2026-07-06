#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "floating_window.h"

//设置项结构体
typedef struct
{
    int clock_work_time;
    int clock_rest_time;
    int drink_goal;
    int drink_cup_capacity;
}Settings;

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
    void slot_clock_timer_timeout();

    //运行按钮点击槽
    void slot_pb_ok_clicked();
    //暂停按钮点击槽
    void slot_pb_pause_clicked();
    //继续按钮点击槽
    void slot_pb_continue_clicked();
    //显示悬浮窗按钮点击槽
    void slot_cb_dirnk_floating_window_show_toggled(bool status);

    //编辑框改变槽
    void slot_le_editing_finished(void);

private:
    Ui::MainWidget *ui;

    //悬浮窗界面
    FloatingWindow m_floating_w;

    //状态栏图标
    QSystemTrayIcon *m_tray_icon;
    //刷新显示定时器
    QTimer m_clock_timer;
    //运行状态,0初始化,1工作,2休息
    uint8_t m_clock_run_status;
    //当前剩余时间
    uint32_t m_clock_loop_last;

    //设置项
    Settings m_settings;

    //读取保存的参数配置
    void read_setting_from_file(void);
    //保存参数配置
    void save_setting_to_file(void);

    //重写窗口关闭事件
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWIDGET_H
