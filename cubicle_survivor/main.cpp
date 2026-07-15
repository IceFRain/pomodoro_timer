#include "main_widget.h"
#include <QApplication>

#include <QFile>
#include <QMutex>
#include <QDateTime>
#include <QDir>

static QMutex log_mutex;

//日志轮转配置
static const QString LOG_DIR    = "logs";
static const QString LOG_FILE   = LOG_DIR + "/log.txt";
static const qint64  MAX_SIZE   = 128 * 1024;       //单个文件最大128K
static const int     MAX_BACKUPS = 2;               //最多保留2个备份

/**
  * @brief 日志轮转
  * @param 无
  * @retval 无
  * 	@arg
 */
static void rotateLogs()
{
    //检查当前日志大小
    QFile current(LOG_FILE);
    if (!current.exists() || current.size() < MAX_SIZE)
    {
        return;
    }

    //删除最老的备份
    QFile::remove(QString("%1/log.%2.txt").arg(LOG_DIR).arg(MAX_BACKUPS));

    //依次重命名：log.4.txt ← log.3.txt ← ... ← log.1.txt ← log.txt
    for (int i = MAX_BACKUPS - 1; i >= 1; --i)
    {
        QString from = QString("%1/log.%2.txt").arg(LOG_DIR).arg(i);
        QString to   = QString("%1/log.%2.txt").arg(LOG_DIR).arg(i + 1);
        QFile::rename(from, to);
    }

    //当前文件变成 log.1.txt
    QFile::rename(LOG_FILE, QString("%1/log.1.txt").arg(LOG_DIR));
}

/**
  * @brief qDebug记录到文件接口函数
  * @param type
  * @param context
  * @param msg
  * @retval 无
  * 	@arg
 */
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QDateTime _datetime = QDateTime::currentDateTime();
    QString szDate = _datetime.toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString txt(szDate);

    switch (type) {
        case QtDebugMsg:
            txt += QString(" [Debug] ");
            break;
        case QtInfoMsg:
            txt += QString(" [Info] ");
            break;
        case QtWarningMsg:
            txt += QString(" [Warning] ");
            break;
        case QtCriticalMsg:
            txt += QString(" [Critical] ");
            break;
        case QtFatalMsg:
            txt += QString(" [Fatal] ");
            break;
        default:
            txt += QString(" [Trace] ");
            break;
    }

    txt.append(QString(" %1").arg(context.file));
    txt.append(QString("<%1>: ").arg(context.line));
    txt.append(msg);

    log_mutex.lock();
    //先检测轮转
    rotateLogs();
    QFile file(LOG_FILE);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << txt << "\r\n";
    file.close();
    log_mutex.unlock();
}

/**
  * @brief 应用初始化配置
  * @param 无
  * @retval 无
  * 	@arg
 */
void app_config(void)
{
    //开启高分屏适配,强制QT渲染
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
}

/**
  * @brief 主函数入口
  * @param argc 参数个数
  * @param argv[] 参数列表
  * @retval 运行结果
  * 	@arg 0 正常退出
 */
int main(int argc, char *argv[])
{
    // 确保日志目录存在
    QDir().mkpath(LOG_DIR);
    //注册文件日志接口
    qInstallMessageHandler(customMessageHandler);
    //初始化配置
    app_config();

    QApplication a(argc, argv);
    //不要在最后一个窗口关闭时关闭程序
    a.setQuitOnLastWindowClosed(false);
    MainWidget w;
    //启动不显示主界面
//    w.show();

    return a.exec();
}

