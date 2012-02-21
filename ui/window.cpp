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
    meslist_ = receiver_->messages();
    foreach (QVariant mes, meslist_)
        items << new QStandardItem(mes.toMap()["header"].toString());
    model->appendColumn(items);
    view->setModel(model);
    connect(view,SIGNAL(activated(QModelIndex)),
            this,SLOT(showMessage(QModelIndex)));
    connect(view,SIGNAL(clicked(QModelIndex)),
            this,SLOT(showMessage(QModelIndex)));

    //backButton_ = new QPushButton(tr("&menu"),this);
    //connect(backButton_,SIGNAL(clicked()),this,SLOT(showList()));

    tabWidget_ = new QTabWidget();
    tabWidget_->addTab(view,tr("Message List"));
    //tabWidget_->addTab(backButton_,"sasdf");
    //showList();
    setCentralWidget(tabWidget_);
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
