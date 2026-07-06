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
    setWindowTitle("番茄定时器 v1.0.0");

    //设置托盘图标
    m_tray_icon = new QSystemTrayIcon(this);
    m_tray_icon->setIcon(QIcon(":/icons/timer.png"));
    m_tray_icon->show();
    //设置浮窗界面
    m_floating_w.show();

    //添加并连接任务栏图标右键菜单
    QMenu *menu = new QMenu(this);
    QAction *quit_action = new QAction("退出", this);
    connect(quit_action, &QAction::triggered, qApp, &QApplication::quit);
    QAction *start_action = new QAction("开始", this);
    connect(start_action, &QAction::triggered, this, &MainWidget::slot_pb_ok_clicked);
    QAction *pause_action = new QAction("暂停", this);
    connect(pause_action, &QAction::triggered, this, &MainWidget::slot_pb_pause_clicked);
    QAction *continue_action = new QAction("继续", this);
    connect(continue_action, &QAction::triggered, this, &MainWidget::slot_pb_continue_clicked);
    //连接悬浮窗右键菜单
    connect(&m_floating_w,&FloatingWindow::sig_start_loop,this,&MainWidget::slot_pb_ok_clicked);
    connect(&m_floating_w,&FloatingWindow::sig_pause_loop,this,&MainWidget::slot_pb_pause_clicked);
    connect(&m_floating_w,&FloatingWindow::sig_continue_loop,this,&MainWidget::slot_pb_continue_clicked);
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
    read_setting_from_file();

    m_clock_timer.setInterval(1000);
    //连接定时器
    connect(&m_clock_timer, &QTimer::timeout, this, &MainWidget::slot_clock_timer_timeout);
    //连接勾选框
    connect(ui->CB_clock_floating_window_show,&QCheckBox::toggled,this,&MainWidget::slot_cb_dirnk_floating_window_show_toggled);
    //连接按钮
    connect(ui->PB_clock_start,&QPushButton::clicked,this,&MainWidget::slot_pb_ok_clicked);
    connect(ui->PB_clock_pause,&QPushButton::clicked,this,&MainWidget::slot_pb_pause_clicked);
    connect(ui->PB_clock_continue,&QPushButton::clicked,this,&MainWidget::slot_pb_continue_clicked);
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
}

/**
  * @brief 析构函数
  * @param 无
  * @retval 无
  * 	@arg
 */
MainWidget::~MainWidget()
{
    delete ui;
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
  * @brief 运行按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_ok_clicked()
{
    m_clock_run_status = 1;
    m_clock_loop_last = m_settings.clock_work_time;
    m_floating_w.set_clock_range(0,m_settings.clock_work_time);
    m_floating_w.set_clock_value(m_settings.clock_work_time);
    m_clock_timer.start();
}

/**
  * @brief 暂停按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_pause_clicked()
{
    m_clock_timer.stop();
}

/**
  * @brief 继续按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_continue_clicked()
{
    m_clock_timer.start();
}

/**
  * @brief 定时器显示悬浮窗勾选框改变槽
  * @param status 改变后的状态
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_cb_dirnk_floating_window_show_toggled(bool status)
{
    m_floating_w.set_bar_clock_show(status);
//    if(status)
//    {
//        m_floating_w.show();
//    }
//    else
//    {
//        m_floating_w.hide();
//    }
}

/**
  * @brief 编辑框改变槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_le_editing_finished()
{
    m_settings.clock_work_time = ui->LE_clock_work_time->text().toInt();
    m_settings.clock_rest_time = ui->LE_clock_rest_time->text().toInt();
    m_settings.drink_goal = ui->LE_drink_goal->text().toInt();
    m_settings.drink_cup_capacity = ui->LE_drink_cup_capacity->text().toInt();
    save_setting_to_file();
}

/**
  * @brief 读取配置文件
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::read_setting_from_file()
{
    QFile file("settings.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning()<<"settings.json失败,使用默认参数";
        m_settings.clock_work_time = 25*60;
        m_settings.clock_rest_time = 5*60;
        m_settings.drink_goal = 1600;
        m_settings.drink_cup_capacity = 500;
        return;
    }

    QByteArray data = file.readAll();
    file.close();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        qWarning()<<"settings.json解析错误:"<<parseError.errorString()<<",使用默认参数";
        m_settings.clock_work_time = 25*60;
        m_settings.clock_rest_time = 5*60;
        m_settings.drink_goal = 1600;
        m_settings.drink_cup_capacity = 500;
        return;
    }

    QJsonObject root = doc.object();
    //解析settings
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
}

/**
  * @brief 保存配置文件
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::save_setting_to_file()
{
    QJsonObject root;

    //settings结构
    QJsonObject  settings;
    settings["clock_work_time"] = m_settings.clock_work_time;
    settings["clock_rest_time"] = m_settings.clock_rest_time;
    settings["drink_goal"] = m_settings.drink_goal;
    settings["drink_cup_capacity"] = m_settings.drink_cup_capacity;
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

