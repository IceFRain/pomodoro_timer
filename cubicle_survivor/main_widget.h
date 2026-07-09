#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QDebug>
#include <QDate>
#include <QMessageBox>
#include <ShlObj.h>
#include <QDir>
#include <QScreen>
#include "floating_window.h"
#include "settings_window.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

//定义通知类型
enum class NotifyType {
    WorkOver,
    RestOver,
    GetDrinkGoal
};

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private slots:
    //更多设置按钮点击槽
    void slot_pb_more_settings_clicked(void);

    //时钟运行按钮点击槽
    void slot_pb_clock_start_clicked(void);
    //时钟暂停按钮点击槽
    void slot_pb_clock_pause_clicked(void);
    //时钟继续按钮点击槽
    void slot_pb_clock_continue_clicked(void);

    //记录一杯喝水按钮点击槽
    void slot_pb_drink_record_cup_clicked(void);
    //记录喝指定容量水按钮点击槽
    void slot_pb_drink_record_clicked(void);

    //复选框改变槽
    void slot_cb_toggled(void);

    //编辑框改变槽
    void slot_le_editing_finished(void);

    //定时器超时槽
    void slot_clock_timer_timeout();

private:
    Ui::MainWidget *ui;

    //悬浮窗界面
    FloatingWindow m_floating_w;
    //更多设置界面
    SettingsWindow m_settings_w;

    //状态栏图标
    QSystemTrayIcon *m_tray_icon;
    //刷新显示定时器
    QTimer m_clock_timer;
    //运行状态,0初始化,1工作,2休息
    uint8_t m_clock_run_status;
    //当前剩余时间
    uint32_t m_clock_loop_last;

    //设置项
    AllSettings *m_settings;

    //界面按配置初始化
    void init_ui_by_settings(void);

    //获取开机自启动路径
    QString get_auto_start_short_cut_path(void) const;
    //创建开机自启动
    bool creat_auto_start(void);
    //删除开机自启动
    bool delete_auto_start(void);    

    //喝水进度条显示数据
    void bar_drink_show(int value);

    //推送消息
    void push_message(NotifyType type);

    //重写窗口关闭事件
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWIDGET_H
