#include "editor/klm_surface.h"
#include <QMutex>
#include <iostream>

using namespace klm;
using namespace std;

static QMutex tess_mutex;

Surface::Surface(const QVector<glm::vec3> &points) throw(SurfaceException) :
    visible(true), parent(nullptr), subSurfaces(new QVector<Surface*>),
    renderVertices(nullptr), renderIndicies(nullptr),
    translateFromParent(nullptr), debug(false){
    if(points.size() < 3){
        throw SurfaceException("can not create Surface using less then three points");
    }
    glm::vec3 planNormal = GLUtility::getPlaneNormal(points);
    if(planNormal == GLUtility::NON_NORMAL){
        throw SurfaceException("can not create Surface with points not in same plane");
    }

    localVertices = new QVector<Vertex*>();
//    verticesToParent = new QVector<Vertex*>();

    collisionTester = new bg_Polygon();

    boundingBox = new BoundingBox(points);    
    glm::vec3 center = boundingBox->getCenter();
    glm::mat4 rotation;
    GLUtility::getRotation(planNormal, GLUtility::Z_AXIS, rotation);    

    for(QVector<glm::vec3>::const_iterator point = points.begin();
        point != points.end(); ++point){

        Vertex* vertex = new Vertex(*point);
//        verticesToParent->push_back(vertex);

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
    if(translateFromParent != nullptr){
        delete translateFromParent;
    }
    deleteVertices(localVertices);
//    deleteVertices(verticesToParent);
}

void Surface::getSurfaceVertices(QVector<Vertex*> &localVertices) const{
    for(QVector<Vertex*>::iterator vertex = this->localVertices->begin();
        vertex != this->localVertices->end(); ++vertex){
        localVertices.push_back(new Vertex(**vertex));
    }
}

void Surface::getVerticiesToParent(QVector<Vertex*> &vertices) const{
    glm::mat4* overAllTransform;
    if(translateFromParent == nullptr){
        overAllTransform = new glm::mat4(*transFromParent);
    } else {
        overAllTransform = new glm::mat4(glm::translate(*translateFromParent) *
                                         (*transFromParent));
    }
    for(QVector<Vertex*>::iterator vertex = localVertices->begin();
        vertex != localVertices->end(); ++vertex){
        Vertex* transformedVertex = new Vertex(**vertex);
        GLUtility::positionTransform(*transformedVertex, *overAllTransform);
        vertices.push_back(transformedVertex);
    }
    delete overAllTransform;
//    for(QVector<Vertex*>::iterator vertex = verticesToParent->begin();
//        vertex != verticesToParent->end(); ++vertex){
//        Vertex* v = new Vertex(**vertex);
//        vertices.push_back(v);
//    }
}

void Surface::getVerticiesOnParent(QVector<Vertex *> &vertices) const{
    for(QVector<Vertex*>::iterator vertex = localVertices->begin();
        vertex != localVertices->end(); ++vertex){
        Vertex* transformedVertex = new Vertex(**vertex);
        GLUtility::positionTransform(*transformedVertex, *transFromParent);
        vertices.push_back(transformedVertex);
    }
//    for(QVector<Vertex*>::iterator vertex = verticesToParent->begin();
//        vertex != verticesToParent->end(); ++vertex){
//        Vertex* v = new Vertex(**vertex);
//        if(translateFromParent ==nullptr){
//            v->setZ(0.0f);
//        } else {
//            v->setX(v->getX() - translateFromParent->x);
//            v->setY(v->getY() - translateFromParent->y);
//            v->setZ(v->getZ() - translateFromParent->z);
//        }
//        vertices.push_back(v);
//    }
}

void Surface::getTransFromParent(glm::mat4 &transform) const{
    if(parent == nullptr){
        if(translateFromParent != nullptr){
            transform = glm::translate(*translateFromParent) * (*(this->transFromParent));
        } else {
            transform = *(this->transFromParent);
        }
    } else {
        glm::mat4 inheritedTransform;
        parent->getTransFromParent(inheritedTransform);
        if(translateFromParent != nullptr){
            transform = glm::translate(*translateFromParent) * (*(this->transFromParent)) * inheritedTransform;
        } else {
            transform = (*(this->transFromParent)) * inheritedTransform;
        }
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
        return false;
    }

    QVector<Surface*>::iterator subSurface = subSurfaces->begin();
    bool collisionFound = false;
    while(subSurface != subSurfaces->end() && !collisionFound){
        collisionFound = bg::intersects(
                    *(newSubSurface->collisionTester),
                    *((*subSurface)->collisionTester));
        if(collisionFound){
            cout<<"the following polygons have collison"<<endl;
            bg::wkt<bg_Polygon>(*collisionTester);
            cout<<"and "<<endl;
            bg::wkt<bg_Polygon>(*((*subSurface)->collisionTester));
        }
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
    glm::vec3 translate(0.0f, 0.0f, height);
    setTranslateToParent(translate);
}

void Surface::setTranslateToParent(const glm::vec3 &translate){
    if(translateFromParent != nullptr){
        delete translateFromParent;
    }
    translateFromParent = new glm::vec3(translate);
    if(parent != nullptr){
        Surface::updateRenderingData(this->parent);
    }
}

GLfloat Surface::getHeightToParent() const{
    return translateFromParent == nullptr ? 0.0f :
           translateFromParent->z;
}

bool Surface::isConnectedToParent() const{
    return translateFromParent != nullptr;
}

bool Surface::getConnectiveVerticies(QVector<Vertex *> &connectiveVertices) const{
    if(isConnectedToParent()){
        for(QVector<Vertex*>::iterator vertex = this->connectiveVerticies->begin();
            vertex != this->connectiveVerticies->end(); ++vertex){
            connectiveVertices.push_back(new Vertex(**vertex));
        }
        return true;
    }
    return false;
}

bool Surface::getConnectiveIndicies(QVector<GLushort> &connectiveIndicies) const{
    if(isConnectedToParent()){
        for(QVector<GLushort>::iterator index = this->connectiveIndicies->begin();
            index != this->connectiveIndicies->end(); ++index){
            connectiveIndicies.push_back(*index);
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
        gluTessCallback(tess, GLU_TESS_COMBINE,
                        (void (__stdcall*)(void)) tessCombine);
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
    if(targetSurface->isDebug()){
        cout<<"gluTessBeginPolygon(tess, 0);"<<endl;
    }
    {
        gluTessBeginContour(tess);
        if(targetSurface->isDebug()){
            cout<<"\tgluTessBeginContour(tess);"<<endl;
        }
        for(QVector<Vertex*>::iterator vertex = targetSurface->localVertices->begin();
            vertex != targetSurface->localVertices->end(); ++vertex){
            Vertex *newVertex = new Vertex(**vertex);
            GLdouble *vertexData = newVertex->getData();
            gluTessVertex(tess, vertexData, vertexData);
            if(targetSurface->isDebug()){
                cout<<"\t\tgluTessVertex(tess, vertexData,vertexData);"<<endl;
                cout<<"\t\t{"<<vertexData[0]<<","<<vertexData[1]<<
                     ","<<vertexData[2]<<","<<vertexData[3]<<","<<
                        vertexData[4]<<"}"<<endl;
            }
        }
        gluTessEndContour(tess);
        if(targetSurface->isDebug()){
            cout<<"\tgluTessEndContour(tess);"<<endl;
        }

        QVector<Vertex*> surfaceCoords;
        for(QVector<Surface*>::iterator subSurface = targetSurface->subSurfaces->begin();
            subSurface != targetSurface->subSurfaces->end(); ++subSurface){
            (*subSurface)->getVerticiesOnParent(surfaceCoords);
            gluTessBeginContour(tess);
            if(targetSurface->isDebug()){
                cout<<"\tgluTessBeginContour(tess);"<<endl;
            }

            for(QVector<Vertex*>::iterator vertex = surfaceCoords.begin();
                vertex != surfaceCoords.end(); ++vertex){
                Vertex *newVertex = new Vertex(**vertex);
                targetSurface->boundingBox->genTexture(*newVertex);
                GLdouble* vertexData = newVertex->getData();
                gluTessVertex(tess, vertexData, vertexData);
                if(targetSurface->isDebug()){
                    cout<<"\t\tgluTessVertex(tess, vertexData,vertexData);"<<endl;
                    cout<<"\t\t{"<<vertexData[0]<<","<<vertexData[1]<<
                         ","<<vertexData[2]<<","<<vertexData[3]<<","<<
                            vertexData[4]<<"}"<<endl;
                }
            }
            gluTessEndContour(tess);
            if(targetSurface->isDebug()){
                cout<<"\tgluTessEndContour(tess);"<<endl;
            }

            for(QVector<Vertex*>::iterator surfaceCoord = surfaceCoords.begin();
                surfaceCoord != surfaceCoords.end(); ++surfaceCoord){
                delete *surfaceCoord;
            }
            surfaceCoords.clear();
        }

    }
    gluTessEndPolygon(tess);
    if(targetSurface->isDebug()){
        cout<<"gluTessEndPolygon(tess);"<<endl;
    }

    tess_mutex.unlock();
}

GLushort Surface::addRenderingVertex(const Vertex* vertex){
    if(vertex == nullptr || renderVertices == nullptr){
        return -2;
    }
    GLushort index =0;
    for(QVector<Vertex*>::iterator rVertex = renderVertices->begin();
        rVertex != renderVertices->end(); ++rVertex){
        if(vertex->equals(**rVertex)){
            return index;
        }
        ++index;
    }

    //not found after loop is checked
//    renderVertices->push_back(vertex);
//    renderVertices->push_back(&*vertex);
    return -1;
}

void Surface::setDebug(){
    debug = true;
}

bool Surface::isDebug() const{
    return debug;
}

void Surface::tessBegin(GLenum type){return;}

void Surface::tessEnd(){
    if(targetSurface->isDebug()){
        cout<<"Surface : "<<targetSurface<<" tesselation finished;"<<endl;
        for(QVector<Vertex*>::iterator it = targetSurface->renderVertices->begin();
            it != targetSurface->renderVertices->end(); ++it){
            GLdouble* vd = (*it)->getData();
            cout<<"tessVertex : ("<<vd[0]<<",\t\t"<<vd[1]<<",\t\t"<<vd[2]<<",\t\t"<<vd[3]<<",\t\t"<<vd[4]<<")"<<endl;
            cout<<"{"<<vd[0]<<","<<vd[1]<<","<<vd[2]<<","<<vd[3]<<","<<vd[4]<<"},"<<endl;
        }
        cout<<"indicies : (";
        for(QVector<GLushort>::iterator it = targetSurface->renderIndicies->begin();
            it != targetSurface->renderIndicies->end(); ++it){
            cout<<*it<<",";
        }
        cout<<")"<<endl<<endl;
    }
    return;
}
void Surface::tessEdge(){return;}

void Surface::tessVertex(const GLvoid *data){
    const GLdouble *vertexData = (const GLdouble*) data;

    Vertex* callbackVertex = new Vertex(vertexData[0], vertexData[1], vertexData[2], vertexData[3], vertexData[4]);

    bool found = false;
    int index = 0;

    for(QVector<Vertex*>::iterator vertex = targetSurface->renderVertices->begin();
        vertex != targetSurface->renderVertices->end(); ++vertex){
        if(callbackVertex->equals(**vertex)){
            targetSurface->renderIndicies->push_back(index);
            found = true;
            return;
        }
        index++;
    }
    if(!found){
        targetSurface->renderVertices->push_back(callbackVertex);
        targetSurface->renderIndicies->push_back(targetSurface->renderVertices->size() -1);
    }
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

void Surface::tessCombine(GLdouble coords[3],
                          GLdouble *vertex_data[4],
                          GLfloat weight[4], GLdouble **dataOut){
    GLdouble *vertex;
    int i;

    vertex = (GLdouble *) malloc(6 * sizeof(GLdouble));
//    cout << "combine coord" << coords[0] << "," << coords[1] << "," << coords[2] << endl;
//    cout << weight[0] << "," << weight[1] << "," << weight[2] << "," << weight[3] << endl;
//    cout << vertex_data[0] << "," << vertex_data[1] << "," << vertex_data[2] << "," << vertex_data[3] << endl;
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

    //    for (int i = 3; i < 5; i++)
//           vertex[i] =   weight[0] * vertex_data[0][i]
//                       + weight[1] * vertex_data[1][i]
//                       + weight[2] * vertex_data[2][i]
//                       + weight[3] * vertex_data[3][i];
    *dataOut = vertex;
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
