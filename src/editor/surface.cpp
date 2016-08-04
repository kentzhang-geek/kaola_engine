#include "editor/surface.h"
#include <iostream>
#include <QMutex>

using namespace klm_1;
using namespace std;

const int Surface::Vertex::X      = 0;
const int Surface::Vertex::Y      = 1;
const int Surface::Vertex::Z      = 2;
const int Surface::Vertex::W      = 3;
const int Surface::Vertex::H      = 4;
const int Surface::Vertex::SIZE   = 5;

GLUtesselator* Surface::tess      = nullptr;
Surface* Surface::targetSurface   = nullptr;

const glm::vec3 Surface::NON_NORMAL(0.0, 0.0, 0.0);
const glm::vec3 Surface::Z_AXIS(0.0, 0.0, 1.0);

static QMutex tess_locker;

SurfaceException::SurfaceException(const string &message){
    this->message = message;
}

std::string SurfaceException::what() const{
    return message;
}

Surface::Surface(const QVector<glm::vec3> &points) throw(SurfaceException)
    : parent(nullptr), visible(false), subSurfaces(nullptr),
      localVerticies(new QVector<Surface::Vertex*>),
      scale(nullptr), rotation(nullptr),translate(nullptr){

    if(points.size() < 3){
        throw SurfaceException("can not create Surface using less then three points");
    }

    glm::vec3 planeNormal = getPlanNormal(points);
    if(planeNormal == NON_NORMAL){
        throw SurfaceException("can not create Surface if not all points are on same plane");
    }

    independShape = new Polygon();
    parentialShape = new Polygon();

    //set verticies given by consturctor to local vertices
    //and get transform from parent
    boundingBox = new BoundingBox(points);
    glm::vec3 center = boundingBox->getCenter();
    glm::mat4 rotation;
    getRotation(planeNormal, Z_AXIS, rotation);
    for(QVector<glm::vec3>::const_iterator point = points.begin();
        point != points.end(); ++point){

        Vertex* vertex = new Vertex(*point);                

        parentialShape->outer().push_back(Point(vertex->x(), vertex->y()));
        vertex->x(vertex->x() - center.x);
        vertex->y(vertex->y() - center.y);
        vertex->z(vertex->z() - center.z);

        transformVertex(rotation, *vertex);
        localVerticies->push_back(vertex);
        independShape->outer().push_back(Point(vertex->x(), vertex->y()));
    }           

    boundingBox->reset();
    boundingBox->generateTexture(*localVerticies);
    transformFromParent = new glm::mat4(glm::translate(center) * glm::inverse(rotation));
}

Surface::~Surface(){
    if(parentialShape != nullptr){
        delete parentialShape;
    }
    if(independShape != nullptr){
        delete independShape;
    }

    if(boundingBox != nullptr){
        delete boundingBox;
    }

    if(parent != nullptr){
        parent = nullptr;
    }

    if(subSurfaces != nullptr){
        for(QVector<Surface*>::iterator subSurface = subSurfaces->begin();
            subSurface != subSurfaces->end(); ++subSurface){
            delete *subSurface;
        }
        subSurfaces->clear();
        delete subSurfaces;
    }

    deleteVerticies(localVerticies);

    if(transformFromParent != nullptr){
        delete transformFromParent;
    }

    if(scale != nullptr){
        delete scale;
    }

    if(rotation != nullptr){
        delete rotation;
    }

    if(translate != nullptr){
        delete translate;
    }

}

Surface* Surface::addSubSurface(const QVector<glm::vec3> &points){
    Surface* ret = nullptr;
    try{
        ret = new Surface(points);
    } catch(SurfaceException &ex){
        return nullptr;
    }

    bool subSurfaceFits =
            boost::geometry::covered_by((*this->independShape), (*ret->parentialShape));

    for(QVector<Surface*>::iterator subSurface = subSurfaces->begin();
        subSurface != subSurfaces->end(); ++subSurface){

        subSurfaceFits = subSurfaceFits &&
                boost::geometry::intersects(
                    (*ret->independShape), (*(*subSurface)->independShape));

        if(!subSurfaceFits){
            delete ret;
            return nullptr;
        }
    }

    if(subSurfaceFits){
        subSurfaces->push_back(ret);
        return  ret;
    } else {
        return nullptr;
    }
}

int Surface::getSurfaceCnt() const{
    return subSurfaces == nullptr ?  0 : subSurfaces->size();
}

Surface* Surface::getSubSurface(const unsigned int index){
    return subSurfaces == nullptr || subSurfaces->size() <= index ?
                nullptr : subSurfaces->at(index);
}

bool Surface::removeSubSurface(const Surface *subSurface){
    for(QVector<Surface*>::iterator sub = subSurfaces->begin();
        sub != subSurfaces->end(); ++sub){
        if((*sub) == subSurface){
            subSurfaces->erase(sub);
            delete subSurface;
            return true;
        } else {
            sub ++;
        }
    }
    return false;
}

bool Surface::isRoot() const{
    return parent != nullptr;
}

glm::mat4 Surface::getTransformFromParent() const{
    return glm::mat4(*this->transformFromParent);
}

glm::mat4 Surface::getTransformFromWorld() const{
    glm::mat4 matrix;
    if(parent != nullptr){
        matrix *= parent->getTransformFromWorld();
    }
    matrix *= getSurfaceTransform();
    matrix *= getTransformFromParent();
    return matrix;
}

void Surface::getLocalVertices(QVector<Vertex*> &localVertices) const{
    for(QVector<Vertex*>::iterator localVertex = this->localVerticies->begin();
        localVertex != this->localVerticies->end(); ++localVertex){
        localVertices.push_back(new Vertex(**localVertex));
    }
}

void Surface::getVerticesOnParent(QVector<Vertex*> &verticesOnParent) const{
    glm::mat4 trans = getTransformFromParent();
    for(QVector<Vertex*>::iterator localVertex = this->localVerticies->begin();
        localVertex != this->localVerticies->end(); ++localVertex){
        Vertex* newVertex = new Vertex(**localVertex);
        transformVertex(trans, *newVertex);
        newVertex->z(0.0f);
        verticesOnParent.push_back(newVertex);
    }
}

void Surface::getVerticesToParent(QVector<Vertex *> &verticesToParent) const{
    glm::mat4 trans = getSurfaceTransform() * getTransformFromParent();
    for(QVector<Vertex*>::iterator localVertex = this->localVerticies->begin();
        localVertex != this->localVerticies->end(); ++localVertex){
        Vertex* newVertex = new Vertex(**localVertex);
        transformVertex(trans, *newVertex);
        verticesToParent.push_back(newVertex);
    }
}

void Surface::setScale(const glm::vec3 &scale){
    if(this->scale == nullptr){
        this->scale = new glm::vec3(scale);
    } else {
        *(this->scale) = scale;
    }
}

glm::mat4 Surface::getSurfaceTransform() const{
    glm::mat4 matrix = glm::mat4(1.0);
    if(scale != nullptr){
        matrix = glm::scale(matrix, *scale);
    }
    if(rotation != nullptr){
        matrix = (*rotation) * matrix;
    }
    if(translate != nullptr){
        matrix = glm::translate(matrix, *translate);
    }
    return matrix;
}

void Surface::updateSurfaceMesh(){}

void Surface::updateConnectionMesh(){}

bool Surface::tesselate(const Surface *surface){
    tess_locker.lock();

    if(tess == nullptr){
        tess = gluNewTess();
        gluTessCallback(tess, GLU_TESS_BEGIN,
                        (void (__stdcall*)(void)) tessBegin);
        gluTessCallback(tess, GLU_TESS_VERTEX,
                        (void (__stdcall*)())     tessVertex);
        gluTessCallback(tess, GLU_TESS_END,
                        (void (__stdcall*)(void)) tessEnd);
        gluTessCallback(tess, GLU_TESS_EDGE_FLAG_DATA,
                        (void (__stdcall*)(void)) tessEdge);
        gluTessCallback(tess, GLU_TESS_ERROR,
                        (void (__stdcall*)(void)) tessError);
    }
    if(targetSurface != nullptr){
        if(TESS_DEBUG){
            cout<<"Tessellation begin with target Surface is not null"<<endl;
        }
        return false;
    }

    targetSurface = surface;


    tess_locker.unlock();
}

bool Surface::equalVecr(const glm::vec3 &v1, const glm::vec3 &v2){
    return
            (v1.x - v2.x < 0.0000001) && (v1.x - v2.x > -0.0000001) &&
            (v1.y - v2.y < 0.0000001) && (v1.y - v2.y > -0.0000001) &&
            (v1.z - v2.z < 0.0000001) && (v1.z - v2.z > -0.0000001);
}

glm::vec3 Surface::getNormal(const Surface::Vertex &p1, const Surface::Vertex &p2, const Surface::Vertex &p3){
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
    normal = glm::normalize(normal);

    return normal;
}

glm::vec3 Surface::getPlanNormal(const QVector<glm::vec3> &points){
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
        if(!equalVecr(testNormal,sampleNormal)){
            return NON_NORMAL;
        }
    }
    return sampleNormal;
}

void Surface::getRotation(const glm::vec3 &source,
                          const glm::vec3 &destation, glm::mat4 &result){
    glm::vec3 tempSource = source;
    tempSource = glm::normalize(tempSource);
    if(tempSource == destation){
        result = glm::mat4(1.0f);
    } else {
        GLfloat sourceLen = glm::length(source);
        GLfloat destLen = glm::length(destation);
        if(sourceLen == 0.0f || destLen == 0.0f){
            result = glm::mat4(1.0f);
        } else {
            glm::vec3 crossVec = glm::cross(source, destation);
            if(crossVec == NON_NORMAL){
                crossVec.y = 1.0f;
            }
            result =glm::rotate(
                    glm::acos(glm::dot(source, destation)/(sourceLen * destLen)),
                    crossVec);
        }
    }
}

void Surface::transformVertex(const glm::mat4 &trans, Surface::Vertex &vertex){
    glm::vec4 v(vertex.x(), vertex.y(), vertex.z(), 1.0f);
    v = trans * v;
    vertex.x(v.x);
    vertex.y(v.y);
    vertex.z(v.z);
}

void Surface::deleteVerticies(QVector<Surface::Vertex*> *vertices){
    for(QVector<Surface::Vertex*>::iterator vertex = vertices->begin();
        vertex != vertices->end(); ++vertex){
        delete *vertex;
    }
    vertices->clear();
    delete vertices;
}

Surface::Vertex::Vertex(const Surface::Vertex &another){
    data = new GLdouble[SIZE];
    this->x(another.x());
    this->y(another.y());
    this->z(another.z());
    this->w(another.w());
    this->h(another.h());
}

Surface::Vertex::Vertex(const glm::vec3 &point){
    data = new GLdouble[5];
    x(point.x);
    y(point.y);
    z(point.z);
    w(0);
    h(0);
}

Surface::Vertex::Vertex(GLdouble x, GLdouble y,
                         GLdouble z, GLdouble w, GLdouble h){
    data = new GLdouble[SIZE];
    this->x(x);
    this->y(y);
    this->z(z);
    this->w(w);
    this->h(h);
}

Surface::Vertex::~Vertex(){
    delete[] data;
}

GLdouble Surface::Vertex::x() const{
    return data[X];
}

void Surface::Vertex::x(GLdouble x){
    this->data[X] = x;
}

GLdouble Surface::Vertex::y() const{
    return data[Y];
}

void Surface::Vertex::y(GLdouble y){
    this->data[Y] = y;
}

GLdouble Surface::Vertex::z() const{
    return data[Z];
}

void Surface::Vertex::z(GLdouble z){
    this->data[Z] = z;
}

GLdouble Surface::Vertex::w() const{
    return data[W];
}

void Surface::Vertex::w(GLdouble w){
    this->data[W] = w;
}

GLdouble Surface::Vertex::h() const{
    return data[H];
}

void Surface::Vertex::h(GLdouble h){
    this->data[H] = h;
}

bool Surface::Vertex::equals(const Surface::Vertex &another) const{
    if(&another == this){
        return true;
    } else {
        for(int index = 0; index != SIZE; ++index){
            if(this->data[index] != another.data[index]){
                return false;
            }
        }
        return true;
    }
}

GLfloat Surface::Vertex::distance(const Surface::Vertex &another) const{
    glm::vec3 source;
    glm::vec3 desti;
    this->getPosition(source);
    another.getPosition(desti);
    return glm::distance(source, desti);
}

void Surface::Vertex::getPosition(glm::vec3 &position) const{
    position.x = this->x();
    position.y = this->y();
    position.z = this->z();
}

void Surface::Vertex::setPosition(const glm::vec3 &position) {
    this->x(position.x);
    this->y(position.y);
    this->z(position.z);
}

void Surface::Vertex::getTexture(glm::vec2 &texture) const{
    texture.x = this->w();
    texture.y = this->y();
}

void Surface::Vertex::setTexture(const glm::vec2 &texture){
    this->w(texture.x);
    this->h(texture.y);
}

GLdouble* Surface::Vertex::getData(){
    return data;
}

Surface::BoundingBox::BoundingBox(const QVector<glm::vec3> &verticies){
    init();
    for(QVector<glm::vec3>::const_iterator vertex = verticies.begin();
        vertex != verticies.end(); ++vertex){
        setMinimal(*vertex);
        setMaximal(*vertex);
    }
}

glm::vec3 Surface::BoundingBox::getCenter() const{
    return glm::vec3(
                (xMin + xMax)/2,
                (yMin + yMax)/2,
                (zMin + zMax)/2);
}

void Surface::BoundingBox::generateTexture(const QVector<Surface::Vertex*> &verticies) const{
    for(QVector<Surface::Vertex*>::const_iterator vertex = verticies.begin();
        vertex != verticies.end(); ++vertex){
        (*vertex)->w((*vertex)->x() - xMin);
        (*vertex)->h((*vertex)->y() - yMin);
    }
}

void Surface::BoundingBox::reset(){
    glm::vec3 center = getCenter();
    xMin -= center.x;
    xMax -= center.x;
    yMin -= center.y;
    yMax -= center.y;
    zMin -= center.z;
    zMax -= center.z;
}

void Surface::BoundingBox::init(){
    xMin = numeric_limits<double>::max();
    yMin = numeric_limits<double>::max();
    zMin = numeric_limits<double>::max();
    xMax = numeric_limits<double>::min();
    yMax = numeric_limits<double>::min();
    zMax = numeric_limits<double>::min();
}

void Surface::BoundingBox::setMinimal(const glm::vec3 &vertex){
    if(xMin > vertex.x){
        xMin = vertex.x;
    }
    if(yMin > vertex.y){
        yMin = vertex.y;
    }
    if(zMin > vertex.z){
        zMin = vertex.z;
    }
}

void Surface::BoundingBox::setMaximal(const glm::vec3 &vertex){
    if(xMax < vertex.x){
        xMax = vertex.x;
    }
    if(yMax < vertex.y){
        yMax = vertex.y;
    }
    if(zMax < vertex.z){
        zMax = vertex.z;
    }
}
