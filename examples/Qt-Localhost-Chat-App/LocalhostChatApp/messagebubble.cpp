#include "messagebubble.h"
#include <QTextEdit>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QTextOption>

MessageBubble::MessageBubble(const QString& text, bool alignRight, QWidget* parent)
    : QWidget(parent)
{
    auto* textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setFrameShape(QFrame::NoFrame);
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Enable character wrap instead of word-only wrap
    QTextOption textOption = textEdit->document()->defaultTextOption();
    textOption.setWrapMode(QTextOption::WordWrap);
    textEdit->document()->setDefaultTextOption(textOption);

    // Set CSS class for styling
    textEdit->setProperty("class", alignRight ? "message-right" : "message-left");

    // Set message content and layout constraint
    constexpr int MaxWidth = 150;
    textEdit->setPlainText(text);
    textEdit->setMaximumWidth(MaxWidth );

    textEdit->document()->adjustSize();
    qreal documentHeight = textEdit->document()->size().height();

    const int cssVerticalPadding = 6 + 6;
    const int layoutMargins = 4 + 4;
    textEdit->setFixedHeight(int(documentHeight) + cssVerticalPadding + layoutMargins);

    // Layout and alignment
    auto* horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setContentsMargins(10, 10, 10, 10);
    horizontalLayout->setSpacing(0);


    if (alignRight) {
        horizontalLayout->addStretch();
        horizontalLayout->addWidget(textEdit);
    } else {
        horizontalLayout->addWidget(textEdit);
        horizontalLayout->addStretch();
    }
    textEdit->setAlignment(Qt::AlignCenter);

    setLayout(horizontalLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);


}
