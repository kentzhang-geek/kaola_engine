#ifndef GL3D_UTILS_H
#define GL3D_UTILS_H

#include "pugiconfig.hpp"
#include "pugixml.hpp"
#include <QString>

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

#define GL3D_UTILS_PROPERTY_VIRTUAL(member_name, ...) \
    private: \
    __VA_ARGS__ member_name;\
    public: \
    virtual void set_##member_name(__VA_ARGS__ member_name##_tag) { \
        this->member_name = member_name##_tag; \
    } \
    virtual __VA_ARGS__ get_##member_name() { \
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

#define GL3D_UTILS_THROW(...) \
    throw QString::asprintf("Throw Error at %s : %d ==> ", __FILE__, __LINE__) + QString::asprintf(__VA_ARGS__);


#define IN
#define OUT
#define INOUT

// perform noncopyable
namespace utils {
    class noncopyable {
    protected:
        noncopyable() {}

        ~noncopyable() {}

    private:  // emphasize the following members are private
        noncopyable(const noncopyable &);

        const noncopyable &operator=(const noncopyable &);
    };
}

namespace gl3d {
    namespace xml {
        template <typename T>
        void save_vec_to_xml(T vec, pugi::xml_node & node) {
            int dimenson = vec.length();
            node.append_attribute("type").set_value(QString::asprintf("glm_vec%i", dimenson).toStdString().c_str());
            for (int i = 0; i < vec.length(); i++) {
                node.append_attribute(QString::asprintf("d_%i", i).toStdString().c_str()).set_value(vec[i]);
            }

            return;
        }

        template <typename T>
        void load_xml_to_vec(pugi::xml_node node, T & vec) {
            vec = T(0.0f);
            std::string type = (node.attribute("type").value());
            int dimenson = vec.length();
            if (type != QString::asprintf("glm_vec%i", dimenson).toStdString()) {
                return;
            }
            for (int i = 0; i < vec.length(); i++) {
                vec[i] = node.attribute(QString::asprintf("d_%i", i).toStdString().c_str()).as_float();
            }
        }

        template <typename T>
        void save_mat_to_xml(T mt, pugi::xml_node & node) {
            int dimeson = mt.length();
            node.append_attribute("type").set_value(QString::asprintf("glm_mat%i", dimeson).toStdString().c_str());
            for (int i = 0; i < mt.length(); i++) {
                pugi::xml_node tmp = node.append_child(QString::asprintf("d_%i", i).toStdString().c_str());
                save_vec_to_xml(mt[i], tmp);
            }
        }

        template <typename T>
        void load_xml_to_mat(pugi::xml_node node, T &mt) {
            mt = T(0.0f);
            std::string type = (node.attribute("type").value());
            int dimeson = mt.length();
            if (type != QString::asprintf("glm_mat%i", dimeson).toStdString()) {
                return;
            }
            for (int i = 0; i < mt.length(); i++) {
                pugi::xml_node tmp = node.child(QString::asprintf("d_%i", i).toStdString().c_str());
                load_xml_to_vec(tmp, mt[i]);
            }
        }
    }
}


#endif // GL3D_UTILS_H
