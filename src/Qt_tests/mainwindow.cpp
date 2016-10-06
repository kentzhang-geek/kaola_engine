#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_login.h"
#include "src/utils/qui/myhelper.h"
#include "resource_and_network/network_tool.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) {
    // for test only
//    this->on_testtiaozhuan_clicked();

    auto test = gl3d::gl3d_global_param::shared_instance();
    Ui::Login * lgui = new Ui::Login;
    lgui->setupUi(this);
    lgui->login_password->setEchoMode(QLineEdit::Password);
    delete lgui;
    myHelper::FormInCenter(this);
    setWindowState(Qt::WindowMaximized);
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_testtiaozhuan_clicked()
{
    dhw = new drawhomewin(this->parentWidget());
    dhw->show();
    this->releaseMouse();
    this->releaseKeyboard();
    this->hide();
}

void MainWindow::on_login_signin_button_clicked() {
    QLineEdit * uname = this->findChild<QLineEdit *>("login_username");
    QLineEdit * pword = this->findChild<QLineEdit *>("login_password");
    QPushButton * btn = this->findChild<QPushButton *>("login_signin_button");

    if (uname->text().isEmpty() || pword->text().isEmpty()) {
        QMessageBox mbox;
        mbox.setText(tr("Please insert your password and username"));
        mbox.setWindowTitle(tr("Warning"));
        mbox.exec();
        return;
    }

    // set invisible
    uname->setReadOnly(true);
    pword->setReadOnly(true);
    btn->setEnabled(false);

    // TODO : login
    this->lg = new klm::network::login_tool;
    connect(this->lg, SIGNAL(login_success(bool)), this, SLOT(process_login(bool)));
//    this->lg->login_with("627420278@qq.com", "123456789");
    this->lg->login_with(uname->text(), pword->text());

    return;
}

void MainWindow::process_login(bool isok) {
    delete this->lg;
    this->lg = NULL;

    QLineEdit * uname = this->findChild<QLineEdit *>("login_username");
    QLineEdit * pword = this->findChild<QLineEdit *>("login_password");
    QPushButton * btn = this->findChild<QPushButton *>("login_signin_button");

    // set invisible
    uname->setReadOnly(false);
    pword->setReadOnly(false);
    btn->setEnabled(true);

    if (isok) {
        // hide all children
        QObjectList olist = this->children();
        for (auto oit = olist.begin();
                oit != olist.end();
                oit++) {
            if (oit.i->t()->isWidgetType()) {
                ((QWidget *)oit.i->t())->hide();
            }
        }
        // new ui
        Ui::MainWindow * ui = new Ui::MainWindow;
        ui->setupUi(this);
        delete ui;
    }
    else {
        QMessageBox box;
        box.setText(tr("Signin Failed, Please confirm your infomation and retry"));
        box.setWindowTitle(tr("Signin Failed"));
        box.exec();
        return;
    }
}

