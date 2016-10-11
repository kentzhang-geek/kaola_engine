#include "resource_and_network/pack_tool.h"

using namespace klm;

QString pack_tool::pack_password;

bool pack_tool::check_excutable() {
    QFile exe("7z.exe");
    QFile dll("7z.dll");
    return exe.exists() && dll.exists();
}

void pack_tool::init_with_password(QString pwd) {
    pack_tool::pack_password = pwd;
}

void pack_tool::pack(QVector<QString> source_files, QString pack_file) {
    QString exe("7z.exe");
    QString space(" ");
    QString cmd = exe;
    if (pack_tool::pack_password.length() > 0) {
        cmd += space + "-p" + pack_tool::pack_password;
    }

    cmd += space + "u" + space + pack_file;

    Q_FOREACH(QString fit, source_files) {
            cmd += space + fit;
        }

    qDebug(cmd.toStdString().c_str());
    QProcess * zip_process = new QProcess;
    zip_process->start(cmd);
    connect(zip_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(process_done(int, QProcess::ExitStatus)));

    return;
}

void pack_tool::process_done(int eCode, QProcess::ExitStatus eStatus) {
    qDebug("pack process finished");
    emit finished();
    return;
}

void pack_tool::unpack(QString pack_file, QString dst_dir) {
    QString exe("7z.exe");
    QString space(" ");
    QString cmd = exe;
    if (pack_tool::pack_password.length() > 0) {
        cmd += space + "-p" + pack_tool::pack_password;
    }

    cmd += space + "x " + pack_file;

    if (dst_dir.length() > 0) {
        cmd += " -o" + dst_dir;
    }

    qDebug(cmd.toStdString().c_str());
    QProcess * zip_process = new QProcess;
    zip_process->start(cmd);
    connect(zip_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(process_done(int, QProcess::ExitStatus)));

    return;
}
