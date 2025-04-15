#ifndef EMOJIDIALOG_H
#define EMOJIDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>

class EmojiDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmojiDialog(QWidget *parent = nullptr);
    ~EmojiDialog();

signals:
    void emojiSelected(const QString &emoji);

private slots:
    void onEmojiClicked();

private:
    QGridLayout *gridLayout;
    QList<QPushButton*> emojiButtons;
    void initEmojis();
};

#endif // EMOJIDIALOG_H
