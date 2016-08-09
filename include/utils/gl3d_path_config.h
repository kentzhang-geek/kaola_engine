#ifndef GL3D_PATH_CONFIG_H
#define GL3D_PATH_CONFIG_H

#ifdef _WIN32
#define GL3D_PATH_SEPRATOR "\\"
#define GL3D_PATH_SHADER \
    "C:\\Users\\Administrator\\Desktop\\Qt_Projects\\qt_opengl_engine\\shaders"
#define GL3D_PATH_MODELS \
    "C:\\Users\\Administrator\\Desktop\\Qt_Projects\\qt_opengl_engine"
#else
#define GL3D_PATH_SEPRATOR "/"
#define GL3D_PATH_SHADER "/Users/gang_liu/Develop/QT_Project/kaolamao/shaders_mac"
#define GL3D_PATH_MODELS "/Users/gang_liu/Develop/QT_Project/kaolamao/models"
#endif

#endif // GL3D_PATH_CONFIG_H
