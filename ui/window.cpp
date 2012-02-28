#include "window.h"
#include <QListView>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QStackedLayout>
#include <QTabWidget>
#include "receiver/receiver.h"

Window::Window(Receiver* r, QWidget* p) : QMainWindow(p), receiver_(r)
{
    QListView *view = new QListView(this);
    QList<QStandardItem*> items; //список элементов в строке матрицы
    QStandardItemModel *model = new QStandardItemModel();
    messageModel_ = model;
    meslist_ = receiver_->messages();
    foreach (QVariant mes, meslist_)
        items << new QStandardItem(mes.toMap()["header"].toString());
    model->appendColumn(items);
    view->setModel(model);

    connect(view,SIGNAL(clicked(QModelIndex)),
            this,SLOT(showMessage(QModelIndex)));
    connect(view,SIGNAL(activated(QModelIndex)),
            this,SLOT(showMessage(QModelIndex)));

    tabWidget_ = new QTabWidget();
    tabWidget_->addTab(view,tr("Message List"));

    view = new QListView(this);
    model = new QStandardItemModel();
    items.clear();
    for (int i=0; i<(int)receiver_->boxesList_.size(); i++)
        items << new QStandardItem(QString::
            fromStdString(receiver_->boxesList_[i]));
    model->appendColumn(items);
    view->setModel(model);
    connect(view,SIGNAL(clicked(QModelIndex)),
            this,SLOT(changeBox(QModelIndex)));
    connect(view,SIGNAL(activated(QModelIndex)),
            this,SLOT(changeBox(QModelIndex)));
    tabWidget_->addTab(view,tr("Mail Boxes"));
    //showList();
    setCentralWidget(tabWidget_);
    QWidget *settings = new QWidget(this);
    QLayout *box = new QVBoxLayout();
    QPushButton *updateButton = new QPushButton(tr("Receive new messages"),
                                                    this);
    connect(updateButton,SIGNAL(clicked()),
            this,SLOT(receive()));
    box->addWidget(updateButton);
    settings->setLayout(box);
    tabWidget_->addTab(settings,tr("Settings"));
}

void Window::showList()
{
    tabWidget_->setCurrentIndex(0);
}

void Window::showMessage(QModelIndex ind)
{
    QVBoxLayout *box = new QVBoxLayout();
    QTextEdit *edit = new QTextEdit(meslist_.at(ind.row())
                          .toMap()["body"].toString(),this);
    box->addWidget(edit);
    tabWidget_->addTab(edit,"message");
    tabWidget_->setCurrentIndex(tabWidget_->count()-1);
    //backButton_->setVisible(1);

    //layout()->removeItem(listLayout_);
    //mainLayout_->update();
    //setLayout(mainLayout_);
}

void Window::changeBox(QModelIndex ind)
{
    receiver_->currentBox_ = ind.row();
    receive();
}

void Window::receive()
{
    QList<QStandardItem*> items; //список элементов в строке матрицы
    messageModel_->clear();
    meslist_ = receiver_->messages();
    foreach (QVariant mes, meslist_)
        items << new QStandardItem(mes.toMap()["header"].toString());
    messageModel_->appendColumn(items);
}
