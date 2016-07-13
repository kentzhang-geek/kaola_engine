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

#endif // GL3D_UTILS_H
