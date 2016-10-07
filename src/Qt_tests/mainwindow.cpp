#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_login.h"
#include "src/utils/qui/myhelper.h"
#include "resource_and_network/network_tool.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) {
    // for test
//    Ui::MainWindow * ui = new Ui::MainWindow;
//    ui->setupUi(this);
//    delete ui;
//    return;

    this->resize(640, 640);
    web = new QWebEngineView(this);
    web->resize(this->width(), this->height());
    web->load(QUrl("http://192.168.0.100:8080/index.html#/app/login"));
    web->show();

//    QString code = QString::fromLocal8Bit("alert(\'wwww aaaa\')");
//    web->page()->runJavaScript(code);
    this->channel = new QWebChannel(this);
    web->page()->setWebChannel(this->channel);
    this->channel->registerObject(QStringLiteral("twin"), this);

    // for test only
//    this->on_testtiaozhuan_clicked();

//    auto test = gl3d::gl3d_global_param::shared_instance();
//    Ui::Login * lgui = new Ui::Login;
//    lgui->setupUi(this);
//    lgui->login_password->setEchoMode(QLineEdit::Password);
//    delete lgui;
//    myHelper::FormInCenter(this);
//    setWindowState(Qt::WindowMaximized);
}

void MainWindow::test_text(const QString &text) {
    QMessageBox * box = new QMessageBox();
    box->setWindowTitle("now test hitted");
    box->setText(text);
//    box->show();
    box->exec();

    if (text == "login") {
        // hide all children
        QObjectList olist = this->children();
        for (auto oit = olist.begin();
             oit != olist.end();
             oit++) {
            if (oit.i->t()->isWidgetType()) {
                ((QWidget *)oit.i->t())->hide();
            }
        }
        Ui::MainWindow * ui = new Ui::MainWindow;
        ui->setupUi(this);
        delete ui;
    }
    return;
}

void MainWindow::resizeEvent(QResizeEvent *ev) {
//    web->resize(this->width(), this->height()); // TODO : resize web view
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

