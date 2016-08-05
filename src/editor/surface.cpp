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
      scale(nullptr), rotation(nullptr),translate(nullptr),
      renderingVerticies(nullptr), renderingIndicies(nullptr),
      connectiveVertices(nullptr), connectiveIndices(nullptr){

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

    if(renderingVerticies != nullptr){
        deleteVerticies(renderingVerticies);
    }
    if(renderingIndicies != nullptr){
        renderingIndicies->clear();
        delete renderingIndicies;
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

glm::mat4 Surface::getRenderingTransform() const{
    glm::mat4 matrix;
    if(parent != nullptr){
        matrix *= parent->getRenderingTransform();
    }
    matrix *= getTransformFromParent();
    matrix *= getSurfaceTransform();    
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

void Surface::setRotation(const glm::mat4 &rotation){
    if(this->rotation == nullptr){
        this->rotation = new glm::mat4(rotation);
    } else {
        *(this->rotation) = rotation;
    }
}

void Surface::setTranslate(const glm::vec3 &translate){
    if(this->translate == nullptr){
        this->translate = new glm::vec3(translate);
    } else {
        *(this->translate) = translate;
    }
    updateConnectionMesh();
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

bool Surface::isConnectiveSurface() const{
    return getSurfaceTransform() == glm::mat4(1.0f);
}

void Surface::setSurfaceMaterial(const string &id){
    this->surfaceMaterial = id;
}

std::string Surface::getSurfaceMaterial() const{
    return surfaceMaterial;
}

void Surface::setConnectiveMaterial(const string &id){
    this->connectiveMaterial = id;
}

std::string Surface::getConnectiveMaterial() const{
    return connectiveMaterial;
}

GLfloat Surface::getRoughArea(){
    if(localVerticies == nullptr || localVerticies->size() == 0){
        return 0.0f;
    }

    float ret = 0.0;
    for(int index = 0; index < (localVerticies->size() -1); ++ index){
        Surface::Vertex* v1 = localVerticies->operator[](index);
        Surface::Vertex* v2 = localVerticies->operator[](index + 1);
        ret += ((v1->x() * v2->x()) - (v1->y() * v2->y()));
    }
    Surface::Vertex* v1 = localVerticies->last();
    Surface::Vertex* v2 = localVerticies->first();
    ret += ((v1->x() * v2->x()) - (v1->y() * v2->y()));
    return ret;
}

GLfloat Surface::getPreciseArea(){
    float ret = getRoughArea();
    for(QVector<Surface*>::iterator subSurface = subSurfaces->begin();
        subSurface != subSurfaces->end(); ++subSurface){
        ret -= (*subSurface)->getRoughArea();
    }
    return ret;
}

const QVector<Surface::Vertex*>* Surface::getRenderingVertices(){
    return renderingVerticies;
}

const QVector<Surface::Vertex*>* Surface::getConnectiveVerticies(){
    return connectiveVertices;
}

const QVector<GLushort>* Surface::getRenderingIndices(){
    return renderingIndicies;
}

const QVector<GLushort>* Surface::getConnectiveIndicies(){
    return connectiveIndices;
}

void Surface::updateSurfaceMesh(){
    Surface::tesselate(this);
}

void Surface::updateConnectionMesh(){
    if(isConnectiveSurface()){
        if(connectiveVertices != nullptr){
            deleteVerticies(connectiveVertices);
        }
        connectiveVertices = new QVector<Surface::Vertex*>;

        if(connectiveIndices != nullptr){
            connectiveIndices->clear();
        } else {
            connectiveIndices = new QVector<GLushort>;
        }

        QVector<Surface::Vertex*> base;
        QVector<Surface::Vertex*> derived;

        getVerticesOnParent(base);
        getVerticesToParent(derived);

        Surface::Vertex* baseVertex = new Surface::Vertex(*base[0]);
        Surface::Vertex* derivedVertex = new Surface::Vertex(*derived[0]);

        connectiveVertices->push_back(baseVertex);
        connectiveVertices->push_back(derivedVertex);

        for(int index = 1; index != base.size(); ++index){
            Surface::Vertex* vb = new Surface::Vertex(*base[index]);
            Surface::Vertex* vd = new Surface::Vertex(*derived[index]);

            vb->w(baseVertex->distance(*vb) + baseVertex->w());
            vb->h(0.0);
            vd->w(derivedVertex->distance(*vd) + derivedVertex->h());
            vd->h(vd->distance(*vb));

            connectiveVertices->push_back(vb);
            connectiveVertices->push_back(vd);

            baseVertex = vb;
            derivedVertex = vd;
        }

        int modeBase = connectiveVertices->size();

        for(int index = 0; index < modeBase-2; index +=2){
            connectiveIndices->push_back(index);
            connectiveIndices->push_back((index + 3) % modeBase);
            connectiveIndices->push_back((index + 2) % modeBase);
            connectiveIndices->push_back(index);
            connectiveIndices->push_back((index + 1) % modeBase);
            connectiveIndices->push_back((index + 3) % modeBase);
        }

        Surface::Vertex* endBaseVertex = new Surface::Vertex(*(connectiveVertices->operator[](0)));
        Surface::Vertex* endDerivedVertex =  new Surface::Vertex(*(connectiveVertices->operator[](1)));

        endBaseVertex->w(baseVertex->distance(*endBaseVertex) + baseVertex->w());
        endBaseVertex->h(0.0);
        endDerivedVertex->w(derivedVertex->distance(*endDerivedVertex) + derivedVertex->w());
        endDerivedVertex->h(endDerivedVertex->distance(*endBaseVertex));

        int index = connectiveVertices->size();

        connectiveIndices->push_back(index - 2);
        connectiveIndices->push_back(index + 1);
        connectiveIndices->push_back(index);
        connectiveIndices->push_back(index - 2);
        connectiveIndices->push_back(index - 1);
        connectiveIndices->push_back(index + 1);

        if(CONN_DEBUG){
            Surface::vertexLogger(*connectiveVertices, *connectiveIndices,
                                  "connective surface in local coordinate system");
        }

        glm::mat4 transform = parent == nullptr ? getTransformFromParent() * getSurfaceTransform() :
                 (parent->getRenderingTransform() * getTransformFromParent() * getSurfaceTransform());
        for(QVector<Surface::Vertex*>::iterator vertex = connectiveVertices->begin();
            vertex != connectiveVertices->end(); ++vertex){
            Surface::transformVertex(transform, **vertex);
        }

        if(CONN_DEBUG){
            Surface::vertexLogger(*connectiveVertices, *connectiveIndices,
                                  "connective surface in world coordinate system");
        }

    } else {
        if(CONN_DEBUG){
            cout<<"no connective surface for Surface ("<<this<<")"<<endl;
        }
    }
}

bool Surface::tesselate(Surface *surface){
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
        gluTessCallback(tess, GLU_TESS_COMBINE,
                        (void (__stdcall*)(void)) tessCombine);
    }
    if(targetSurface != nullptr){
        if(TESS_DEBUG){
            cout<<"Tessellation begin with target Surface is not null"<<endl;
        }
        return false;
    }

    targetSurface = surface;
    if(targetSurface->renderingVerticies != nullptr){
        for(QVector<Surface::Vertex*>::iterator renderVertex = targetSurface->renderingVerticies->begin();
            renderVertex != targetSurface->renderingVerticies->end(); ++renderVertex){
            delete (*renderVertex);
        }
        targetSurface->renderingVerticies->clear();
        delete targetSurface->renderingVerticies;
    }
    targetSurface->renderingVerticies = new QVector<Surface::Vertex*>;

    if(targetSurface->renderingIndicies != nullptr){
        targetSurface->renderingIndicies->clear();
    } else {
        targetSurface->renderingIndicies = new QVector<GLushort>;
    }

    QVector<Surface::Vertex*> *verticies  = new QVector<Surface::Vertex*>;

    gluTessBeginPolygon(tess, 0);
    if(TESS_DEBUG){
        cout<<"gluTessBeginPolygon(tess, 0);"<<endl;
    }
    {
        gluTessBeginContour(tess);
        if(TESS_DEBUG){
            cout<<"\tgluTessBeginContour(tess);"<<endl;
        }

        targetSurface->getLocalVertices(*verticies);
        for(QVector<Surface::Vertex*>::iterator vertex = verticies->begin();
            vertex != verticies->end(); ++vertex){
            Surface::Vertex* vertexData = new Surface::Vertex(**vertex);
            GLdouble *vertexDataElement = vertexData->getData();
            gluTessVertex(tess, vertexDataElement, vertexDataElement);
            if(TESS_DEBUG){
                cout<<"\t\tgluTessVertex(tess, vertexDataElement, vertexDataElement);";
                cout<<" ("<<vertexData->x()<<","<<vertexData->y()<<","<<vertexData->z()<<")"<<endl;
            }
        }

        gluTessEndContour(tess);
        if(TESS_DEBUG){
            cout<<"gluTessEndContour(tess);"<<endl;
        }

        for(QVector<Surface*>::iterator subSurface = targetSurface->subSurfaces->begin();
            subSurface != targetSurface->subSurfaces->end(); ++subSurface){
            verticies->clear();
            (*subSurface)->getVerticesOnParent(*verticies);

            gluTessBeginContour(tess);
            if(TESS_DEBUG){
                cout<<"gluTessBeginContour(tess);"<<endl;
            }

            for(QVector<Surface::Vertex*>::iterator vertex = verticies->begin();
                vertex != verticies->end(); ++vertex){
                Surface::Vertex* vertexData = new Surface::Vertex(**vertex);
                targetSurface->boundingBox->generateTexture(*vertexData);
                GLdouble* vertexDataElement = vertexData->getData();
                gluTessVertex(tess, vertexDataElement, vertexDataElement);
                if(TESS_DEBUG){
                    cout<<"\t\tgluTessVertex(tess, vertexDataElement, vertexDataElement);";
                    cout<<" ("<<vertexData->x()<<","<<vertexData->y()<<","<<vertexData->z()<<")"<<endl;
                }
            }

            gluTessEndContour(tess);
            if(TESS_DEBUG){
                cout<<"gluTessEndContour(tess);"<<endl;
            }
        }
    }
    gluTessEndPolygon(tess);
    if(TESS_DEBUG){
        cout<<"gluTessEndPolygon(tess);"<<endl;
    }
    targetSurface = nullptr;
    tess_locker.unlock();
}

void Surface::tessCombine(GLdouble coords[], GLdouble *vertex_data[],
                          GLfloat weight[], GLdouble **dataOut){
    GLdouble *vertex;
    int i;
    vertex = (GLdouble *) malloc(6 * sizeof(GLdouble));
    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];
    for (i = 3; i < 5; i++) {
        vertex[i] =  weight[0] * vertex_data[0][i];
        if  (weight[1] > 0)
            vertex[i] += weight[1] * vertex_data[1][i];
        if  (weight[2] > 0)
            vertex[i] += weight[2] * vertex_data[2][i];
        if  (weight[3] > 0)
            vertex[i] += weight[3] * vertex_data[3][i];
    }
    *dataOut = vertex;
}

void Surface::tessBegin(GLenum type){
    if(TESS_DEBUG){
        cout<<"tessellation process has began for Surface : ("
            <<targetSurface<<")"<<endl;
    }
    return;
}

void Surface::tessVertex(const GLvoid* data){
    const GLdouble* vertexData = (const GLdouble*) data;
    Surface::Vertex* callbackVertex = new Surface::Vertex(
                vertexData[0], vertexData[1], vertexData[2], vertexData[3], vertexData[4]);
    bool found = false;
    int index = 0;

    for(QVector<Surface::Vertex*>::iterator renderingVertex = targetSurface->renderingVerticies->begin();
        renderingVertex != targetSurface->renderingVerticies->end(); ++renderingVertex){
        if(callbackVertex->equals(**renderingVertex)){
            targetSurface->renderingIndicies->push_back(index);
            found = true;
            return;
        }
        index++;
    }

    if(!found){
        targetSurface->renderingVerticies->push_back(callbackVertex);
        targetSurface->renderingIndicies->push_back(targetSurface->renderingIndicies->size() - 1);
    }
}

void Surface::tessEnd(){   
    if(TESS_DEBUG){
        vertexLogger(*(targetSurface->renderingVerticies),
                     *(targetSurface->renderingIndicies),
                     "tessellated result in local coordinate system ");
    }
    glm::mat4 transform = targetSurface->getRenderingTransform();
    for(QVector<Surface::Vertex*>::iterator vertex = targetSurface->renderingVerticies->begin();
        vertex != targetSurface->renderingVerticies->end(); ++vertex){
        Surface::transformVertex(transform, **vertex);
    }
    if(TESS_DEBUG){
        vertexLogger(*(targetSurface->renderingVerticies),
                     *(targetSurface->renderingIndicies),
                     "tessellated result in world coordinate system ");
    }
}

void Surface::tessEdge(){return;}

void Surface::tessError(GLenum type){
    cout<<"Tessellation Error has been called"<<endl;
    const GLubyte *errStr;
    errStr = gluErrorString(type);
    cout<<"[ERROR] : "<<errStr<<endl;
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

void Surface::vertexLogger(const QVector<Surface::Vertex *> &verticies,
                           const QVector<GLushort> &indicies,
                           const string title){
    cout<<title<<" : "<<endl;
    cout<<"{"<<endl;
    for(QVector<Surface::Vertex*>::const_iterator vertex = verticies.begin();
        vertex != verticies.end(); ++vertex){
        cout<<"{"<<(*vertex)->x()<<", "<<(*vertex)->y()<<", "<<(*vertex)->z()<<", "
           <<(*vertex)->w()<<", "<<(*vertex)->h()<<"},"<<endl;
    }
    cout<<"}"<<endl;

    cout<<"indices = "<<endl<<"{"<<endl;
    int cnt = 0;
    for(QVector<GLushort>::const_iterator index = indicies.begin();
        index != indicies.end(); ++index){
        cout<<*index<<",";
        if(cnt++ % 3 == 0){
            cout<<endl;
        }
    }
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

void Surface::BoundingBox::generateTexture(Surface::Vertex &vertex) const{
    vertex.w(vertex.x() - xMin);
    vertex.h(vertex.y() - yMin);
}

void Surface::BoundingBox::generateTexture(const QVector<Surface::Vertex*> &verticies) const{
    for(QVector<Surface::Vertex*>::const_iterator vertex = verticies.begin();
        vertex != verticies.end(); ++vertex){
        generateTexture(**vertex);
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
