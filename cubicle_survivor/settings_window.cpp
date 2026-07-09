#include "settings_window.h"
#include "ui_settings_window.h"

/**
  * @brief 构造函数
  * @param parent:父窗口
  * @retval 无
  * 	@arg
 */
SettingsWindow::SettingsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    //设置窗口名
    setWindowTitle("设置");

    //设置列表文字居中
    for (int i = 0; i < ui->LW_list->count(); i++)
    {
        ui->LW_list->item(i)->setTextAlignment(Qt::AlignCenter);
    }
    //设置版本号
    ui->L_version->setText(SOFT_VERSION);
    QString bt = QString(__DATE__ " " __TIME__).simplified();
    QDateTime dt = QLocale(QLocale::English).toDateTime(bt, "MMM d yyyy HH:mm:ss");
    ui->L_build_time->setText(dt.toString("yyyy.MM.dd HH:mm:ss"));

    //设置列表选择和页面切换连接
    connect(ui->LW_list,&QListWidget::currentRowChanged,ui->SW_settings,&QStackedWidget::setCurrentIndex);

    //设置按钮连接
    connect(ui->PB_project_url,&QPushButton::clicked,this,&SettingsWindow::slot_pb_project_url_clicked);
    connect(ui->PB_reset_settings,&QPushButton::clicked,this,&SettingsWindow::slot_pb_reset_settings_clicked);

    //复选框连接
    connect(ui->CB_notice_select_clock,&QCheckBox::toggled,this,&SettingsWindow::slot_cb_toggled);
    connect(ui->CB_notice_select_drink,&QCheckBox::toggled,this,&SettingsWindow::slot_cb_toggled);
    connect(ui->CB_notice_type_message_box,&QCheckBox::toggled,this,&SettingsWindow::slot_cb_toggled);
    connect(ui->CB_notice_type_sys_notice,&QCheckBox::toggled,this,&SettingsWindow::slot_cb_toggled);

    //输入框连接
    connect(ui->LE_sys_notice_time,&QLineEdit::editingFinished,this,&SettingsWindow::slot_le_editing_finished);
    connect(ui->LE_custom_work_over_notify,&QLineEdit::editingFinished,this,&SettingsWindow::slot_le_editing_finished);
    connect(ui->LE_custom_rest_over_notify,&QLineEdit::editingFinished,this,&SettingsWindow::slot_le_editing_finished);
    connect(ui->LE_custom_get_drink_goal_notify,&QLineEdit::editingFinished,this,&SettingsWindow::slot_le_editing_finished);

    //默认选择第一组设置项
    ui->LW_list->setCurrentRow(0);
}

/**
  * @brief 析构函数
  * @param 无
  * @retval 无
  * 	@arg
 */
SettingsWindow::~SettingsWindow()
{
    delete ui;
}

/**
  * @brief 获取设置项指针
  * @param 无
  * @retval 设置项指针
  * 	@arg
 */
AllSettings *SettingsWindow::get_settings_pointer()
{
    return &m_settings;
}

/**
  * @brief 从文件中读取设置
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::read_settings_from_file()
{
    QFile file("settings.json");
    QJsonObject root;
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "settings.json open failed, use default settings";
    }
    else
    {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
        file.close();

        if (error.error == QJsonParseError::NoError)
        {
            root = doc.object();
        }
        else
        {
            qDebug() << "JSON parse failed:" << error.errorString()<<", use default settings";
        }
    }

    QJsonObject clock  = root["clock"].toObject();
    QJsonObject drink  = root["drink"].toObject();
    QJsonObject notice = root["notice"].toObject();

    m_settings.clock.work_time     = clock["work_time"].toInt(25);
    m_settings.clock.rest_time     = clock["rest_time"].toInt(5);
    m_settings.clock.floating_show = clock["floating_show"].toInt(1);

    m_settings.drink.goal          = drink["goal"].toInt(1800);
    m_settings.drink.cup_capacity  = drink["cup_capacity"].toInt(500);
    m_settings.drink.record        = drink["record"].toInt(0);
    m_settings.drink.floating_show = drink["floating_show"].toInt(1);
    m_settings.drink.record_date   = QDate::fromString(drink["record_date"].toString(), Qt::ISODate);
    QDate today = QDate::currentDate();
    if (!m_settings.drink.record_date.isValid() || m_settings.drink.record_date != today)
    {
        m_settings.drink.record_date = QDate::currentDate();
        m_settings.drink.record = 0;
    }

    m_settings.notice.clock_notice_enable  = notice["clock_notice_enable"].toInt(1);
    m_settings.notice.drink_notice_enable  = notice["drink_notice_enable"].toInt(1);
    m_settings.notice.messge_box_enable    = notice["message_box_enable"].toInt(0);
    m_settings.notice.sys_notice_enable    = notice["sys_notice_enable"].toInt(1);
    m_settings.notice.sys_notice_keep_time = notice["sys_notice_keep_time"].toInt(30);
    m_settings.notice.clock_work_over_tips = notice["clock_work_over_tips"].toString("休息一下~");
    m_settings.notice.clock_rest_over_tips = notice["clock_rest_over_tips"].toString("干活~");
    m_settings.notice.get_drink_goal_tips  = notice["get_drink_goal_tips"].toString("达成今日喝水目标~");

    init_ui_by_settings();
}

/**
  * @brief 写入设置到文件
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::save_settings_to_file()
{
    QJsonObject clock;
    clock["work_time"]     = m_settings.clock.work_time;
    clock["rest_time"]     = m_settings.clock.rest_time;
    clock["floating_show"] = m_settings.clock.floating_show;

    QJsonObject drink;
    drink["goal"]          = m_settings.drink.goal;
    drink["cup_capacity"]  = m_settings.drink.cup_capacity;
    drink["record"]        = m_settings.drink.record;
    drink["floating_show"] = m_settings.drink.floating_show;
    drink["record_date"]   = m_settings.drink.record_date.toString(Qt::ISODate);

    QJsonObject notice;
    notice["clock_notice_enable"]   = m_settings.notice.clock_notice_enable;
    notice["drink_notice_enable"]   = m_settings.notice.drink_notice_enable;
    notice["message_box_enable"]   = m_settings.notice.messge_box_enable;
    notice["sys_notice_enable"]    = m_settings.notice.sys_notice_enable;
    notice["sys_notice_keep_time"] = m_settings.notice.sys_notice_keep_time;
    notice["clock_work_over_tips"] = m_settings.notice.clock_work_over_tips;
    notice["clock_rest_over_tips"] = m_settings.notice.clock_rest_over_tips;
    notice["get_drink_goal_tips"]  = m_settings.notice.get_drink_goal_tips;

    QJsonObject root;
    root["clock"]  = clock;
    root["drink"]  = drink;
    root["notice"] = notice;

    QFile file("settings.json");
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug()<<"settings.json open failed";
        return;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();
}

/**
  * @brief 项目官网按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::slot_pb_project_url_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/IceFRain/cubicle-survivor"));
}

/**
  * @brief 设置初始化按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::slot_pb_reset_settings_clicked()
{
    //二次确认
    QMessageBox::StandardButton btn = QMessageBox::question(
        this, "确认", "要重置所有设置吗？");

    if (btn == QMessageBox::Yes)
    {
        //删除当前设置文件后,重新读取数据,设置为默认值
        QFile::remove("./settings.json");
        read_settings_from_file();
        save_settings_to_file();
        emit sig_settings_changed();
    }
}

/**
  * @brief 复选框状态变化槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::slot_cb_toggled()
{
    QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
    if(!cb)
    {
        return;
    }

    //番茄时钟通知使能
    if(cb == ui->CB_notice_select_clock)
    {
        m_settings.notice.clock_notice_enable = ui->CB_notice_select_clock->isChecked();
    }
    //喝水通知使能
    else if(cb == ui->CB_notice_select_drink)
    {
        m_settings.notice.drink_notice_enable = ui->CB_notice_select_drink->isChecked();
    }
    //系统通知模式使能
    else if(cb == ui->CB_notice_type_message_box)
    {
        m_settings.notice.messge_box_enable = ui->CB_notice_type_message_box->isChecked();
    }
    //弹窗通知模式使能
    else if(cb == ui->CB_notice_type_sys_notice)
    {
        m_settings.notice.sys_notice_enable = ui->CB_notice_type_sys_notice->isChecked();
    }

    save_settings_to_file();
}

/**
  * @brief 编辑框编辑完成槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::slot_le_editing_finished()
{
    QLineEdit *le = qobject_cast<QLineEdit*>(sender());
    if(!le)
    {
        return;
    }

    //通知停留时间
    if(le == ui->LE_sys_notice_time)
    {
        m_settings.notice.sys_notice_keep_time = ui->LE_sys_notice_time->text().toInt();
        if(m_settings.notice.sys_notice_keep_time<0)
        {
            m_settings.notice.sys_notice_keep_time = 0;
            ui->LE_sys_notice_time->setText("0");
        }
    }
    //工作时间结束提示信息
    else if(le == ui->LE_custom_work_over_notify)
    {
        m_settings.notice.clock_work_over_tips = ui->LE_custom_work_over_notify->text();
    }
    //休息时间结束提示信息
    else if(le == ui->LE_custom_rest_over_notify)
    {
        m_settings.notice.clock_rest_over_tips = ui->LE_custom_rest_over_notify->text();
    }
    //喝水达到目标提示信息
    else if(le == ui->LE_custom_get_drink_goal_notify)
    {
        m_settings.notice.get_drink_goal_tips = ui->LE_custom_get_drink_goal_notify->text();
    }

    save_settings_to_file();
}

/**
  * @brief 界面按配置初始化
  * @param 无
  * @retval 无
  * 	@arg
 */
void SettingsWindow::init_ui_by_settings()
{
    ui->CB_notice_select_clock->setChecked(m_settings.notice.clock_notice_enable);
    ui->CB_notice_select_drink->setChecked(m_settings.notice.drink_notice_enable);
    ui->CB_notice_type_message_box->setChecked(m_settings.notice.messge_box_enable);
    ui->CB_notice_type_sys_notice->setChecked(m_settings.notice.sys_notice_enable);

    ui->LE_sys_notice_time->setText(QString::number(m_settings.notice.sys_notice_keep_time));
    ui->LE_custom_work_over_notify->setText(m_settings.notice.clock_work_over_tips);
    ui->LE_custom_rest_over_notify->setText(m_settings.notice.clock_rest_over_tips);
    ui->LE_custom_get_drink_goal_notify->setText(m_settings.notice.get_drink_goal_tips);
}



