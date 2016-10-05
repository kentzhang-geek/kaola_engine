#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_login.h"
#include "src/utils/qui/myhelper.h"
#include "resource_and_network/network_tool.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) {
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

    // set invisible
//    uname->setReadOnly(true);
//    pword->setReadOnly(true);

    // TODO : login
    klm::network::login_tool * lg = new klm::network::login_tool;
    lg->login_with(uname->text(), pword->text());
}
