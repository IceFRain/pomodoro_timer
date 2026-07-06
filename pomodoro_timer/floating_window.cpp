#include "floating_window.h"
#include "ui_floating_window.h"
#include <QMenu>

#ifdef Q_OS_WIN
#include <windows.h>
HWND f_hwnd;
#endif

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

    //设置窗口大小
    setMinimumSize(1,1);
    resize(80,42);

#ifdef Q_OS_WIN
    f_hwnd = (HWND)this->winId();
    SetWindowPos(f_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#endif

    //进度条样式设置
    ui->progressBar->setStyleSheet(
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
    ui->progressBar->setTextVisible(true);
    this->set_range(0,1500);
    this->set_value(0);

    //进度条右键点击菜单设置
    ui->progressBar->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->progressBar, &QWidget::customContextMenuRequested, [&](const QPoint &pos){
        QMenu menu;
        QAction *satrt_action = menu.addAction("开始");
        QAction *pause_action = menu.addAction("暂停");
        QAction *continue_action = menu.addAction("继续");
        QAction *quit_action = menu.addAction("退出");
        QAction *selected = menu.exec(ui->progressBar->mapToGlobal(pos));
        if (selected == quit_action)
        {
            qApp->quit();
        }
        else if (selected == satrt_action)
        {
            emit sig_start_loop();
        }
        else if (selected == pause_action)
        {
            emit sig_pause_loop();
        }
        else if (selected == continue_action)
        {
            emit sig_continue_loop();
        }});
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
  * @brief 设置秒数值
  * @param value 当前剩余秒数
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_value(int value)
{
    ui->progressBar->setValue(value);
    int minutes = value / 60;
    int seconds = value % 60;
    ui->progressBar->setFormat(QString("%1:%2") .arg(minutes, 2, 10, QChar('0')) .arg(seconds, 2, 10, QChar('0')));

#ifdef Q_OS_WIN
    if(!this->isHidden())
    {
        QRect rect = this->geometry();   // 或者 frameGeometry()
        int x = rect.x();
        int y = rect.y();
        int w = rect.width();
        int h = rect.height();

        SetWindowPos(f_hwnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
    }
#endif
}

/**
  * @brief 设置秒范围
  * @param min 通常为0
  * @param max 进度条最大时的秒值
  * @retval 无
  * 	@arg
 */
void FloatingWindow::set_range(int min, int max)
{
    ui->progressBar->setRange(min,max);
}

/**
  * @brief 鼠标点击事件
  * @param event 事件
  * @retval 无
  * 	@arg
 */
void FloatingWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
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
    if (event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - dragPosition);
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
    if (event->button() == Qt::LeftButton)
    {
        emit sig_double_clicked();  // 发射你定义的双击信号
        event->accept();
    }
}
