#ifndef MERCHANDISE_H
#define MERCHANDISE_H

//replace this to windows opengl header

#ifdef _WIN32
#elif defined __APPLE__
    #include <OpenGL/gl3.h>
    #include <OpenGL/gl3ext.h>
#endif


#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <map>

using namespace std;

namespace klm{
    class Merchandise{

    enum class MerchandiseType{
        Surfacing, Furniture
    };

    protected:
        Merchandise(const string &id);
        ~Merchandise();

    public:
        glm::mat4 getRotation() const;
        void setRotation(const glm::mat4 &rotation);

        glm::vec3 getTranslation() const;
        void setTranslateiong(const glm::vec3 &translation);

        glm::mat4 getTransformation() const;

        string getID() const;

        virtual MerchandiseType getType() = 0;
        virtual float getPrice() = 0;
    protected:
        string id;
        glm::mat4 *rotation;
        glm::vec3 *translation;
        float basePrice;
    };

    /**
     * @brief this class is used for surface material. it should be texture only
     * when it is used in this system.
     */
    class Surfacing : public Merchandise{
    public:
        Surfacing(const string &id);
        ~Surfacing();
        float getPrice();
        MerchandiseType getType() const;
    public:
        const static int TEXTURE_SIZE;
    private:
        //id map from this surfacing to texture ids
        static map<string, std::array<GLuint, TEXTURE_SIZE>> idMap;
    };

    class Furniture : public Merchandise{
    public:
        Furniture(const string &id);
        ~Furniture();
        float getPrice() const;

    private:
        //id from this furniture to VBO
        static map<string, GLuint> idMap;
    };

}

#endif // MERCHANDISE_H
