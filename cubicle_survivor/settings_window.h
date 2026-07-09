#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <QWidget>
#include <QUrl>
#include <QDesktopServices>
#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QMessageBox>

//时钟设置项结构体
typedef struct
{
    int work_time;
    int rest_time;
    short floating_show;
}ClockSettings;
//喝水设置项结构体
typedef struct
{
    int goal;
    int cup_capacity;
    int record;
    short floating_show;
    QDate record_date;
}DrinkSettings;
//消息通知设置项结构体
typedef struct
{
    short clock_notice_enable;
    short drink_notice_enable;
    short messge_box_enable;
    short sys_notice_enable;
    short sys_notice_keep_time;
    QString clock_work_over_tips;
    QString clock_rest_over_tips;
    QString get_drink_goal_tips;
}NoticeSettings;
//设置项结构体
typedef struct
{
    ClockSettings clock;
    DrinkSettings drink;
    NoticeSettings notice;
}AllSettings;


#define SOFT_VERSION    "V1.1.3"


namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();
    //获取设置项指针,允许外部修改
    AllSettings* get_settings_pointer(void);
    //读取保存的参数配置
    void read_settings_from_file(void);
    //保存参数配置
    void save_settings_to_file(void);

public slots:
    //项目官网按钮点击槽
    void slot_pb_project_url_gitee_clicked(void);
    void slot_pb_project_url_github_clicked(void);
    //设置初始化按钮点击槽
    void slot_pb_reset_settings_clicked(void);

    //复选框改变槽
    void slot_cb_toggled(void);
    //编辑框编辑完成槽
    void slot_le_editing_finished(void);

signals:
    //设置改变信号
    void sig_settings_changed(void);

private:
    Ui::SettingsWindow *ui;
    //保存设置项
    AllSettings m_settings;

    //界面按配置初始化
    void init_ui_by_settings(void);
};

#endif // SETTINGS_WINDOW_H



