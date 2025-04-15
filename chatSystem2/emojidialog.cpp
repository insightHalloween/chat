#include "emojidialog.h"
#include <QHBoxLayout>
#include <QDebug>

// 常用表情列表
const QStringList emojis = {
    "😀", "😃", "😄", "😁", "😆", "😅", "😂", "🤣", "😊", "😇",
    "🙂", "🙃", "😉", "😌", "😍", "🥰", "😘", "😗", "😙", "😚",
    "😋", "😛", "😝", "😜", "🤪", "🤨", "🧐", "🤓", "😎", "🤩",
    "🥳", "😏", "😒", "😞", "😔", "😟", "😕", "🙁", "☹️", "😣",
    "😖", "😫", "😩", "🥺", "😢", "😭", "😤", "😠", "😡", "🤬",
    "🤯", "😳", "🥵", "🥶", "😱", "😨", "😰", "😥", "😓", "🤗",
    "🤔", "🤭", "🤫", "🤥", "😶", "😐", "😑", "😬", "🙄", "😯",
    "😦", "😧", "😮", "😲", "🥱", "😴", "🤤", "😪", "😵", "🤐",
    "🥴", "🤢", "🤮", "🤧", "😷", "🤒", "🤕", "🤑", "🤠", "😈",
    "👿", "👹", "👺", "🤡", "💩", "👻", "💀", "☠️", "👽", "👾",
    "🤖", "🎃", "😺", "😸", "😹", "😻", "😼", "😽", "🙀", "😿",
    "😾", "👋", "🤚", "🖐️", "✋", "🖖", "👌", "🤏", "✌️", "🤞",
    "🤟", "🤘", "👈", "👉", "👆", "👇", "👍", "👎", "✊", "👊",
    "🤛", "🤜", "👏", "🙌", "👐", "🤲", "🤝", "🙏", "✍️", "💅",
    "🤳", "💪", "🦾", "🦵", "🦿", "🦶", "👂", "🦻", "👃", "🧠",
    "🦷", "🦴", "👀", "👁️", "👅", "👄", "💋", "🩸", "💧", "💦"
};

EmojiDialog::EmojiDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("选择表情");
    setFixedSize(500, 400);

    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(5);
    // gridLayout->setContentsMargins(10, 10, 10, 10);

    initEmojis();
}

EmojiDialog::~EmojiDialog()
{
    qDeleteAll(emojiButtons);
}

void EmojiDialog::initEmojis()
{
    int row = 0;
    int col = 0;
    const int maxCols = 10;

    for (const QString &emoji : emojis) {
        QPushButton *button = new QPushButton(emoji, this);
        // button->setFixedSize(35, 35);
        button->setFont(QFont("Segoe UI Emoji", 14));
        button->setStyleSheet(R"(
            QPushButton {
                background-color: #f0f0f0;
                border: 2px solid #e0e0e0;
                border-radius: 8px;
                padding: 0px;
            }
            QPushButton:hover {
                background-color: #e0e0e0;
            }
            QPushButton:pressed {
                background-color: #d0d0d0;
            }
        )");
        connect(button, &QPushButton::clicked, this, &EmojiDialog::onEmojiClicked);

        gridLayout->addWidget(button, row, col);
        emojiButtons.append(button);

        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

void EmojiDialog::onEmojiClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        emit emojiSelected(button->text());
        accept();
    }
}
