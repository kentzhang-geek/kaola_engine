#include "resource_and_network/network_tool.h"

using namespace klm;
using namespace klm::network;
using namespace klm::network::common_tools;

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
                emit login_success(true);
                return;
            }
        }
    }

    emit login_success(false);
    return;
}
