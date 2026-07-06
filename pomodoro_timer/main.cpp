#include "main_widget.h"

#include <QApplication>

void core_config(void)
{
    //开启高分屏适配,强制QT渲染
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWidget w;
//    w.show();
    return a.exec();
}

