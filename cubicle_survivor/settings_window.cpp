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

    //设置列表文字居中
    for (int i = 0; i < ui->LW_list->count(); i++)
    {
        ui->LW_list->item(i)->setTextAlignment(Qt::AlignCenter);
    }

    //设置列表选择和页面切换连接
    connect(ui->LW_list,&QListWidget::currentRowChanged,ui->SW_settings,&QStackedWidget::setCurrentIndex);

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
