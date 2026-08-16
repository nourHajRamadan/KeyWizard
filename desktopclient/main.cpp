#include "mainwindow.h"

#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include <QTextStream>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap(":/all/kwdc.png");
    QSplashScreen splash(pixmap);
    splash.show();

    QThread::sleep(3);

    a.processEvents();
    QFontDatabase::addApplicationFont(":/all/Pixellari.ttf");

    QFile styleFile(":/all/style.qss");
    if(styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream styleStream(&styleFile);
        a.setStyleSheet(styleStream.readAll());
        styleFile.close();
    }

    a.setWindowIcon(QIcon(":/all/icon.png"));

    MainWindow w;
    w.show();
    splash.finish(&w);
    w.refreshSerialPorts();


    return QCoreApplication::exec();
}
