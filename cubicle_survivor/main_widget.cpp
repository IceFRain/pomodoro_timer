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

    //默认托盘图标
    m_tray_icon = new QSystemTrayIcon(this);
    m_tray_icon->setIcon(QIcon(":/icons/ghanta.png"));
    m_tray_icon->show();
    //默认显示浮窗界面
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
    QAction *record_cup_action = new QAction("记一杯", this);
    connect(record_cup_action, &QAction::triggered, this, &MainWidget::slot_pb_drink_record_cup_clicked);
    menu->addAction(start_action);
    menu->addAction(pause_action);
    menu->addAction(continue_action);
    menu->addAction(record_cup_action);
    menu->addAction(quit_action);
    m_tray_icon->setContextMenu(menu);
    //连接时钟悬浮窗右键菜单
    connect(&m_floating_w,&FloatingWindow::sig_clock_start_loop,this,&MainWidget::slot_pb_clock_start_clicked);
    connect(&m_floating_w,&FloatingWindow::sig_clock_pause_loop,this,&MainWidget::slot_pb_clock_pause_clicked);
    connect(&m_floating_w,&FloatingWindow::sig_clock_continue_loop,this,&MainWidget::slot_pb_clock_continue_clicked);
    //连接喝水悬浮窗右键菜单
    connect(&m_floating_w,&FloatingWindow::sig_drink_record_cup,this,&MainWidget::slot_pb_drink_record_cup_clicked);
    //连接悬浮窗双击动作
    connect(&m_floating_w, &FloatingWindow::sig_double_clicked,this, [this](){
                    this->showNormal();   // 恢复窗口
                    this->raise();        // 提到最前
                    this->activateWindow(); // 激活窗口
                });

    //数据初始化
    m_clock_run_status = 0;
    m_clock_loop_last = 0;
    m_settings = m_settings_w.get_settings_pointer();
    m_settings_w.read_settings_from_file();
    init_ui_by_settings();
    connect(&m_settings_w,&SettingsWindow::sig_settings_changed,this,&MainWidget::init_ui_by_settings);
    //时钟定时器间隔1s
    m_clock_timer.setInterval(1000);

    //连接定时器
    connect(&m_clock_timer, &QTimer::timeout, this, &MainWidget::slot_clock_timer_timeout);
    //连接复选框
    connect(ui->CB_clock_floating_window_show,&QCheckBox::toggled,this,&MainWidget::slot_cb_toggled);
    connect(ui->CB_drink_floating_window_show,&QCheckBox::toggled,this,&MainWidget::slot_cb_toggled);
    connect(ui->CB_auto_start,&QCheckBox::toggled,this,&MainWidget::slot_cb_toggled);
    //连接按钮
    connect(ui->PB_more_settings,&QPushButton::clicked,this,&MainWidget::slot_pb_more_settings_clicked);
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
    connect(ui->LE_drink_record,&QLineEdit::returnPressed,this,&MainWidget::slot_pb_drink_record_clicked);

    //任务图标双击动作
    connect(m_tray_icon, &QSystemTrayIcon::activated,this, [this](QSystemTrayIcon::ActivationReason reason){
                if (reason == QSystemTrayIcon::DoubleClick) {
                    this->showNormal();   // 恢复窗口
                    this->raise();        // 提到最前
                    this->activateWindow(); // 激活窗口
                }});

    //进度条样式设计
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
    m_floating_w.disconnect();
    m_settings_w.disconnect();
    m_tray_icon->disconnect();
    delete ui;
}

/**
  * @brief 更多设置按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_more_settings_clicked()
{
    m_settings_w.showNormal();
    m_settings_w.raise();
    m_settings_w.activateWindow();
}

/**
  * @brief 时钟运行按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_clock_start_clicked()
{
    //切换到运行状态
    m_clock_run_status = 1;
    //从工作时间开始倒计时
    m_clock_loop_last = m_settings->clock.work_time*60;
    //悬浮窗进度条设置
    m_floating_w.set_clock_range(0,m_clock_loop_last);
    m_floating_w.set_clock_value(m_clock_loop_last);
    //开始计时
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
  * @brief 记录一杯喝水按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_drink_record_cup_clicked()
{
    bar_drink_show(m_settings->drink.cup_capacity);
}

/**
  * @brief 记录喝指定容量水按钮点击槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_pb_drink_record_clicked()
{
    bar_drink_show(ui->LE_drink_record->text().toInt());
}

/**
  * @brief 复选框改变槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_cb_toggled()
{
    QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
    if(!cb)
    {
        return;
    }

    //番茄时钟悬浮窗使能
    if(cb == ui->CB_clock_floating_window_show)
    {
        m_floating_w.set_bar_clock_show(ui->CB_clock_floating_window_show->isChecked());
    }
    //喝水悬浮窗使能
    else if(cb == ui->CB_drink_floating_window_show)
    {
        m_floating_w.set_bar_drink_show(ui->CB_drink_floating_window_show->isChecked());
    }
    //开机自启
    else if(cb == ui->CB_auto_start)
    {
        if (ui->CB_auto_start->isChecked())
        {
            creat_auto_start();
        }
        else
        {
            delete_auto_start();
        }
    }
}

/**
  * @brief 编辑框编辑完成槽
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::slot_le_editing_finished()
{
    QLineEdit *le = qobject_cast<QLineEdit*>(sender());
    if(!le)
    {
        return;
    }

    //工作时间设置
    if(le == ui->LE_clock_work_time)
    {
        m_settings->clock.work_time = ui->LE_clock_work_time->text().toInt();
    }
    //休息时间设置
    else if(le == ui->LE_clock_rest_time)
    {
        m_settings->clock.rest_time = ui->LE_clock_rest_time->text().toInt();
    }
    //喝水目标设置
    else if(le == ui->LE_drink_goal)
    {
        m_settings->drink.goal = ui->LE_drink_goal->text().toInt();
        m_settings_w.save_settings_to_file();
        //进度条范围重新设置
        ui->B_drink->setRange(0,m_settings->drink.goal);
        m_floating_w.set_drink_range(0,m_settings->drink.goal);
        //刷新进度条显示
        bar_drink_show(0);
    }
    //喝水单杯容量设置
    else if(le == ui->LE_drink_cup_capacity)
    {
        m_settings->drink.cup_capacity = ui->LE_drink_cup_capacity->text().toInt();
    }

    m_settings_w.save_settings_to_file();
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
            //切换状态并进行提示
            case 1:
                m_clock_run_status = 0;
                m_clock_loop_last = m_settings->clock.rest_time*60;
                m_floating_w.set_clock_range(0,m_clock_loop_last);
                push_message(NotifyType::WorkOver);
            break;
            case 0:
                m_clock_run_status = 1;
                m_clock_loop_last = m_settings->clock.work_time*60;
                m_floating_w.set_clock_range(0,m_clock_loop_last);
                push_message(NotifyType::RestOver);
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
  * @brief 界面按配置初始化
  * @param 无
  * @retval 无
  * 	@arg
 */
void MainWidget::init_ui_by_settings()
{
    ui->LE_clock_work_time->setText(QString::number(m_settings->clock.work_time));
    ui->LE_clock_rest_time->setText(QString::number(m_settings->clock.rest_time));
    ui->CB_clock_floating_window_show->setChecked(m_settings->clock.floating_show);

    ui->LE_drink_goal->setText(QString::number(m_settings->drink.goal));
    ui->LE_drink_cup_capacity->setText(QString::number(m_settings->drink.cup_capacity));
    ui->CB_drink_floating_window_show->setChecked(m_settings->drink.floating_show);
    ui->B_drink->setRange(0,m_settings->drink.goal);

    m_floating_w.set_drink_range(0,m_settings->drink.goal);
    bar_drink_show(0);
    ui->CB_auto_start->setChecked(QFile::exists(get_auto_start_short_cut_path()));
}

/**
  * @brief 获取开机自启动路径
  * @param 无
  * @retval 无
  * 	@arg
 */
QString MainWidget::get_auto_start_short_cut_path() const
{
    wchar_t buf[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_STARTUP, NULL, 0, buf);
    return QString::fromWCharArray(buf)
           + "\\" + QCoreApplication::applicationName() + ".lnk";
}

/**
  * @brief 创建开机自启动
  * @param 无
  * @retval 是否成功
  * 	@arg true成功 false失败
 */
bool MainWidget::creat_auto_start()
{
    //CoCreateInstance创建一个ShellLink COM实例,用于构建.lnk快捷方式
    IShellLinkW *psl = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                  IID_IShellLinkW, (void**)&psl);
    if (FAILED(hr))
    {
        //COM创建失败 直接返回
        return false;
    }

    //目标路径指向程序自身的exe路径
    psl->SetPath(QCoreApplication::applicationFilePath().toStdWString().c_str());
    //工作目录 设为exe所在文件夹(路径转为原生分隔符\)
    psl->SetWorkingDirectory(
        QDir::toNativeSeparators(QCoreApplication::applicationDirPath())
            .toStdWString().c_str());

    //通过QueryInterface获取IPersistFile接口,用于将快捷方式写入磁盘文件
    IPersistFile *ppf = nullptr;
    hr = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
    bool ok = false;
    if (SUCCEEDED(hr))
    {
        //保存到shell:startup目录 TRUE表示覆盖已有同名文件
        ok = SUCCEEDED(ppf->Save(get_auto_start_short_cut_path().toStdWString().c_str(), TRUE));
        ppf->Release();
    }
    psl->Release();
    return ok;
}

/**
  * @brief 删除开机自启动
  * @param 无
  * @retval 无
  * 	@arg
 */
bool MainWidget::delete_auto_start()
{
    return QFile::remove(get_auto_start_short_cut_path());
}

/**
  * @brief 喝水进度条显示数据
  * @param value 喝水变化值
  * @retval 无
  * 	@arg
 */
void MainWidget::bar_drink_show(int value)
{
    //记录后到达目标则进行提示
    if(m_settings->drink.record < m_settings->drink.goal
       && (m_settings->drink.record + value) >= m_settings->drink.goal)
    {
        push_message(NotifyType::GetDrinkGoal);
    }

    //喝水量加上本次的值,不允许减到负值
    m_settings->drink.record += value;
    if(m_settings->drink.record < 0)
    {
        m_settings->drink.record = 0;
    }

    //进度条最多到100
    if(m_settings->drink.record < m_settings->drink.goal)
    {
        ui->B_drink->setValue(m_settings->drink.record);
    }
    else
    {
        ui->B_drink->setValue(m_settings->drink.goal);
    }
    //主界面显示具体喝水值
    ui->B_drink->setFormat(QString("%1 / %2 ml").arg(m_settings->drink.record).arg(m_settings->drink.goal));

    //悬浮窗显示百分比
    m_floating_w.set_drink_value(m_settings->drink.record);
    //保存到文件
    m_settings_w.save_settings_to_file();
}

/**
  * @brief 推送消息
  * @param type 消息类型
  * @retval 无
  * 	@arg
 */
void MainWidget::push_message(NotifyType type)
{
    //推送内容填充
    QString title,msg;
    switch (type)
    {
        case NotifyType::WorkOver:
            title = "提示";
            if(m_settings->notice.clock_notice_enable)
            {
                msg = m_settings->notice.clock_work_over_tips;
            }
        break;

        case NotifyType::RestOver:
            title = "提示";
            if(m_settings->notice.clock_notice_enable)
            {
                msg = m_settings->notice.clock_rest_over_tips;
            }
        break;

        case NotifyType::GetDrinkGoal:
            title = "恭喜";
            if(m_settings->notice.drink_notice_enable)
            {
                msg = m_settings->notice.get_drink_goal_tips;
            }
        break;

        default:
        break;
    }

    //推送内容不为空时进行指定方式的推送
    if(!msg.isEmpty())
    {
        if(m_settings->notice.sys_notice_enable)
        {
            m_tray_icon->showMessage(title,msg);
        }
        if(m_settings->notice.messge_box_enable)
        {
            //非阻塞,不卡事件循环
            QMessageBox *box = new QMessageBox(this);
            box->setWindowTitle(title);
            box->setText(msg);
            box->setIcon(QMessageBox::Information);
            box->setStandardButtons(QMessageBox::Ok);
            //关闭时自动释放内存
            box->setAttribute(Qt::WA_DeleteOnClose);
            box->setWindowFlags(box->windowFlags() | Qt::WindowStaysOnTopHint);
            box->show();

            //定位到鼠标当前所在的屏幕进行弹窗
            QScreen *screen = QGuiApplication::screenAt(QCursor::pos());
            if (screen)
            {
                QRect screenRect = screen->availableGeometry();
                box->move(screenRect.center() - box->rect().center());
            }
        }
    }
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

