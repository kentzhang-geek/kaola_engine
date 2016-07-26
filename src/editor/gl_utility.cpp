#include "editor/gl_utility.h"

using namespace klm;
using namespace std;

void GLUtility::normalize(Vertex &vertex) noexcept{
    glm::vec3 vec;
    vertex.getPosition(vec);
    glm::normalize(vec);

    if(vec.x < 0.000001f && vec.x > -0.000001f){
        vec.x = 0.0f;
    } else if(vec.x >= 0.99999f){
        vec.x = 1.0f;
    } else if(vec.x <= -0.99999f){
        vec.x = -1.0f;
    }

    if(vec.y < 0.000001f && vec.y > -0.000001f){
        vec.y = 0.0f;
    } else if(vec.y >= 0.99999f){
        vec.y = 1.0f;
    } else if(vec.y <= -0.99999f){
        vec.y = -1.0f;
    }

    if(vec.z < 0.000001f && vec.z > -0.000001f){
        vec.z = 0.0f;
    } else if(vec.z >= 0.99999f){
        vec.z = 1.0f;
    } else if(vec.x <= -0.99999f){
        vec.z = -1.0f;
    }

    vertex.setX(vec.x);
    vertex.setY(vec.y);
    vertex.setZ(vec.z);
}

glm::vec3 GLUtility::getNormal(const Vertex &p1, const Vertex &p2,
                          const Vertex &p3) noexcept{
    glm::vec3 vp1;
    glm::vec3 vp2;
    glm::vec3 vp3;

    p1.getPosition(vp1);
    p2.getPosition(vp2);
    p3.getPosition(vp3);

    if(vp1 == vp2 || vp1 == vp3 || vp2 == vp3){
        return NON_NORMAL;
    }

    glm::vec3 lineOne = vp1 - vp2;
    glm::vec3 lineTwo = vp3 - vp2;
    glm::vec3 normal = glm::cross(lineTwo, lineOne);
    glm::normalize(normal);

    return normal;
}

void GLUtility::getRotation(const glm::vec3 &source,
                            const glm::vec3 &dest,
                            glm::mat4 &matrix) noexcept{
    if(source == dest){
        matrix = glm::mat4(1.0f);
    } else {
        GLfloat sourceLen = glm::length(source);
        GLfloat destLen = glm::length(dest);
        if(sourceLen == 0.0f || destLen == 0.0f){
            matrix = glm::mat4(1.0f);
        } else {
            matrix =glm::rotate(
                    glm::acos(glm::dot(source, dest)/(sourceLen * destLen)),
                    glm::cross(source, dest));
        }
    }
}

glm::vec3 GLUtility::getPlaneNormal(const QVector<glm::vec3> &points) noexcept{
    if(points.size() <= 3){
        return NON_NORMAL;
    }

    glm::vec3 sampleNormal = getNormal(points.back(),
                                  points.front(),
                                  points[1]);
    for(int index = 1; index != (points.size() -1); ++index){
        glm::vec3 testNormal = getNormal(points[index -1],
                                    points[index],
                                    points[index + 1]);
        if(testNormal != sampleNormal){
            return NON_NORMAL;
        }
    }
    return sampleNormal;
}

void GLUtility::positionTransform(Vertex &vertex, const glm::mat4 &matrix) noexcept{
    glm::vec3 vec;
    vertex.getPosition(vec);
    glm::vec4 temp(vec, 1.0f);
    temp = matrix * temp;
    vertex.setX(temp.x);
    vertex.setY(temp.y);
    vertex.setZ(temp.z);
}

void GLUtility::textureTransform(Vertex &vertex, const glm::mat4 &matrix) noexcept{
    glm::vec2 texture;
    vertex.getTexture(texture);
    glm::vec4 temp(texture, 1.0f, 1.0f);
    temp =  matrix * temp;
    vertex.setW(temp.x);
    vertex.setH(temp.y);
}

const glm::vec3 GLUtility::NON_NORMAL = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 GLUtility::Z_AXIS = glm::vec3(0.0f, 0.0f, 1.0f);