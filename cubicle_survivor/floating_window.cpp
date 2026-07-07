#include "floating_window.h"
#include "ui_floating_window.h"
#include <QMenu>

#ifdef Q_OS_WIN
#include <windows.h>
HWND f_hwnd;
#endif

#define PROGRESS_BAR_WIDTH          52
#define PROGRESS_BAR_HEIGHT         18
#define WINDOW_WIDTH                (PROGRESS_BAR_WIDTH*m_bar_show_count + m_bar_show_count-1)
#define WINDOW_HEIGHT               PROGRESS_BAR_HEIGHT

/**
  * @brief 构造函数
  * @param parent:父窗口
  * @retval 无
  * 	@arg
 */
FloatingWindow::FloatingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FloatingWindow)
{
    ui->setupUi(this);

    //设置窗口标志 工具窗口 无标题栏 顶层显示
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //背景透明
    this->setAttribute(Qt::WA_TranslucentBackground);

    //默认不隐藏时显示的进度条个数为2
    m_bar_show_count = 2;
    //设置窗口大小
    setMinimumSize(1,1);
    resize(WINDOW_WIDTH,WINDOW_HEIGHT);

//windows平台设置窗口属性
#ifdef Q_OS_WIN
    //获取窗口句柄
    f_hwnd = (HWND)this->winId();
    //置顶显示,不聚焦
    SetWindowPos(f_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#endif

    //进度条样式设置
    ui->B_time->setStyleSheet(
                "QProgressBar { "
                " border: 1px solid grey; "
                " text-align: center; "
                " font: 12pt 'MiSans'; "
                "} "
                "QProgressBar::chunk { "
                " background-color: mediumaquamarine; "
                " width: 1px; "
                "}"
            );
    ui->B_time->setTextVisible(true);
    this->set_clock_range(0,1500);
    this->set_clock_value(0);

    ui->B_drink->setStyleSheet(
                "QProgressBar { "
                " border: 1px solid grey; "
                " text-align: center; "
                " font: 12pt 'MiSans'; "
                "} "
                "QProgressBar::chunk { "
                " background-color: lightskyblue; "
                " width: 1px; "
                "}"
            );
    ui->B_drink->setTextVisible(true);
    this->set_drink_range(0,1800);
    this->set_drink_value(0);

    //进度条右键点击槽连接
    ui->B_time->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->B_time, &QWidget::customContextMenuRequested, this,&FloatingWindow::slot_bar_clock_right_clicked);
    //进度条右键点击槽连接
    ui->B_drink->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->B_drink, &QWidget::customContextMenuRequested, this,&FloatingWindow::slot_bar_drink_right_clicked);
}

/**
  * @brief 析构函数
  * @param 无
  * @retval 无
  * 	@arg
 */
FloatingWindow::~FloatingWindow()
{
    delete ui;
}

/**
  * @brief 设置时钟秒数值
  * @param value 当前剩余秒数
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_clock_value(int value)
{
    //设置进度
    ui->B_time->setValue(value);
    //设置文字显示
    int minutes = value / 60;
    int seconds = value % 60;
    ui->B_time->setFormat(QString("%1:%2") .arg(minutes, 2, 10, QChar('0')) .arg(seconds, 2, 10, QChar('0')));
    //刷新窗口
    refresh_windows();
}

/**
  * @brief 设置时钟秒范围
  * @param min 通常为0
  * @param max 进度条最大时的秒值
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_clock_range(int min, int max)
{
    ui->B_time->setRange(min,max);
}

/**
  * @brief 时钟进度条显示状态切换
  * @param status 是否显示
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_bar_clock_show(bool status)
{
    //如果隐藏状态变化,对应当前显示进度条数量也变化
    if(true==status)
    {
        if(ui->B_time->isHidden())
        {
            m_bar_show_count++;
        }
    }
    else
    {
        if(!ui->B_time->isHidden())
        {
            m_bar_show_count--;
        }
    }
    //设置隐藏状态后刷新窗口
    ui->B_time->setHidden(!status);
    refresh_windows();
}

/**
  * @brief 设置饮水量
  * @param value 当前已饮水量
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_drink_value(int value)
{
    if(value < ui->B_drink->maximum())
    {
        ui->B_drink->setValue(value);
    }
    else
    {
        ui->B_drink->setValue(ui->B_drink->maximum());
    }
    int percentage = value * 100 / ui->B_drink->maximum();
    ui->B_drink->setFormat(QString("%1%").arg(percentage));
}

/**
  * @brief 设置饮水进度条范围
  * @param min 最小值
  * @param max 最大值
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_drink_range(int min, int max)
{
    ui->B_drink->setRange(min,max);
}

/**
  * @brief 饮水量进度条显示状态切换
  * @param status 是否显示
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_bar_drink_show(bool status)
{
    //如果隐藏状态变化,对应当前显示进度条数量也变化
    if(true==status)
    {
        if(ui->B_drink->isHidden())
        {
            m_bar_show_count++;
        }
    }
    else
    {
        if(!ui->B_drink->isHidden())
        {
            m_bar_show_count--;
        }
    }
    ui->B_drink->setHidden(!status);
    refresh_windows();
}

/**
  * @brief 时钟进度条右键点击槽
  * @param pos
  * @retval 无
  * 	@arg
 */
void FloatingWindow::slot_bar_clock_right_clicked(const QPoint &pos)
{
    QMenu menu;
    QAction *satrt_action = menu.addAction("开始");
    QAction *pause_action = menu.addAction("暂停");
    QAction *continue_action = menu.addAction("继续");
    QAction *quit_action = menu.addAction("退出");
    QAction *selected = menu.exec(ui->B_time->mapToGlobal(pos));
    if (selected == quit_action)
    {
        qApp->quit();
    }
    else if (selected == satrt_action)
    {
        emit sig_clock_start_loop();
    }
    else if (selected == pause_action)
    {
        emit sig_clock_pause_loop();
    }
    else if (selected == continue_action)
    {
        emit sig_clock_continue_loop();
    }
}

/**
  * @brief 饮水量进度条右键点击槽
  * @param pos
  * @retval 无
  * 	@arg
 */
void FloatingWindow::slot_bar_drink_right_clicked(const QPoint &pos)
{
    QMenu menu;
    QAction *record_cup = menu.addAction("记一杯");
    QAction *quit_action = menu.addAction("退出");
    QAction *selected = menu.exec(ui->B_drink->mapToGlobal(pos));
    if (selected == quit_action)
    {
        qApp->quit();
    }
    else if (selected == record_cup)
    {
        emit sig_drink_record_cup();
    }
}

/**
  * @brief 鼠标点击事件
  * @param event 事件
  * @retval 无
  * 	@arg
 */
void FloatingWindow::mousePressEvent(QMouseEvent *event)
{
    //记录鼠标点击位置
    if (event->button() == Qt::LeftButton)
    {
        m_drag_position = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

/**
  * @brief 鼠标移动事件
  * @param event 事件
  * @retval 无
  * 	@arg
 */
void FloatingWindow::mouseMoveEvent(QMouseEvent *event)
{
    //鼠标点击后移动,窗口对应移动
    if (event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - m_drag_position);
        event->accept();
    }
}

/**
  * @brief 鼠标双击事件
  * @param event 事件
  * @retval 无
  *     @arg
 */
void FloatingWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    //鼠标双击发射信号
    if (event->button() == Qt::LeftButton)
    {
        emit sig_double_clicked();
        event->accept();
    }
}

/**
  * @brief 刷新窗口显示
  * @param 无
  * @retval 无
  *     @arg
 */
void FloatingWindow::refresh_windows()
{
    //如果没有进度条需要显示,直接隐藏悬浮窗
    if(m_bar_show_count==0)
    {
        this->hide();
        return;
    }

    this->show();
//windows平台刷新窗口显示,强制刷新到最前
#ifdef Q_OS_WIN
    if(!this->isHidden())
    {
        resize(WINDOW_WIDTH,WINDOW_HEIGHT);
        QRect rect = this->geometry();
        int x = rect.x();
        int y = rect.y();
        int w = rect.width();
        int h = rect.height();
        SetWindowPos(f_hwnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW| SWP_NOACTIVATE);
    }
#endif
}
