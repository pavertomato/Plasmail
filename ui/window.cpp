#include "window.h"
#include <QListView>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QStackedLayout>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
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
    //настройки
    QWidget *settings = new QWidget(this);
    QLayout *box = new QVBoxLayout();
    //ввод адреса почты
    QLayout *enterBox = new QVBoxLayout();
    enterBox->addWidget(new QLabel(tr("E-mail"),this));
    QLineEdit *edit = new QLineEdit(
            QString::fromStdString(receiver_->mail_),this);
    enterBox->addWidget(edit);
    connect(edit,SIGNAL(textEdited(QString)),
            this,SLOT(mailEdited(QString)));
    box->addItem(enterBox);
    //ввод сервера
    enterBox = new QVBoxLayout();
    enterBox->addWidget(new QLabel(tr("Server"),this));
    edit = new QLineEdit(
            QString::fromStdString(receiver_->server_),this);
    enterBox->addWidget(edit);
    connect(edit,SIGNAL(textEdited(QString)),
            this,SLOT(servEdited(QString)));
    box->addItem(enterBox);
    //ввод имени
    enterBox = new QVBoxLayout();
    enterBox->addWidget(new QLabel(tr("Name"),this));
    edit = new QLineEdit(
            QString::fromStdString(receiver_->name_),this);
    enterBox->addWidget(edit);
    connect(edit,SIGNAL(textEdited(QString)),
            this,SLOT(nameEdited(QString)));
    box->addItem(enterBox);
    //ввод логина
    enterBox = new QVBoxLayout();
    enterBox->addWidget(new QLabel(tr("Username"),this));
    edit = new QLineEdit(
            QString::fromStdString(receiver_->username_),this);
    enterBox->addWidget(edit);
    connect(edit,SIGNAL(textEdited(QString)),
            this,SLOT(usernameEdited(QString)));
    box->addItem(enterBox);
    //ввод имени
    enterBox = new QVBoxLayout();
    enterBox->addWidget(new QLabel(tr("Password"),this));
    edit = new QLineEdit(
            QString::fromStdString(receiver_->password_),this);
    edit->setEchoMode(QLineEdit::Password);
    enterBox->addWidget(edit);
    connect(edit,SIGNAL(textEdited(QString)),
            this,SLOT(passwordEdited(QString)));
    box->addItem(enterBox);
    //кнопка обновления
    QPushButton *updateButton = new QPushButton(tr("Receive new messages"),
                                                    this);
    connect(updateButton,SIGNAL(clicked()),
            this,SLOT(receive()));
    box->addWidget(updateButton);
    settings->setLayout(box);

    tabWidget_->addTab(settings,tr("Settings"));
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

void Window::mailEdited(QString s)
{
    receiver_->mail_ = s.toStdString();
}

void Window::servEdited(QString s)
{
    receiver_->server_ = s.toStdString();
}

void Window::nameEdited(QString s)
{
    receiver_->name_ = s.toStdString();
}

void Window::usernameEdited(QString s)
{
    receiver_->username_ = s.toStdString();
}

void Window::passwordEdited(QString s)
{
    receiver_->password_ = s.toStdString();
}
