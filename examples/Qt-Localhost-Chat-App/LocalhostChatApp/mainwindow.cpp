#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "messagebubble.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Key handshakeKey({
        0x12345678, 0x9ABCDEF0, 0x13572468, 0xACEBDF01,
        0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00
    });
    LightweightSecureTCP::setHandshakeKey(handshakeKey);

    setupServer();
    setupClient();
}

MainWindow::~MainWindow()
{
    m_client->disconnect();
    m_server->stop();
    delete m_server;
    delete m_client;
    delete ui;
}

void MainWindow::setupServer()
{
    m_server = new LightweightSecureServer(PORT);

    m_server->setOnHandshakeSuccess([this](const HandshakeResult& result, Session& session) {
        session.setOnMessageReceived([this](const std::string& message) {
            QMetaObject::invokeMethod(this, [this, message]() {
                appendMessage(ui->listWidget_server, QString::fromStdString(message), false);
            }, Qt::QueuedConnection);
        });

        connect(ui->pushButton_server, &QPushButton::clicked, this, [this, &session]() {
            const QString messageText = ui->lineEdit_server->text();

            if (!messageText.isEmpty()) {
                session.sendMessage(messageText.toStdString());

                QMetaObject::invokeMethod(this, [this, messageText]() {
                    appendMessage(ui->listWidget_server, messageText, true);
                }, Qt::QueuedConnection);
            }
            ui->lineEdit_server->clear();
        });
    });
    m_server->start();
}

    void MainWindow::setupClient()
    {
        m_client = new LightweightSecureClient("127.0.0.1", PORT);
        m_client->setOnHandshakeSuccess([this](const HandshakeResult& result) {
            m_client->setOnMessageReceived([this](const std::string& message) {
                QMetaObject::invokeMethod(this, [this, message]() {
                    appendMessage(ui->listWidget_client, QString::fromStdString(message), false);
                }, Qt::QueuedConnection);
            });

            connect(ui->pushButton_client, &QPushButton::clicked, this, [this]() {
                const QString messageText = ui->lineEdit_client->text();
                if (!messageText.isEmpty()) {
                    m_client->sendMessage(messageText.toStdString());

                    QMetaObject::invokeMethod(this, [this, messageText]() {
                        appendMessage(ui->listWidget_client, messageText, true);
                    }, Qt::QueuedConnection);
                }
                ui->lineEdit_client->clear();
            });
        });
        m_client->connectToServer();
    }

void MainWindow::appendMessage(QListWidget* messageList, const QString& messageText, bool isSentBySelf)
{
    auto* messageWidget = new MessageBubble(messageText, isSentBySelf);

    int listWidth = messageList->viewport()->width();
    int widgetHeight = messageWidget->sizeHint().height();

    auto* listItem = new QListWidgetItem(messageList);
    listItem->setSizeHint(QSize(listWidth, widgetHeight));

    messageList->addItem(listItem);
    messageList->setItemWidget(listItem, messageWidget);
    messageList->scrollToBottom();
}
