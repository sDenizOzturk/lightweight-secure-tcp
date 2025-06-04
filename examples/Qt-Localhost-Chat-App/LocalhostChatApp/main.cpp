#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile qssFile(QStringLiteral("://style.qss"));
    if (qssFile.open(QFile::ReadOnly)) {
        a.setStyleSheet(QString::fromUtf8(qssFile.readAll()));
        qssFile.close();
    }

    MainWindow w;
    w.show();
    return a.exec();
}
