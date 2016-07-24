#include "editor/klm_surface.h"
#include <QMutex>
#include <iostream>

using namespace klm;
using namespace glm;
using namespace std;

static QMutex tess_mutex;

Surface::Surface(const vector<vec3> &points) throw(SurfaceException) :
    visible(true), parent(nullptr), subSurfaces(new vector<Surface*>),
    renderVertices(nullptr), renderIndicies(nullptr){
    if(points.size() < 3){
        throw SurfaceException("can not create Surface using less then three points");
    }
    vec3 planNormal = GLUtility::getPlaneNormal(points);
    if(planNormal == GLUtility::NON_NORMAL){
        throw SurfaceException("can not create Surface with points not in same plane");
    }

    localVertices = new vector<Vertex*>();
    verticesToParent = new vector<Vertex*>();

    collisionTester = new Polygon();

    boundingBox = new BoundingBox(points);    
    vec3 center = boundingBox->getCenter();
    mat4 rotation;
    GLUtility::getRotation(planNormal, GLUtility::Z_AXIS, rotation);

    for(vector<vec3>::const_iterator point = points.begin();
        point != points.end(); ++point){

        Vertex* vertex = new Vertex(*point);
        verticesToParent->push_back(vertex);

        vertex = new Vertex(*point);
        vertex->setX(vertex->getX() - center.x);
        vertex->setY(vertex->getY() - center.y);
        vertex->setZ(vertex->getZ() - center.z);

        GLUtility::positionTransform(*vertex, rotation);
        localVertices->push_back(vertex);

        collisionTester->outer().push_back(Point((*point).x, (*point).y));
    }

    delete boundingBox;

    boundingBox = new BoundingBox(*localVertices);
    boundingBox->genTexture(*localVertices);   


    rotation = glm::inverse(rotation);
    transFromParent = new mat4(glm::translate(center) * rotation);
    updateVertices();
}

Surface::~Surface(){
    if(transFromParent != nullptr){
        delete transFromParent;
    }

    deleteVertices(localVertices);
    deleteVertices(verticesToParent);
}

void Surface::getSurfaceVertices(vector<Vertex*> &localVertices) const{
    for(vector<Vertex*>::iterator vertex = this->localVertices->begin();
        vertex != this->localVertices->end(); ++vertex){
        localVertices.push_back(new Vertex(**vertex));
    }
}

void Surface::getVerticiesToParent(vector<Vertex*> &vertices) const{
    for(vector<Vertex*>::iterator vertex = verticesToParent->begin();
        vertex != verticesToParent->end(); ++vertex){
        Vertex* v = new Vertex(**vertex);
        vertices.push_back(v);
    }
}

void Surface::getVerticiesOnParent(vector<Vertex *> &vertices) const{
    for(vector<Vertex*>::iterator vertex = verticesToParent->begin();
        vertex != verticesToParent->end(); ++vertex){
        Vertex* v = new Vertex(**vertex);
        v->setZ(0.0f);
        vertices.push_back(v);
    }
}

void Surface::getTransFromParent(mat4 &transform) const{
    if(parent == nullptr){
        transform = *(this->transFromParent);
    } else {
        mat4 inheritedTransform;
        parent->getTransFromParent(inheritedTransform);
        transform = inheritedTransform * *(this->transFromParent);
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
    for(vector<Vertex*>::iterator vertex = renderVertices->begin();
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

    if(index != nullptr){
        delete indecies;
    }

    len = renderIndicies->size();
    indecies = new GLushort[len];
    int i = 0;

    for(vector<GLushort>::iterator index = renderIndicies->begin();
        index != renderIndicies->end(); ++index){
        indecies[i++] = (*index);
    }
    return true;
}

bool Surface::addSubSurface(const vector<vec3> &points){
    Surface* newSubSurface;
    try{
        newSubSurface = new Surface(points);
    } catch (SurfaceException &ex){
        delete newSubSurface;
        return false;
    }

    vector<Surface*>::iterator subSurface = subSurfaces->begin();
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
    }

    targetSurface = surface;
    //clean up possible remaining rendering
    //data from last time
    deleteVertices(targetSurface->renderVertices);
    targetSurface->renderVertices = nullptr;
    targetSurface->renderVertices = new vector<Vertex*>;

    if(targetSurface->renderIndicies != nullptr){
        targetSurface->renderIndicies->clear();
    } else {
        targetSurface->renderIndicies = new vector<GLushort>;
    }

    gluTessBeginPolygon(tess, 0);
    {
        gluTessBeginContour(tess);
        cout<<"tesselating surface :"<<targetSurface<<endl;
        for(vector<Vertex*>::iterator vertex = targetSurface->localVertices->begin();
            vertex != targetSurface->localVertices->end(); ++vertex){
            Vertex *newVertex = new Vertex(**vertex);
            GLdouble *vertexData = newVertex->getData();
            gluTessVertex(tess, vertexData, vertexData);
            targetSurface->renderVertices->push_back(newVertex);
        }
        gluTessEndContour(tess);

        vector<Vertex*> surfaceCoords;
        for(vector<Surface*>::iterator subSurface = targetSurface->subSurfaces->begin();
            subSurface != targetSurface->subSurfaces->end(); ++subSurface){
            (*subSurface)->getVerticiesOnParent(surfaceCoords);
            gluTessBeginContour(tess);

            for(vector<Vertex*>::iterator vertex = surfaceCoords.begin();
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
//    for(vector<Vertex*>::iterator it = targetSurface->renderVertices->begin();
//        it != targetSurface->renderVertices->end(); ++it){
//        GLdouble* vd = (*it)->getData();
//        cout<<"tessVertex : ("<<vd[0]<<","<<vd[1]<<","<<vd[2]<<","<<vd[3]<<","<<vd[4]<<")"<<endl;
//    }
//    cout<<"indicies : (";
//    for(vector<GLushort>::iterator it = targetSurface->renderIndicies->begin();
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
    for(vector<Vertex*>::iterator vertex = targetSurface->renderVertices->begin();
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

void Surface::deleteVertices(vector<Vertex*> *vertices) {
    if(vertices != nullptr){        
        for(vector<Vertex*>::iterator vertex = vertices->begin();
            vertex != vertices->end(); ++vertex){
            delete *vertex;
        }

        vertices->empty();
        delete vertices;
    }    
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
