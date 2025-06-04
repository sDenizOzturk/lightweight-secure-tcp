#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <lightweightsecuretcp.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    LightweightSecureServer *m_server;
    LightweightSecureClient *m_client;

    const int PORT = 5555;

    void setupServer();
    void setupClient();

    void appendMessage(QListWidget* messageList, const QString& messageText, bool isSentBySelf);
};
#endif // MAINWINDOW_H
