#ifndef GL3D_LOCK_H
#define GL3D_LOCK_H

#include <QMutex>

namespace gl3d {
class gl3d_lock {
public:
    static gl3d_lock * shared_instance();
    QMutex render_lock;
    QMutex wall_lock;
private:
    gl3d_lock();
};
}

#endif // GL3D_LOCK_H
