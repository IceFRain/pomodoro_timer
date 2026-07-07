#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>
#include <QMessageBox>
#include <QDesktopServices>
#include <ShlObj.h>
#include <QDir>
#include "floating_window.h"

//设置项结构体
typedef struct
{
    int clock_work_time;
    int clock_rest_time;
    int drink_goal;
    int drink_cup_capacity;
    int drink_record;
    int clock_floating_show;
    int drink_floating_show;
    QDate drink_record_date;
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
    //版本信息按钮点击槽
    void slot_pb_version_clicked(void);
    //项目官网按钮点击槽
    void slot_pb_project_url_clicked(void);

    //时钟运行按钮点击槽
    void slot_pb_clock_start_clicked(void);
    //时钟暂停按钮点击槽
    void slot_pb_clock_pause_clicked(void);
    //时钟继续按钮点击槽
    void slot_pb_clock_continue_clicked(void);

    //记录一杯饮水按钮点击槽
    void slot_pb_drink_record_cup_clicked(void);
    //记录指定容量饮水按钮点击槽
    void slot_pb_drink_record_clicked(void);

    //时钟悬浮窗显示复选框改变槽
    void slot_cb_clock_floating_window_show_toggled(bool status);
    //饮水记录悬浮窗显示复选框改变槽
    void slot_cb_drink_floating_window_show_toggled(bool status);
    //自启动复选框改变槽
    void slot_cb_auto_start_toggled(bool status);

    //编辑框改变槽
    void slot_le_editing_finished(void);

    //定时器超时槽
    void slot_clock_timer_timeout();

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
    void read_settings_from_file(void);
    //保存参数配置
    void save_settings_to_file(void);
    //设置默认参数
    void set_to_default_settings(void);

    //获取开机自启动路径
    QString get_auto_start_short_cut_path(void) const;
    //创建开机自启动
    bool creat_auto_start(void);
    //删除开机自启动
    bool delete_auto_start(void);


    //饮水进度条显示数据
    void bar_drink_show(int value);

    //重写窗口关闭事件
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWIDGET_H
