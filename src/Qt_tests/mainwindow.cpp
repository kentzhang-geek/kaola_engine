#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_login.h"
#include "src/utils/qui/myhelper.h"
#include "resource_and_network/network_tool.h"

#include "resource_and_network/pack_tool.h"
#include "resource_and_network/global_info.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) {
    // for test
//    klm::pack_tool * pack = new klm::pack_tool;
//    QVector<QString> tfs;
//    klm::pack_tool::init_with_password("123");
//    tfs.append("test.html");
//    tfs.append("*.xml");
//    tfs.append("debug/*.obj");
//    pack->unpack("t.7z", "tout");
//    pack->pack(tfs, "t.7z");
//    klm::network::call_web_download(
//            "file/2c4469d2-d073-422a-b841-1676f20a31e7/RUU/534a235d-0a75-4bc1-b3d2-6a1fe90c32d0/test.cfn", "tmp/s.7z");
//    klm::pack_tool pkt;
//    pkt.unpack("tmp/s.7z", "tmp");

//    this->lg = new klm::network::login_tool;
//    this->lg->login_with("934635461@qq.com", "hl320619");
//    Ui::MainWindow * ui = new Ui::MainWindow;
//    ui->setupUi(this);
//    delete ui;
//
//    return;

    auto ssize = QApplication::desktop()->availableGeometry();
    this->setGeometry(0, 0, ssize.width(), ssize.height());
    this->setWindowState(Qt::WindowMaximized);
    web = new QWebEngineView(this);
    web->resize(this->width(), this->height());
//    web->load(QUrl(KLM_WEB_LOGIN_URL));
    web->load(QUrl(KLM_SERVER_URL_MAIN));
    web->show();
    connect(web, SIGNAL(loadFinished(bool)), this, SLOT(web_loaded(bool)));

    this->channel = new QWebChannel(this);
    web->page()->setWebChannel(this->channel);
    this->channel->registerObject(QStringLiteral("twin"), this);
}

void MainWindow::web_loaded(bool ib) {
    if (ib) {
        QString code = QString::fromLocal8Bit("qtinit()");
        web->page()->runJavaScript(code);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key::Key_F5)
        this->web->reload();
}

void MainWindow::login(const QString &uname, const QString &pwd) {
    this->lg = new klm::network::login_tool;
    klm::info::shared_instance()->username = uname;
    klm::info::shared_instance()->password = pwd;
    connect(this->lg, SIGNAL(login_success(bool)), this, SLOT(process_login(bool)));
    this->lg->login_with(uname, pwd);
}

void MainWindow::test_text(const QString &text) {
    QMessageBox * box = new QMessageBox();
    box->setWindowTitle("now test hitted");
    box->setText(text);
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

void MainWindow::on_testtiaozhuan_clicked() {
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

    if (isok) {
        klm::info::shared_instance()->is_login = true;
    }
    else {
        klm::info::shared_instance()->is_login = false;
        QMessageBox box;
        box.setText(tr("Signin Failed, Please confirm your infomation and retry"));
        box.setWindowTitle(tr("Signin Failed"));
        box.exec();
        this->web->reload();
        return;
    }
}

void MainWindow::open_sketch(const QString & plan_id, const QString & design_id, const QString & path) {
    klm::info::shared_instance()->design_id = design_id;
    klm::info::shared_instance()->plan_id = plan_id;
    klm::info::shared_instance()->d_p_path = path;

    // begin design
    dhw = new drawhomewin(this->parentWidget());
    dhw->show();
    this->releaseMouse();
    this->releaseKeyboard();

    this->setWindowState(Qt::WindowMinimized);
}
