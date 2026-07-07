#include "main_widget.h"
#include "ui_main_widget.h"
#include <QMenu>
#include <QAction>

/**
  * @brief 构造函数
  * @param parent:父窗口
  * @retval 无
  * 	@arg
 */
MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    //设置窗口名
    setWindowTitle("格子间幸存者");

    //设置托盘图标
    m_tray_icon = new QSystemTrayIcon(this);
    m_tray_icon->setIcon(QIcon(":/icons/cup.png"));
    m_tray_icon->show();
    //设置浮窗界面
    m_floating_w.show();

    //添加并连接任务栏图标右键菜单
    QMenu *menu = new QMenu(this);
    QAction *quit_action = new QAction("退出", this);
    connect(quit_action, &QAction::triggered, qApp, &QApplication::quit);
    QAction *start_action = new QAction("开始", this);
    connect(start_action, &QAction::triggered, this, &MainWidget::slot_pb_clock_start_clicked);
    QAction *pause_action = new QAction("暂停", this);
    connect(pause_action, &QAction::triggered, this, &MainWidget::slot_pb_clock_pause_clicked);
    QAction *continue_action = new QAction("继续", this);
    connect(continue_action, &QAction::triggered, this, &MainWidget::slot_pb_clock_continue_clicked);
    //连接时钟悬浮窗右键菜单
    connect(&m_floating_w,&FloatingWindow::sig_clock_start_loop,this,&MainWidget::slot_pb_clock_start_clicked);
    connect(&m_floating_w,&FloatingWindow::sig_clock_pause_loop,this,&MainWidget::slot_pb_clock_pause_clicked);
    connect(&m_floating_w,&FloatingWindow::sig_clock_continue_loop,this,&MainWidget::slot_pb_clock_continue_clicked);
    //连接饮水悬浮窗右键菜单
    connect(&m_floating_w,&FloatingWindow::sig_drink_record_cup,this,&MainWidget::slot_pb_drink_record_cup_clicked);
    //连接悬浮窗双击动作
    connect(&m_floating_w, &FloatingWindow::sig_double_clicked,this, [this](){
                    this->showNormal();   // 恢复窗口
                    this->raise();        // 提到最前
                    this->activateWindow(); // 激活窗口
                });

    menu->addAction(start_action);
    menu->addAction(pause_action);
    menu->addAction(continue_action);
    menu->addAction(quit_action);
    m_tray_icon->setContextMenu(menu);

    //数据初始化
    m_clock_run_status = 0;
    m_clock_loop_last = 0;
    read_settings_from_file();
    //时钟定时器间隔1s
    m_clock_timer.setInterval(1000);

    //连接定时器
    connect(&m_clock_timer, &QTimer::timeout, this, &MainWidget::slot_clock_timer_timeout);
    //连接复选框
    connect(ui->CB_clock_floating_window_show,&QCheckBox::toggled,this,&MainWidget::slot_cb_clock_floating_window_show_toggled);
    connect(ui->CB_drink_floating_window_show,&QCheckBox::toggled,this,&MainWidget::slot_cb_drink_floating_window_show_toggled);
    //连接按钮
    connect(ui->PB_version,&QPushButton::clicked,this,&MainWidget::slot_pb_version_clicked);
    connect(ui->PB_project_url,&QPushButton::clicked,this,&MainWidget::slot_pb_project_url_clicked);
    connect(ui->PB_clock_start,&QPushButton::clicked,this,&MainWidget::slot_pb_clock_start_clicked);
    connect(ui->PB_clock_pause,&QPushButton::clicked,this,&MainWidget::slot_pb_clock_pause_clicked);
    connect(ui->PB_clock_continue,&QPushButton::clicked,this,&MainWidget::slot_pb_clock_continue_clicked);
    connect(ui->PB_drink_record_cup,&QPushButton::clicked,this,&MainWidget::slot_pb_drink_record_cup_clicked);
    connect(ui->PB_drink_record,&QPushButton::clicked,this,&MainWidget::slot_pb_drink_record_clicked);
    //连接编辑框
    connect(ui->LE_clock_work_time,&QLineEdit::editingFinished,this,&MainWidget::slot_le_editing_finished);
    connect(ui->LE_clock_rest_time,&QLineEdit::editingFinished,this,&MainWidget::slot_le_editing_finished);
    connect(ui->LE_drink_goal,&QLineEdit::editingFinished,this,&MainWidget::slot_le_editing_finished);
    connect(ui->LE_drink_cup_capacity,&QLineEdit::editingFinished,this,&MainWidget::slot_le_editing_finished);

    //任务图标双击动作
    connect(m_tray_icon, &QSystemTrayIcon::activated,this, [this](QSystemTrayIcon::ActivationReason reason){
                if (reason == QSystemTrayIcon::DoubleClick) {
                    this->showNormal();   // 恢复窗口
                    this->raise();        // 提到最前
                    this->activateWindow(); // 激活窗口
                }});

    //样式设计
    ui->B_drink->setStyleSheet(
                "QProgressBar { "
                " border: 1px solid grey; "
                " text-align: center; "
                " font: 12pt 'MiSans'; "
                "} "
                "QProgressBar::chunk { "
                " background-color: lightblue; "
                " width: 1px; "
                "}"
            );
}

/**
  * @brief 析构函数
  * @param 无
  * @retval 无
  * 	@arg
 */
MainWidget::~MainWidget()
{
    m_clock_timer.disconnect();
    delete ui;
}

/**
  * @brief 版本信息按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_version_clicked()
{
    QString version_str = "Designed by IceFRain\n";
    version_str = version_str + "Version: v1.0.0\n";
    QString bt = QString(__DATE__ " " __TIME__).simplified();
    QDateTime dt = QLocale(QLocale::English).toDateTime(bt, "MMM d yyyy HH:mm:ss");
    version_str = version_str + "Build time:" + dt.toString("yyyy.MM.dd HH:mm:ss");

    QMessageBox::information(this, "版本信息", version_str);
}

/**
  * @brief 项目官网按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_project_url_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/IceFRain/cubicle-survivor"));
}

/**
  * @brief 时钟运行按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_clock_start_clicked()
{
    m_clock_run_status = 1;
    m_clock_loop_last = m_settings.clock_work_time;
    m_floating_w.set_clock_range(0,m_settings.clock_work_time);
    m_floating_w.set_clock_value(m_settings.clock_work_time);
    m_clock_timer.start();
}

/**
  * @brief 时钟暂停按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_clock_pause_clicked()
{
    m_clock_timer.stop();
}

/**
  * @brief 时钟继续按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_clock_continue_clicked()
{
    m_clock_timer.start();
}

/**
  * @brief 记录一杯饮水按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_drink_record_cup_clicked()
{
    bar_drink_show(m_settings.drink_cup_capacity);
}

/**
  * @brief 记录指定容量饮水按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_drink_record_clicked()
{
    bar_drink_show(ui->LE_drink_record->text().toInt());
}

/**
  * @brief 定时器显示悬浮窗勾选框改变槽
  * @param status 改变后的状态
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_cb_clock_floating_window_show_toggled(bool status)
{
    m_floating_w.set_bar_clock_show(status);
}

/**
  * @brief 饮水记录悬浮窗显示复选框改变槽
  * @param status 改变后的状态
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_cb_drink_floating_window_show_toggled(bool status)
{
    m_floating_w.set_bar_drink_show(status);
}

/**
  * @brief 编辑框改变槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_le_editing_finished()
{
    m_settings.clock_work_time = ui->LE_clock_work_time->text().toInt()*60;
    m_settings.clock_rest_time = ui->LE_clock_rest_time->text().toInt()*60;
    if(m_settings.drink_goal != ui->LE_drink_goal->text().toInt())
    {
        m_settings.drink_goal = ui->LE_drink_goal->text().toInt();
        ui->B_drink->setRange(0,m_settings.drink_goal);
        m_floating_w.set_drink_range(0,m_settings.drink_goal);
    }
    m_settings.drink_cup_capacity = ui->LE_drink_cup_capacity->text().toInt();
    m_settings.clock_floating_show = ui->CB_clock_floating_window_show->isChecked();
    m_settings.drink_floating_show = ui->CB_drink_floating_window_show->isChecked();
    bar_drink_show(0);
    save_settings_to_file();
}

/**
  * @brief 定时器超时槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_clock_timer_timeout()
{
    m_clock_loop_last --;
    if(m_clock_loop_last==0)
    {
        switch (m_clock_run_status)
        {
            //切换状态
            case 1:
                m_clock_run_status = 0;
                m_clock_loop_last = m_settings.clock_rest_time;
                m_floating_w.set_clock_range(0,m_settings.clock_rest_time);
                m_tray_icon->showMessage("提示", "休息一下~");
            break;
            case 0:
                m_clock_run_status = 1;
                m_clock_loop_last = m_settings.clock_work_time;
                m_floating_w.set_clock_range(0,m_settings.clock_work_time);
                m_tray_icon->showMessage("提示", "干活~");
            break;
            default:
            return;
        }
    }

    switch (m_clock_run_status)
    {
        //工作状态
        case 1:
            m_tray_icon->setToolTip(QString("工作剩余 %1:%2")
                                    .arg(m_clock_loop_last / 60)
                                    .arg(m_clock_loop_last % 60));
        break;

        //休息状态
        case 0:
            m_tray_icon->setToolTip(QString("休息剩余 %1:%2")
                                    .arg(m_clock_loop_last / 60)
                                    .arg(m_clock_loop_last % 60));
        break;
    }
    m_floating_w.set_clock_value(m_clock_loop_last);
}

/**
  * @brief 读取配置文件
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::read_settings_from_file()
{
    QFile file("settings.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning()<<"打开settings.json失败,使用默认参数";
        set_to_default_settings();
        return;
    }

    QByteArray data = file.readAll();
    file.close();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        qWarning()<<"settings.json解析错误:"<<parseError.errorString()<<",使用默认参数";
        set_to_default_settings();
        return;
    }

    //解析settings
    QJsonObject root = doc.object();
    QJsonObject settings = root["settings"].toObject();

    m_settings.clock_work_time = settings["clock_work_time"].toInt();
    ui->LE_clock_work_time->setText(QString::number(m_settings.clock_work_time));
    m_settings.clock_work_time = m_settings.clock_work_time*60;
    m_settings.clock_rest_time = settings["clock_rest_time"].toInt();
    ui->LE_clock_rest_time->setText(QString::number(m_settings.clock_rest_time));
    m_settings.clock_rest_time = m_settings.clock_rest_time*60;

    m_settings.drink_goal = settings["drink_goal"].toInt();
    ui->LE_drink_goal->setText(QString::number(m_settings.drink_goal));
    m_settings.drink_cup_capacity = settings["drink_cup_capacity"].toInt();
    ui->LE_drink_cup_capacity->setText(QString::number(m_settings.drink_cup_capacity));

    m_settings.clock_floating_show = settings["clock_floating_show"].toInt();
    ui->CB_clock_floating_window_show->setChecked(m_settings.clock_floating_show);
    m_floating_w.set_bar_clock_show(m_settings.clock_floating_show);
    m_settings.drink_floating_show = settings["drink_floating_show"].toInt();
    ui->CB_drink_floating_window_show->setChecked(m_settings.drink_floating_show);
    m_floating_w.set_bar_drink_show(m_settings.drink_floating_show);

    m_settings.drink_record_date = QDate::fromString(settings["drink_record_date"].toString(), "yyyy-MM-dd");
    QDate today = QDate::currentDate();
    if (!m_settings.drink_record_date.isValid() || m_settings.drink_record_date != today)
    {
        //新的一天drink_record置零
        m_settings.drink_record_date = today;
        m_settings.drink_record = 0;
    }
    else
    {
        m_settings.drink_record = settings["drink_record"].toInt();
    }
    ui->B_drink->setRange(0,m_settings.drink_goal);
    //drink_record已获取,此处只刷新显示,变化量传0
    bar_drink_show(0);
    m_floating_w.set_drink_range(0,m_settings.drink_goal);
    m_floating_w.set_drink_value(m_settings.drink_record);
}

/**
  * @brief 保存配置文件
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::save_settings_to_file()
{
    QJsonObject root;

    //settings结构
    QJsonObject  settings;
    settings["clock_work_time"] = m_settings.clock_work_time/60;
    settings["clock_rest_time"] = m_settings.clock_rest_time/60;
    settings["drink_goal"] = m_settings.drink_goal;
    settings["drink_cup_capacity"] = m_settings.drink_cup_capacity;
    settings["clock_floating_show"] = m_settings.clock_floating_show;
    settings["drink_floating_show"] = m_settings.drink_floating_show;
    settings["drink_record"] = m_settings.drink_record;
    settings["drink_record_date"] = m_settings.drink_record_date.toString("yyyy-MM-dd");
    root["settings"] = settings;

    //转成JSON文档
    QJsonDocument doc(root);
    //打开写入文件
    QFile file("settings.json");
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning()<<"settings.json进行写入";
        return;
    }

    //美化缩进
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

/**
  * @brief 设置默认参数
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::set_to_default_settings()
{
    m_settings.clock_work_time = 25*60;
    m_settings.clock_rest_time = 5*60;
    m_settings.drink_goal = 1600;
    m_settings.drink_record = 0;
    m_settings.drink_cup_capacity = 500;
    m_settings.clock_floating_show = 1;
    m_settings.drink_floating_show = 1;
    m_settings.drink_record_date = QDate::currentDate();
    ui->B_drink->setRange(0,m_settings.drink_goal);
    ui->B_drink->setValue(m_settings.drink_record);
}

/**
  * @brief 饮水进度条显示数据
  * @param value 饮水变化值
  * @retval 无
  * 	@arg
 */
void MainWidget::bar_drink_show(int value)
{
    if(m_settings.drink_record < m_settings.drink_goal
       && (m_settings.drink_record + value) >= m_settings.drink_goal)
    {
        m_tray_icon->showMessage("恭喜", "达成今日饮水目标~");
    }

    m_settings.drink_record += value;
    if(m_settings.drink_record < 0)
    {
        m_settings.drink_record = 0;
    }
    if(m_settings.drink_record < m_settings.drink_goal)
    {
        ui->B_drink->setValue(m_settings.drink_record);
    }
    else
    {
        ui->B_drink->setValue(m_settings.drink_goal);
    }
    ui->B_drink->setFormat(QString("%1 / %2 ml").arg(m_settings.drink_record).arg(m_settings.drink_goal));

    m_floating_w.set_drink_value(m_settings.drink_record);
    save_settings_to_file();
}

/**
  * @brief 窗口关闭事件
  * @param event 事件
  * @retval 无
  * 	@arg
 */
void MainWidget::closeEvent(QCloseEvent *event)
{
    //拦截关闭事件 改为隐藏窗口
    this->hide();
    event->ignore();
}

