#ifndef GL3D_PATH_CONFIG_H
#define GL3D_PATH_CONFIG_H

#ifdef _WIN32
#define GL3D_PATH_SEPRATOR "\\"
#define GL3D_PATH_SHADER \
    "C:\\Users\\Administrator\\Desktop\\Qt_Projects\\qt_opengl_engine\\shaders"
#define GL3D_PATH_MODELS \
    "C:\\Users\\Administrator\\Desktop\\Qt_Projects\\qt_opengl_engine\\shaders"
#else
#define GL3D_PATH_SEPRATOR "/"
#define GL3D_PATH_SHADER ""
#define GL3D_PATH_MODELS ""
#endif

#endif // GL3D_PATH_CONFIG_H
