#ifndef GLOBAL_INFO_H
#define GLOBAL_INFO_H

#include <QString>
#include <QMap>
#include <QVector>

namespace klm {
  class info {
  public:
      static info * shared_instance();
      bool is_login;
      QString username;
      QString password;
      QString design_id;
      QString plan_id;
      QString d_p_path;

  private:
      info();
  };
};

#endif // GLOBAL_INFO_H
