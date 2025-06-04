#ifndef MESSAGEBUBBLE_H
#define MESSAGEBUBBLE_H

#include <QWidget>

class MessageBubble : public QWidget
{
    Q_OBJECT

public:
    explicit MessageBubble(const QString& text, bool alignRight, QWidget *parent = nullptr);
};

#endif // MESSAGEBUBBLE_H
