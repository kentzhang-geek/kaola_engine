#include "resource_and_network/network_tool.h"

using namespace klm;
using namespace klm::network;
using namespace klm::network::common_tools;

QNetworkAccessManager manager;

poster::poster() {
    this->rply = NULL;
    return;
}

void poster::post_request(QUrl url, QUrlQuery & data) {
    QNetworkRequest request = QNetworkRequest(url);
    if (this->mgr.networkAccessible() == QNetworkAccessManager::NotAccessible) {
        return ;
    }
    // what a fucking bug
    this->mgr.connectToHost(url.host());
//    this->mgr.post(QNetworkRequest(QUrl("http://www.baidu.com/")), QByteArray());

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader, data.toString(QUrl::FullyEncoded).toUtf8().length());
    QNetworkReply* reply = this->mgr.post(request, data.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply, SIGNAL(finished()), this, SLOT(reply_finished()));
    this->rply = reply;
    return;
}

void poster::reply_finished() {
    emit complete();
}


login_tool::login_tool() {
    return ;
}

#if 0
void login_tool::login_with(QString username, QString password) {
    this->per = new common_tools::poster;
    this->per->host = this->server_url;
    QUrlQuery data;
    data.addQueryItem("id", username.toUtf8().constData());
    data.addQueryItem("pwd", password.toUtf8().constData());
    connect(per, SIGNAL(complete()), this, SLOT(process_post()));
    QUrl ss((this->server_url + "login"));
    this->per->post_request(ss, data);
    return;
}
#endif

void login_tool::process_post() {
    if (this->per->rply->error()) {
        qDebug("Error is Login Post");
    }
    QByteArray arr = this->per->rply->readAll();
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(arr);
    if (!doc.isEmpty()) {
        QJsonObject jobj = doc.object();
        if (jobj.contains("status")) {
            if (jobj.value("status").toString() == "OK") {
                qDebug("OK");
                emit login_success(true);
                return;
            }
        }
    }

    emit login_success(false);
    return;
}

bool http_post_request(const std::string &url, const QUrlQuery &data){
    QEventLoop loop;

    QNetworkRequest request(QUrl(url.c_str()));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply* reply = manager.post(request, data.toString(QUrl::FullyEncoded).toUtf8());
    loop.exec();

    if(reply->error()){
        qDebug("Error in request");
    } else {
        QByteArray data = reply->readAll();
        qDebug(data.data());
        QJsonDocument doc;
        doc = QJsonDocument::fromJson(data);
        if (!doc.isEmpty()) {
            QJsonObject jobj = doc.object();
            if (jobj.contains("status")) {
                if (jobj.value("status").toString() == "OK") {
                    qDebug("OK");
                    return true;
                }
            }
        }
    }

    return false;
}

#if 1
void login_tool::login_with(QString uname, QString pwd) {
    const std::string userName = uname.toUtf8().constData();
    const std::string password = pwd.toUtf8().constData();
    QUrlQuery data;
    data.addQueryItem("id", userName.c_str());
    data.addQueryItem("pwd", password.c_str());

    std::string url(this->server_url.toStdString() + "login");
    bool is_success = http_post_request(url, data);
    if (is_success) {
        emit login_success(true);
    }
    else {
        emit login_success(false);
    }
}
#endif