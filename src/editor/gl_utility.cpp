#include "editor/gl_utility.h"

using namespace klm;
using namespace glm;
using namespace std;

void GLUtility::normalize(Vertex &vertex) noexcept{
    vec3 vec;
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

vec3 GLUtility::getNormal(const Vertex &p1, const Vertex &p2,
                          const Vertex &p3) noexcept{
    vec3 vp1;
    vec3 vp2;
    vec3 vp3;

    p1.getPosition(vp1);
    p2.getPosition(vp2);
    p3.getPosition(vp3);

    if(vp1 == vp2 || vp1 == vp3 || vp2 == vp3){
        return NON_NORMAL;
    }

    vec3 lineOne = vp1 - vp2;
    vec3 lineTwo = vp3 - vp2;
    vec3 normal = glm::cross(lineTwo, lineOne);
    glm::normalize(normal);

    return normal;
}

void GLUtility::getRotation(const vec3 &source, const vec3 &dest, mat4 &matrix) noexcept{
    if(source == dest){
        matrix = mat4(1.0f);
    } else {
        GLfloat sourceLen = glm::length(source);
        GLfloat destLen = glm::length(dest);
        if(sourceLen == 0.0f || destLen == 0.0f){
            matrix = mat4(1.0f);
        } else {
            matrix =glm::rotate(
                    glm::acos(glm::dot(source, dest)/(sourceLen * destLen)),
                    glm::cross(source, dest));
        }
    }
}

vec3 GLUtility::getPlaneNormal(const vector<vec3> &points) noexcept{
    if(points.size() <= 3){
        return NON_NORMAL;
    }

    vec3 sampleNormal = getNormal(points.back(),
                                  points.front(),
                                  points[1]);
    for(int index = 1; index != (points.size() -1); ++index){
        vec3 testNormal = getNormal(points[index -1],
                                    points[index],
                                    points[index + 1]);
        if(testNormal != sampleNormal){
            return NON_NORMAL;
        }
    }
    return sampleNormal;
}

void GLUtility::positionTransform(Vertex &vertex, const mat4 &matrix) noexcept{
    vec3 vec;
    vertex.getPosition(vec);
    vec4 temp(vec, 1.0f);
    temp = matrix * temp;
    vertex.setX(temp.x);
    vertex.setY(temp.y);
    vertex.setZ(temp.z);
}

void GLUtility::textureTransform(Vertex &vertex, const mat4 &matrix) noexcept{
    vec2 texture;
    vertex.getTexture(texture);
    vec4 temp(texture, 1.0f, 1.0f);
    temp =  matrix * temp;
    vertex.setW(temp.x);
    vertex.setH(temp.y);
}

const vec3 GLUtility::NON_NORMAL = vec3(0.0f, 0.0f, 0.0f);
const vec3 GLUtility::Z_AXIS = vec3(0.0f, 0.0f, 1.0f);
