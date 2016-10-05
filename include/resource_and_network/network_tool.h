#ifndef NETWORK_TOOL_H
#define NETWORK_TOOL_H


#include <stdio.h>
#include <iterator>
#include <string>
#include <iostream>

// Qt Headers
#include <QVector>
#include <QMap>
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
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <iostream>
#include <QEventLoop>
#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QtNetwork/QHttpMultiPart>
#include <QtNetwork/QHttpPart>
#include <QVariant>
#include <QDir>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "kaola_engine/Program.hpp"
#include "kaola_engine/log.h"
#include "kaola_engine/shader_manager.hpp"
#include "kaola_engine/gl3d_texture.hpp"
#include "kaola_engine/gl3d_material.hpp"
#include "kaola_engine/gl3d_obj_authority.h"
#include "kaola_engine/gl3d_general_texture.hpp"
#include "kaola_engine/gl3d_render_process.hpp"

// OpenGL in windows should use GLEW
#include "kaola_engine/glheaders.h"

// kaolaengine components
#include "utils/gl3d_utils.h"
#include "utils/gl3d_scale.h"
#include "utils/gl3d_global_param.h"
#include "kaola_engine/gl3d_general_light_source.h"
#include "kaola_engine/gl3d_mesh.h"
#include "kaola_engine/gl3d_object.h"
#include "kaola_engine/gl3d_scene.h"
#include "utils/gl3d_math.h"
#include "utils/gl3d_global_param.h"

namespace klm {
    namespace network {
        namespace common_tools {
            class poster : public QObject {
                Q_OBJECT
            public:
                QString host;
                QNetworkAccessManager mgr;
                QNetworkReply * rply;

                poster();
                void post_request(QUrl url, QUrlQuery & data);

            public slots:
                void reply_finished();

            signals:
                void complete();
            };
        }

        class login_tool : public QObject{
            Q_OBJECT
        public:
            const QString server_url = "http://192.168.0.105:8080/";
            common_tools::poster * per;
            login_tool();
            void login_with(QString username, QString password);

        signals:
            void login_success(bool);

        public slots:
            void process_post();
        };
    }
}

#endif // NETWORK_TOOL_H
