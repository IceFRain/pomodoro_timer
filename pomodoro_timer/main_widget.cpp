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
    m_run_status = 0;
    m_work_time = 0;
    m_rest_time = 0;
    m_loop_last = 0;

    m_timer.setInterval(1000);
    //连接定时器
    connect(&m_timer, &QTimer::timeout, this, &MainWidget::slot_timer_timeout);
    //连接勾选框
    connect(ui->CB_time_floating_window_show,&QCheckBox::toggled,this,&MainWidget::slot_cb_floating_window_show_toggled);
    //连接按钮
    connect(ui->PB_ok,&QPushButton::clicked,this,&MainWidget::slot_pb_ok_clicked);
    connect(ui->PB_pause,&QPushButton::clicked,this,&MainWidget::slot_pb_pause_clicked);
    connect(ui->PB_continue,&QPushButton::clicked,this,&MainWidget::slot_pb_continue_clicked);

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
void MainWidget::slot_timer_timeout()
{
    m_loop_last --;
    if(m_loop_last==0)
    {
        switch (m_run_status)
        {
            //切换状态
            case 1:
                m_run_status = 0;
                m_loop_last = m_rest_time;
                m_floating_w.set_range(0,m_rest_time);
                m_tray_icon->showMessage("提示", "休息一下~");
            break;
            case 0:
                m_run_status = 1;
                m_loop_last = m_work_time;
                m_floating_w.set_range(0,m_work_time);
                m_tray_icon->showMessage("提示", "干活~");
            break;
            default:
            return;
        }
    }

    switch (m_run_status)
    {
        //工作状态
        case 1:
            m_tray_icon->setToolTip(QString("工作剩余 %1:%2")
                                    .arg(m_loop_last / 60)
                                    .arg(m_loop_last % 60));
        break;

        //休息状态
        case 0:
            m_tray_icon->setToolTip(QString("休息剩余 %1:%2")
                                    .arg(m_loop_last / 60)
                                    .arg(m_loop_last % 60));
        break;
    }
    m_floating_w.set_value(m_loop_last);
}

/**
  * @brief 运行按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_ok_clicked()
{
    m_run_status = 1;
    m_work_time = ui->LE_work_time->text().toUInt()*60;
    m_rest_time = ui->LE_rest_time->text().toUInt()*60;
    m_loop_last = m_work_time;
    m_floating_w.set_range(0,m_work_time);
    m_floating_w.set_value(m_work_time);
    m_timer.start();
    this->hide();
}

/**
  * @brief 暂停按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_pause_clicked()
{
    m_timer.stop();
}

/**
  * @brief 继续按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_continue_clicked()
{
    m_timer.start();
}

/**
  * @brief 显示悬浮窗勾选框改变槽
  * @param status 改变后的桩体
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_cb_floating_window_show_toggled(bool status)
{
    if(status)
    {
        m_floating_w.show();
    }
    else
    {
        m_floating_w.hide();
    }
}

void MainWidget::closeEvent(QCloseEvent *event)
{
    //拦截关闭事件 改为隐藏窗口
    this->hide();
    event->ignore();
}

