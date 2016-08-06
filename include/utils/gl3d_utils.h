#ifndef GL3D_UTILS_H
#define GL3D_UTILS_H

// usage : GL3D_UTILS_PROPERTY(member name, member type) in gl3d class
#define GL3D_UTILS_PROPERTY(member_name, ...) \
    private: \
    __VA_ARGS__ member_name;\
    public: \
    void set_##member_name(__VA_ARGS__ member_name##_tag) { \
        this->member_name = member_name##_tag; \
    } \
    __VA_ARGS__ get_##member_name() { \
        return this->member_name; \
    }

#define GL3D_UTILS_PROPERTY_DECLARE(member_name, ...) \
    private: \
    __VA_ARGS__ member_name;\
    public: \
    void set_##member_name(__VA_ARGS__ member_name##_tag); \
    __VA_ARGS__ get_##member_name() ;


#define GL3D_UTILS_PROPERTY_GET_POINTER(member_name, ...) \
    private: \
    __VA_ARGS__ member_name;\
    public: \
    void set_##member_name(__VA_ARGS__ member_name##_tag) { \
        this->member_name = member_name##_tag; \
    } \
    __VA_ARGS__ * get_##member_name() { \
        return &this->member_name; \
    }

#define GL3D_UTILS_ERROR(...) throw QString::asprintf(__VA_ARGS__);

#define GL3D_UTILS_WARN(...) std::cout <<  QString::asprintf(__VA_ARGS__).toStdString() << std::endl;

#define GL3D_UTILS_SHADOW_MAP_ORTHO glm::ortho(-20.0,20.0,-20.0,20.0,-30.0, 200.0)

#define IN
#define OUT
#define INOUT

#endif // GL3D_UTILS_H
