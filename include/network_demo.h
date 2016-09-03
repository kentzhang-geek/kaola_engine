#ifndef NETWORK_DEMO_H
#define NETWORK_DEMO_H

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QJsonObject>
#include <QMessageBox>
#include <QString>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <iostream>
#include <QEventLoop>
#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QVariant>
#include <QDir>

QLineEdit* userNameField;
QLineEdit* passwordField;
QPushButton* login;
QNetworkAccessManager manager;

void finished(){
    std::cout<<"File uploading finished"<<std::endl;
}

void progress( qint64 bytesSent, qint64 bytesTotal ){
    std::cout<<bytesSent<<" out of "<<bytesTotal<<" has been sent"<<std::endl;
}

void downFinished(QNetworkReply *reply){
    if(reply->error()){
        std::cout<<"Failed to download"<<std::endl;
    } else {
        QFile* file = new QFile("/Users/gang_liu/Desktop/6789.mp3");
        if(file->open(QFile::Append)){
            file->write(reply->readAll());
            file->flush();
            file->close();
        }
        delete file;
    }

    reply->deleteLater();
}



void http_file_downloading(const std::string& url){

    QObject::connect(&manager, &QNetworkAccessManager::finished, downFinished);
    manager.get(QNetworkRequest(QUrl(url.c_str())));

}

void http_file_uploading(const std::string& url, const std::string &file_name){

    QEventLoop loop;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;
    std::string content = "form-data; name=\"pn\"; filename=\"";
    content.append("心安理得.mp3").append("\"");
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(content.c_str()));

    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    QFile* file = new QFile(file_name.c_str());
    file->open(QIODevice::ReadOnly);
    filePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(filePart);

    QUrl uploadingURL(url.c_str());
    QNetworkRequest u_request(uploadingURL);

    QNetworkReply* reply = manager.post(u_request, multiPart);
    loop.exec();
    QObject::connect(reply, &QNetworkReply::finished, finished);
    QObject::connect(reply, &QNetworkReply::uploadProgress, progress);

    if(reply->error()){
        std::cout<<"Error in request"<<std::endl;
    } else {
        QString replyStr = reply->readAll();
        std::cout<<"replyStr = "<<replyStr.toUtf8().constData()<<std::endl;
    }

}

void http_post_request(const std::string &url, const QUrlQuery &data){
    static int cnt = 0;
    QEventLoop loop;

    QNetworkRequest request(QUrl(url.c_str()));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply* reply = manager.post(request, data.toString(QUrl::FullyEncoded).toUtf8());
    loop.exec();

    cnt++;

    if(reply->error()){
        std::cout<<"Error in request"<<std::endl;
    } else {
        QString replyStr = reply->readAll();
        std::cout<<"replyStr = "<<replyStr.toUtf8().constData()<<std::endl;

        std::string file_name("/Users/gang_liu/Desktop/123.mp3");
        std::string n_url("http://localhost:8080/u_t");
        http_file_uploading(n_url, file_name);

        http_file_downloading("http://localhost:8080/u_t?f_n=TLIM.mp3");
    }
}

void n_login(){
    if(userNameField->text().isNull() || userNameField->text().isEmpty()){
        QMessageBox msgBox;
        msgBox.setText("用户名不能为空");
        msgBox.exec();
    } else if(passwordField->text().isNull() || passwordField->text().isEmpty()){
        QMessageBox msgBox;
        msgBox.setText("密码不能为空");
        msgBox.exec();
    }
    const std::string userName = userNameField->text().toUtf8().constData();
    const std::string password = passwordField->text().toUtf8().constData();
    QUrlQuery data;
    data.addQueryItem("u_id", userName.c_str());
    data.addQueryItem("pwd", password.c_str());

    std::string url("http://localhost:8080/user");
    http_post_request(url, data);
}

#endif // NETWORK_DEMO_H
