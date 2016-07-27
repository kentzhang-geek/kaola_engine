#include "editor/klm_surface.h"
#include <QMutex>
#include <iostream>

using namespace klm;
using namespace std;

static QMutex tess_mutex;

Surface::Surface(const QVector<glm::vec3> &points) throw(SurfaceException) :
    visible(true), parent(nullptr), subSurfaces(new QVector<Surface*>),
    renderVertices(nullptr), renderIndicies(nullptr),
    translate(nullptr){
    if(points.size() < 3){
        throw SurfaceException("can not create Surface using less then three points");
    }
    glm::vec3 planNormal = GLUtility::getPlaneNormal(points);
    if(planNormal == GLUtility::NON_NORMAL){
        throw SurfaceException("can not create Surface with points not in same plane");
    }

    localVertices = new QVector<Vertex*>();
    verticesToParent = new QVector<Vertex*>();

    collisionTester = new bg_Polygon();

    boundingBox = new BoundingBox(points);    
    glm::vec3 center = boundingBox->getCenter();
    glm::mat4 rotation;
    GLUtility::getRotation(planNormal, GLUtility::Z_AXIS, rotation);

    for(QVector<glm::vec3>::const_iterator point = points.begin();
        point != points.end(); ++point){

        Vertex* vertex = new Vertex(*point);
        verticesToParent->push_back(vertex);

        vertex = new Vertex(*point);
        vertex->setX(vertex->getX() - center.x);
        vertex->setY(vertex->getY() - center.y);
        vertex->setZ(vertex->getZ() - center.z);

        GLUtility::positionTransform(*vertex, rotation);

        localVertices->push_back(vertex);        
        collisionTester->outer().push_back(bg_Point((*point).x, (*point).y));
    }    

    delete boundingBox;

    boundingBox = new BoundingBox(*localVertices);
    boundingBox->genTexture(*localVertices);   


    rotation = glm::inverse(rotation);
    transFromParent = new glm::mat4(glm::translate(center) * rotation);
    updateVertices();
}

Surface::~Surface(){
    if(transFromParent != nullptr){
        delete transFromParent;
    }

    deleteVertices(localVertices);
    deleteVertices(verticesToParent);
}

void Surface::getSurfaceVertices(QVector<Vertex*> &localVertices) const{
    for(QVector<Vertex*>::iterator vertex = this->localVertices->begin();
        vertex != this->localVertices->end(); ++vertex){
        localVertices.push_back(new Vertex(**vertex));
    }
}

void Surface::getVerticiesToParent(QVector<Vertex*> &vertices) const{
    for(QVector<Vertex*>::iterator vertex = verticesToParent->begin();
        vertex != verticesToParent->end(); ++vertex){
        Vertex* v = new Vertex(**vertex);
        vertices.push_back(v);
    }
}

void Surface::getVerticiesOnParent(QVector<Vertex *> &vertices) const{
    for(QVector<Vertex*>::iterator vertex = verticesToParent->begin();
        vertex != verticesToParent->end(); ++vertex){
        Vertex* v = new Vertex(**vertex);
        if(translate ==nullptr){
            v->setZ(0.0f);
        } else {
            v->setX(v->getX() - translate->x);
            v->setY(v->getY() - translate->y);
            v->setZ(v->getZ() - translate->z);
        }
        vertices.push_back(v);
    }
}

void Surface::getTransFromParent(glm::mat4 &transform) const{
    if(parent == nullptr){
        transform = *(this->transFromParent);
    } else {
        glm::mat4 inheritedTransform;
        parent->getTransFromParent(inheritedTransform);
        transform = *(this->transFromParent) * inheritedTransform;
    }
}

/**
 * TO DO: implement connective surface here
 * @brief Surface::updateVertices
 */
void Surface::updateVertices(){
    Surface::updateRenderingData(this);
}

Surface& Surface::getParent() const{    
    return *parent;
}

bool Surface::getRenderingVertices(GLfloat *&data, int &len) const{
    if(renderVertices == nullptr || renderVertices->size() == 0){
        return false;
    }

    if(data != nullptr){
        delete data;
    }

    len = renderVertices->size() * Vertex::VERTEX_SIZE;
    data = new GLfloat[len];

    int index = 0;
    for(QVector<Vertex*>::iterator vertex = renderVertices->begin();
        vertex != renderVertices->end(); ++vertex){
        const GLdouble* vertexData = (*vertex)->getData();
        data[index++] = (GLfloat)vertexData[Vertex::X];
        data[index++] = (GLfloat)vertexData[Vertex::Y];
        data[index++] = (GLfloat)vertexData[Vertex::Z];
        data[index++] = (GLfloat)vertexData[Vertex::W];
        data[index++] = (GLfloat)vertexData[Vertex::H];
    }
    return true;
}

bool Surface::getRenderingIndicies(GLushort *&indecies, int &len) const{
    if(renderIndicies == nullptr || renderIndicies->size() == 0){
        return false;
    }

    if(indecies != nullptr){
        delete indecies;
    }

    len = renderIndicies->size();
    indecies = new GLushort[len];
    int i = 0;

    for(QVector<GLushort>::iterator index = renderIndicies->begin();
        index != renderIndicies->end(); ++index){
        indecies[i++] = (*index);
    }
    return true;
}

bool Surface::addSubSurface(const QVector<glm::vec3> &points){
    Surface* newSubSurface;
    try{
        newSubSurface = new Surface(points);
    } catch (SurfaceException &ex){
        delete newSubSurface;
        return false;
    }

    QVector<Surface*>::iterator subSurface = subSurfaces->begin();
    bool collisionFound = false;
    while(subSurface != subSurfaces->end() && !collisionFound){
        collisionFound = bg::intersects(
                    *collisionTester,
                    *((*subSurface)->collisionTester));
        subSurface++;
    }

    if(collisionFound){
        delete newSubSurface;
        return false;
    } else {
        newSubSurface->parent = this;
        subSurfaces->push_back(newSubSurface);
        updateVertices();
        return true;
    }

}

bool Surface::isRoot() const{
    return parent == nullptr;
}

int Surface::getSubSurfaceCnt() const{
    return subSurfaces->size();
}

Surface* Surface::getSubSurface(const int index) const{
    if(index < 0 || index >= subSurfaces->size()){
        return nullptr;
    } else {
        return subSurfaces->at(index);
    }
}

void Surface::setHeightToParent(const GLfloat &height){
    if(translate != nullptr){
        delete translate;
    }
    translate = new glm::vec3(0.0f, 0.0f, height);
}

GLfloat Surface::getHeightToParent() const{
    return translate == nullptr ? 0.0f :
                                  translate->z;
}

bool Surface::isConnectedToParent() const{
    return translate != nullptr;
}

bool Surface::getConnectiveVerticies(QVector<Vertex *> &connectiveVertices) const{
    if(!isConnectedToParent()){
        for(QVector<Vertex*>::iterator vertex = this->connectiveVerticies->begin();
            vertex != this->connectiveVerticies->end(); ++vertex){
            connectiveVertices.push_back(new Vertex(**vertex));
        }
        return true;
    }

    return false;
}


void Surface::updateConnectivedData(){
    if(connectiveIndicies != nullptr){
        connectiveIndicies->clear();
        delete connectiveIndicies;
    }
    connectiveIndicies = new QVector<GLushort>();

    if(connectiveVerticies != nullptr){
        for(QVector<Vertex*>::iterator vertex = connectiveVerticies->begin();
            vertex != connectiveVerticies->end(); ++vertex){
            delete *vertex;
        }
        connectiveVerticies->clear();
        delete connectiveVerticies;
    }
    connectiveVerticies = new QVector<Vertex*>();

    QVector<Vertex*> base;
    QVector<Vertex*> derived;
    getVerticiesToParent(base);
    getVerticiesOnParent(derived);

    int modeBase = base.size();

    Vertex* baseVertex = new Vertex(*base[0]);
    Vertex* derivedVertex = new Vertex(*derived[0]);

    baseVertex->setW(0.0);
    baseVertex->setH(0.0);
    derivedVertex->setW(0.0);
    derivedVertex->setH(derivedVertex->distance(*baseVertex));

    connectiveVerticies->push_back(baseVertex);
    connectiveVerticies->push_back(derivedVertex);

    for(int index = 1; index != base.size(); ++index){
        Vertex* v1 = new Vertex(*base[index]);
        Vertex* v2 = new Vertex(*derived[index]);

        v1->setW(baseVertex->distance(*v1));
        v1->setH(0.0);
        v2->setW(derivedVertex->distance(*v2));
        v2->setH(v2->distance(*v1));

        connectiveVerticies->push_back(v1);
        connectiveVerticies->push_back(v2);

        baseVertex = v1;
        derivedVertex = v2;
    }

    for(int index = 1; index < base.size(); index+=2){
        connectiveIndicies->push_back(index);
        connectiveIndicies->push_back((index + 3) % modeBase);
        connectiveIndicies->push_back((index + 2) % modeBase);
        connectiveIndicies->push_back(index);
        connectiveIndicies->push_back((index + 1) % modeBase);
        connectiveIndicies->push_back((index + 3) % modeBase);
    }

}

void Surface::updateRenderingData(Surface *surface){
    tess_mutex.lock();

    if(tess == nullptr){
        tess = gluNewTess();

        gluTessCallback(tess, GLU_TESS_BEGIN,
                        (void (__stdcall*)(void)) tessBegin);
        gluTessCallback(tess, GLU_TESS_END,
                        (void (__stdcall*)(void)) tessEnd);
        gluTessCallback(tess, GLU_TESS_VERTEX,
                        (void (__stdcall*)())     tessVertex);
        gluTessCallback(tess, GLU_TESS_EDGE_FLAG_DATA,
                        (void (__stdcall*)(void)) tessEdge);
        gluTessCallback(tess, GLU_TESS_ERROR,
                        (void (__stdcall*)(void)) tessError);
    }

    targetSurface = surface;
    //clean up possible remaining rendering
    //data from last time
    deleteVertices(targetSurface->renderVertices);
    targetSurface->renderVertices = nullptr;
    targetSurface->renderVertices = new QVector<Vertex*>;

    if(targetSurface->renderIndicies != nullptr){
        targetSurface->renderIndicies->clear();
    } else {
        targetSurface->renderIndicies = new QVector<GLushort>;
    }

    gluTessBeginPolygon(tess, 0);
    {
        gluTessBeginContour(tess);
        cout<<"tesselating surface :"<<targetSurface<<endl;
        for(QVector<Vertex*>::iterator vertex = targetSurface->localVertices->begin();
            vertex != targetSurface->localVertices->end(); ++vertex){
            Vertex *newVertex = new Vertex(**vertex);
            GLdouble *vertexData = newVertex->getData();
            gluTessVertex(tess, vertexData, vertexData);
            targetSurface->renderVertices->push_back(newVertex);
        }
        gluTessEndContour(tess);

        QVector<Vertex*> surfaceCoords;
        for(QVector<Surface*>::iterator subSurface = targetSurface->subSurfaces->begin();
            subSurface != targetSurface->subSurfaces->end(); ++subSurface){
            (*subSurface)->getVerticiesOnParent(surfaceCoords);
            gluTessBeginContour(tess);

            for(QVector<Vertex*>::iterator vertex = surfaceCoords.begin();
                vertex != surfaceCoords.end(); ++vertex){
                Vertex *newVertex = new Vertex(**vertex);
                targetSurface->boundingBox->genTexture(*newVertex);
                GLdouble* vertexData = newVertex->getData();
                gluTessVertex(tess, vertexData, vertexData);
                targetSurface->renderVertices->push_back(newVertex);
            }
            gluTessEndContour(tess);
        }

    }
    gluTessEndPolygon(tess);

    tess_mutex.unlock();
}

void Surface::tessBegin(GLenum type){return;}
void Surface::tessEnd(){
//    cout<<"Surface : "<<targetSurface<<" tesselation finished;"<<endl;
//    for(QVector<Vertex*>::iterator it = targetSurface->renderVertices->begin();
//        it != targetSurface->renderVertices->end(); ++it){
//        GLdouble* vd = (*it)->getData();
//        cout<<"tessVertex : ("<<vd[0]<<","<<vd[1]<<","<<vd[2]<<","<<vd[3]<<","<<vd[4]<<")"<<endl;
//    }
//    cout<<"indicies : (";
//    for(QVector<GLushort>::iterator it = targetSurface->renderIndicies->begin();
//        it != targetSurface->renderIndicies->end(); ++it){
//        cout<<*it<<",";
//    }
//    cout<<")"<<endl<<endl;
    return;
}
void Surface::tessEdge(){return;}

void Surface::tessVertex(const GLvoid *data){
    const GLdouble *vertexData = (const GLdouble*) data;
    int index = 0;
    for(QVector<Vertex*>::iterator vertex = targetSurface->renderVertices->begin();
        vertex != targetSurface->renderVertices->end(); ++vertex){
        if((*vertex)->getData() == vertexData){
            GLdouble* vd = (*vertex)->getData();
            targetSurface->renderIndicies->push_back(index);
            return;
        }
        index++;
    }
    cout<<"vertex is not found in targetSurface->renderVertices()"<<endl;
}

void Surface::deleteVertices(QVector<Vertex*> *vertices) {
    if(vertices != nullptr){        
        for(QVector<Vertex*>::iterator vertex = vertices->begin();
            vertex != vertices->end(); ++vertex){
            delete *vertex;
        }

        vertices->empty();
        delete vertices;
    }    
}

void Surface::tessError(GLenum type){
    cout<<"Tessellation Error has been called"<<endl;
    const GLubyte *errStr;
    errStr = gluErrorString(type);
    cout<<"[ERROR] : "<<errStr<<endl;
}

GLUtesselator* Surface::tess = nullptr;
Surface* Surface::targetSurface = nullptr;

void Surface::deleteTessellator(){
    gluDeleteTess(tess);
}

SurfaceException::SurfaceException(const string &message) : message(message){}

string SurfaceException::what() const{
    return message;
}
