#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QVariantList>
#include <QModelIndex>
#include <QVBoxLayout>
class Receiver;
class QStackedLayout;
class QPushButton;
class QTabWidget;
class Window : public QMainWindow
{
    Q_OBJECT

    Receiver *receiver_;
    QVariantList meslist_;
    QVBoxLayout *mainLayout_;
    QVBoxLayout *messageLayout_;
    QStackedLayout *listLayout_;
    QPushButton *backButton_;
    QTabWidget *tabWidget_;
public:
    Window(Receiver*,QWidget* p =NULL);
public slots:
    void showList();
    void showMessage(QModelIndex);
};

#endif // WINDOW_H
