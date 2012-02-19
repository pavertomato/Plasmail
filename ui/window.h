#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QVariantList>
#include <QModelIndex>
#include <QVBoxLayout>
class Receiver;
class QStackedLayout;
class QPushButton;
class Window : public QWidget
{
    Q_OBJECT

    Receiver *receiver_;
    QVariantList meslist_;
    QVBoxLayout *mainLayout_;
    QVBoxLayout *messageLayout_;
    QStackedLayout *listLayout_;
    QPushButton *backButton_;
public:
    Window(Receiver*,QWidget* p =NULL);
public slots:
    void showList();
    void showMessage(QModelIndex);
};

#endif // WINDOW_H
