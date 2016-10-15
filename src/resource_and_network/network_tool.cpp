#include "resource_and_network/network_tool.h"

using namespace klm;
using namespace klm::network;
using namespace klm::network::common_tools;

QNetworkAccessManager manager;

QNetworkAccessManager* klm::network::shared_mgr() {
    return &manager;
}

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

QJsonDocument klm::network::call_web_new(QString name, QString url, QString web_new_url) {
    QEventLoop loop;
    QNetworkAccessManager * gMgr = klm::network::shared_mgr();

    QUrl uploadingURL(QString(KLM_SERVER_URL) + web_new_url);
    uploadingURL.setUrl(QString(KLM_SERVER_URL) + web_new_url);
    QNetworkRequest u_request(uploadingURL);
    u_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery qua;
    qua.addQueryItem("name", name);
    qua.addQueryItem("url", url);
    QNetworkReply* reply = gMgr->post(u_request, qua.toString(QUrl::FullyEncoded).toUtf8());
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QJsonDocument doc;
    if (reply->error()) {
        qDebug(reply->errorString().toStdString().c_str());
    }
    else {
        QByteArray data = reply->readAll();
        qDebug(data.data());
        doc = QJsonDocument::fromJson(data);
    }
    delete reply;

    return doc;
}

QJsonDocument
klm::network::call_web_file_upload(QString id, QString filename, QString upload_fn, QString upload_url) {
    QEventLoop loop;
    QNetworkAccessManager * gMgr = klm::network::shared_mgr();

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;
    QString content = "form-data; name=\"cat_file\"; filename=\"" + upload_fn + "\"";
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(content));
    QFile* file = new QFile(filename);
    file->open(QFile::ReadOnly);
    filePart.setBodyDevice(file);
    file->setParent(multiPart);

    QHttpPart idPart;
    idPart = QHttpPart();
    idPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"id\""));
    idPart.setBody(QVariant(id).toByteArray());
    multiPart->append(idPart);
    multiPart->append(filePart);

    QUrl uploadingURL = QUrl(QString(KLM_SERVER_URL) + upload_url);
    uploadingURL.setUrl(QString(KLM_SERVER_URL) + upload_url);
    QNetworkRequest u_request = QNetworkRequest(uploadingURL);
    u_request.setHeader(QNetworkRequest::ContentTypeHeader, QString("multipart/form-data; boundary=") + multiPart->boundary());
    u_request.setRawHeader("Accept", "application/json, text/plain, */*");
    QNetworkReply * reply = gMgr->post(u_request, multiPart);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    file->close();
    delete file;

    QJsonDocument doc;
    if (reply->error()) {
        qDebug(reply->errorString().toStdString().c_str());
    }
    else {
        QByteArray data = reply->readAll();
        qDebug(data.data());
        doc = QJsonDocument::fromJson(data);
    }
    delete reply;

    return doc;
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

    QString sysinfo = QSysInfo::kernelType();
    sysinfo += QSysInfo::kernelVersion();
    sysinfo += QSysInfo::currentCpuArchitecture();

    data.addQueryItem("os", sysinfo.toStdString().c_str());

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