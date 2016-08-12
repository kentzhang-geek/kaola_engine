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
#include <QMap>

#include "kaola_engine/gl3d.hpp"

#include "utils/gl3d_utils.h"

//class gl3d::gl3d_material;
//class gl3d::abstract_object;

using namespace std;

namespace klm{
class Merchandise{

public:
    enum class MerchandiseType{
        Surfacing = 0,
        Furniture
    };

public:
    Merchandise(const string &id);
    ~Merchandise();

public:
    virtual glm::mat4 getRotation() const;
    virtual void setRotation(const glm::mat4 &rotation);

    virtual glm::vec3 getTranslation() const;
    virtual void setTranslateiong(const glm::vec3 &translation);

    virtual glm::mat4 getTransformation() const;

    virtual string getID() const;

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
    MerchandiseType getType() ;

};

class Furniture : public Merchandise{
public:
    Furniture(const string &id);
    ~Furniture();
    float getPrice() ;
    MerchandiseType getType() ;
    virtual string getPickID() ;
    virtual void setPickID(string pickID);
private:
    string pickID;
};

}

#endif // MERCHANDISE_H
