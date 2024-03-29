#include "editor/surface.h"
#include <iostream>
#include <QMutex>
#include <sstream>
#include "resource_and_network/klm_resource_manager.h"

using namespace klm;
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

Surface::Surface(const Surface* parent)
    : parent(parent), surfaceVisible(true), connectiveVisible(true),
      subSurfaces(new QVector<Surface*>),
      localVerticies(new QVector<Surface::Vertex*>),
      transformFromParent(new glm::mat4(1.0)),boundingBox(nullptr),
      scale(nullptr), rotation(nullptr),translate(nullptr),
      attachedFurniture(new QMap<std::string, Furniture*>),
      independShape(nullptr), parentialShape(nullptr),
      renderingVerticies(nullptr), renderingIndicies(nullptr),
      connectiveVertices(nullptr), connectiveIndices(nullptr),
      surfaceMaterial(nullptr), connectiveMaterial(nullptr){
}

Surface::Surface(const QVector<glm::vec3> &points, const Surface* parent) throw(SurfaceException)
    : parent(parent), surfaceVisible(true), connectiveVisible(true),
      subSurfaces(new QVector<Surface*>),
      localVerticies(new QVector<Surface::Vertex*>),
      scale(nullptr), rotation(nullptr),translate(nullptr),
      attachedFurniture(new QMap<std::string, Furniture*>),
      renderingVerticies(nullptr), renderingIndicies(nullptr),
      connectiveVertices(nullptr), connectiveIndices(nullptr),
      surfaceMaterial(nullptr), connectiveMaterial(nullptr){

    if(points.size() < 3){
        throw SurfaceException("can not create Surface using less then three points");
    }

    glm::vec3 planeNormal = getPlanNormal(points);
    if(planeNormal == NON_NORMAL){
        throw SurfaceException("can not create Surface if not all points are on same plane");
    }

    independShape = new bg_Polygon();
    parentialShape = new bg_Polygon();

    //set verticies given by consturctor to local vertices
    //and get transform from parent
    boundingBox = new BoundingBox(points);
    glm::vec3 center = boundingBox->getCenter();
    glm::mat4 rotation;
    getRotation(planeNormal, Z_AXIS, rotation);
    for(QVector<glm::vec3>::const_iterator point = points.begin();
        point != points.end(); ++point){

        Vertex* vertex = new Vertex(*point);

        parentialShape->outer().push_back(bg_Point(vertex->x(), vertex->y()));
        vertex->x(vertex->x() - center.x);
        vertex->y(vertex->y() - center.y);
        vertex->z(vertex->z() - center.z);

        transformVertex(rotation, *vertex);
        localVerticies->push_back(vertex);
        independShape->outer().push_back(bg_Point(vertex->x(), vertex->y()));
    }
    transformFromParent = new glm::mat4(glm::translate(center) * glm::inverse(rotation));

    delete boundingBox;
    boundingBox = new Surface::BoundingBox(*localVerticies);
    boundingBox->generateTexture(*localVerticies);

    updateSurfaceMesh();
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
        qDeleteAll(subSurfaces->begin(), subSurfaces->end());
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

    if(attachedFurniture != nullptr){
        qDeleteAll(attachedFurniture->begin(), attachedFurniture->end());
        attachedFurniture->clear();
        delete attachedFurniture;
    }
}

Surface* Surface::addSubSurface(const QVector<glm::vec3> &points){
    Surface* ret = nullptr;
    try{
        ret = new Surface(points, this);
    } catch(SurfaceException &ex){
        return nullptr;
    }

    bool subSurfaceFits =
            boost::geometry::covered_by((*this->independShape), (*ret->parentialShape)) ||
            boost::geometry::within((*ret->parentialShape), (*this->independShape));

    if(!subSurfaceFits){
        delete ret;
        return nullptr;
    } else {

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
    }
    if(subSurfaceFits){
        subSurfaces->push_back(ret);
        updateSurfaceMesh();
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
            updateSurfaceMesh();
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
    glm::mat4 matrix(1.0);
    if(parent != nullptr){
        matrix =
                parent->getRenderingTransform() *
                getTransformFromParent() *
                getSurfaceTransform();
    } else {
        matrix =
                getTransformFromParent() *
                getSurfaceTransform();
    }
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
    glm::mat4 trans = getTransformFromParent() * getSurfaceTransform();

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
    updateConnectionMesh();
}

void Surface::setRotation(const glm::mat4 &rotation){
    if(this->rotation == nullptr){
        this->rotation = new glm::mat4(rotation);
    } else {
        *(this->rotation) = rotation;
    }
    updateConnectionMesh();
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
    if(translate != nullptr){
        matrix *= glm::translate(*translate);
    }
    if(rotation != nullptr){
        matrix *= (*rotation);
    }
    if(scale != nullptr){
        matrix *= glm::scale(*scale);
    }
    return matrix;
}

bool Surface::isConnectiveSurface() const{
    glm::mat4 transform = getSurfaceTransform();
    bool equals = (transform != glm::mat4(1.0f));
    return equals;
}

bool Surface::isSurfaceVisible() const{
    return surfaceVisible;
}

void Surface::showSurface(){
    surfaceVisible = true;
}

void Surface::hideSurface(){
    surfaceVisible = false;
}

bool Surface::isConnectiveVisible() const{
    return connectiveVisible;
}

void Surface::showConnective(){
    connectiveVisible = true;
}

void Surface::hideConnective(){
    connectiveVisible = false;
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
    static QVector<Surface::Vertex*> ret;
    if(ret.size() != 0){
        for(QVector<Surface::Vertex*>::iterator v = ret.begin();
            v != ret.end(); ++v){
            delete *v;
        }
    }
    ret.clear();

    glm::mat4 trans = getRenderingTransform();
    for(QVector<Surface::Vertex*>::iterator vertex = renderingVerticies->begin();
        vertex != renderingVerticies->end(); ++vertex){
        Surface::Vertex* v = new Surface::Vertex(**vertex);
        transformVertex(trans, *v);
        ret.push_back(v);
    }

    return &ret;
}

const QVector<Surface::Vertex*>* Surface::getConnectiveVerticies(){
    static QVector<Surface::Vertex*> ret;
    if(ret.size() != 0){
        for(QVector<Surface::Vertex*>::iterator v = ret.begin();
            v != ret.end(); ++v){
            delete *v;
        }
    }
    ret.clear();

    glm::mat4 *transform = parent == nullptr ?
                new glm::mat4(1.0f) :
                new glm::mat4(parent->getRenderingTransform());

    for(QVector<Surface::Vertex*>::iterator vertex = connectiveVertices->begin();
        vertex != connectiveVertices->end(); ++vertex){
        Surface::Vertex* v = new Surface::Vertex(**vertex);
        transformVertex(*transform, *v);
        ret.push_back(v);
    }

    delete transform;

    return &ret;
}

const QVector<GLushort>* Surface::getRenderingIndices(){
    static QVector<GLushort> ret;
    if(ret.size() != 0){
        ret.clear();
    }
    for(QVector<GLushort>::iterator index = renderingIndicies->begin();
        index != renderingIndicies->end(); ++index){
        ret.push_back(*index);
    }
    return &ret;
}

const QVector<GLushort>* Surface::getConnectiveIndicies(){
    static QVector<GLushort> ret;
    if(ret.size() != 0){
        ret.clear();
    }
    for(QVector<GLushort>::iterator index = connectiveIndices->begin();
        index != connectiveIndices->end(); ++index){
        ret.push_back(*index);
    }
    return &ret;
}

Surfacing* Surface::getSurfaceMaterial() const{
    return surfaceMaterial;
}

void Surface::setSurfaceMaterial(Surfacing *surfacing){
    this->surfaceMaterial = surfacing;
}

Surfacing* Surface::getConnectiveMaterial() const{
    return connectiveMaterial;
}

void Surface::setConnectiveMateiral(Surfacing *connective){
    this->connectiveMaterial = connective;
}

void Surface::addFurniture(Furniture* furniture){
    attachedFurniture->insert(furniture->getPickID(), furniture);
}

int Surface::removeFurniture(const std::string &pickID){
    return attachedFurniture->remove(pickID);
}

Furniture* Surface::getFurniture(const string &pickID){
    return attachedFurniture->value(pickID);
}

void Surface::save(pugi::xml_node &node){
    pugi::xml_node surfaceNode = node.append_child(IOUtility::SURFACE.c_str());

    // materials hot fix
    if (this->surfaceMaterial != NULL)
        surfaceNode.append_attribute("mtl_id").set_value(this->surfaceMaterial->getID().c_str());
    if (this->connectiveMaterial != NULL)
        surfaceNode.append_attribute("cnt_mtl_id").set_value(this->connectiveMaterial->getID().c_str());

    writeVerticies(surfaceNode, *localVerticies, IOUtility::LOCAL_VERTIICES.c_str());

    IOUtility::writeMatrix(surfaceNode, *transformFromParent, IOUtility::TRANSFORM_FROM_PPARENT.c_str());
    surfaceNode.append_attribute(IOUtility::SURFACE_VISIBLE.c_str()).set_value(surfaceVisible);
    surfaceNode.append_attribute(IOUtility::CONNECTIVE_VISIBLE.c_str()).set_value(connectiveVisible);

    if(surfaceMaterial != nullptr){
        pugi::xml_node materialNode = surfaceNode.append_child(IOUtility::SURFACE_MATEIRAL.c_str());
        materialNode.append_attribute(IOUtility::MERCHANIDSE_ID.c_str()).set_value(surfaceMaterial->getID().c_str());

        IOUtility::writeMatrix(materialNode, surfaceMaterial->getRotation());
        IOUtility::writeVector(materialNode, surfaceMaterial->getTranslation());
    }

    if(connectiveMaterial != nullptr){
        pugi::xml_node materialNode = surfaceNode.append_child(IOUtility::CONNECTIVE_MATERIAL.c_str());
        materialNode.append_attribute(IOUtility::MERCHANIDSE_ID.c_str()).set_value(connectiveMaterial->getID().c_str());

        IOUtility::writeMatrix(materialNode, connectiveMaterial->getRotation());
        IOUtility::writeVector(materialNode, connectiveMaterial->getTranslation());
    }

    for(QMap<std::string, Furniture*>::iterator furniture = attachedFurniture->begin();
        furniture != attachedFurniture->end(); ++furniture){
        pugi::xml_node furnitureNode = surfaceNode.append_child(IOUtility::FUNITURE.c_str());
        furnitureNode.append_attribute(IOUtility::FURNITURE_ID.c_str()).set_value((*furniture)->getID().c_str());
        IOUtility::writeMatrix(furnitureNode, (*furniture)->getRotation());
        IOUtility::writeVector(furnitureNode, (*furniture)->getTranslation());
    }

    if(scale != nullptr){
        IOUtility::writeVector(surfaceNode, *scale, IOUtility::SURFACE_SCALE);
    }
    if(rotation != nullptr){
        IOUtility::writeMatrix(surfaceNode, *rotation, IOUtility::SURFACE_ROTATION);
    }
    if(translate != nullptr){
        IOUtility::writeVector(surfaceNode, *translate, IOUtility::SURFACE_TRANSLATE);
    }

    if(subSurfaces != nullptr){
        for(QVector<Surface*>::iterator subSurface = subSurfaces->begin();
            subSurface != subSurfaces->end(); ++subSurface){
            (*subSurface)->save(surfaceNode);
        }
    }

}

bool Surface::load(const pugi::xml_node &node){
    if(IOUtility::SURFACE.compare(std::string(node.name())) == 0){
        if(!readVerticies(node, localVerticies)){
            return false;
        }

        // materials
        std::string mtlid(node.attribute("mtl_id").value());
        this->surfaceMaterial = new Surfacing(mtlid);
        mtlid = std::string(node.attribute("cnt_mtl_id").value());
        if (mtlid.size() > 0)
            this->connectiveMaterial = new Surfacing(mtlid);

        static const string TFPXPath = "./"+IOUtility::MATRIX+"[@"+IOUtility::USAGE+"=\""
                +IOUtility::TRANSFORM_FROM_PPARENT+"\"]";
        pugi::xpath_node_set matrixXPathNode = node.select_nodes(TFPXPath.c_str());
        if(matrixXPathNode.size() != 1){
            return false;
        }
        if(this->transformFromParent == nullptr){
            this->transformFromParent = new glm::mat4;
        }

        IOUtility::readMatrix(matrixXPathNode.first().node(), *transformFromParent);

        if(node.attribute(IOUtility::SURFACE_VISIBLE.c_str())){
            surfaceVisible = node.attribute(IOUtility::SURFACE_VISIBLE.c_str()).as_bool();
        }

        if(node.attribute(IOUtility::CONNECTIVE_VISIBLE.c_str())){
            connectiveVisible = node.attribute(IOUtility::CONNECTIVE_VISIBLE.c_str()).as_bool();
        }

        if(attachedFurniture != nullptr){
            attachedFurniture = new QMap<std::string, Furniture*>;
        }
        static const string furnitureXPath = "./"+IOUtility::FUNITURE+"[@"+IOUtility::FURNITURE_ID+"]";
        pugi::xpath_node_set furnitureXPathes = node.select_nodes(furnitureXPath.c_str());
        for(pugi::xpath_node furnitureNode : furnitureXPathes){
            //todo: load furniture from resource management
            std::string id = furnitureNode.node().attribute(IOUtility::FURNITURE_ID.c_str()).as_string();
            Furniture* furniturePtr = (Furniture*)klm::resource::manager::shared_instance()->get_merchandise_item(id);
            if(furniturePtr != nullptr){
                attachedFurniture->insert(id, furniturePtr);

                static const string furnitureRotXPath = "./"+IOUtility::MATRIX;
                pugi::xpath_node rotNode = furnitureNode.node().select_node(furnitureRotXPath.c_str());
                if(rotNode){
                    glm::mat4 rot;
                    IOUtility::readMatrix(rotNode.node(), rot);
                    furniturePtr->setRotation(rot);
                }

                static const string furnitureTranXPath = "./"+IOUtility::VECTOR;
                pugi::xpath_node tranNode = furnitureNode.node().select_node(furnitureTranXPath.c_str());
                if(tranNode){
                    glm::vec3 trans;
                    IOUtility::readVector(tranNode.node(), trans);
                    furniturePtr->setTranslateiong(trans);
                }
            }

        }

        static const string surfaceScaleXpath = "./"+IOUtility::VECTOR+"[@"+IOUtility::USAGE+"=\""+IOUtility::SURFACE_SCALE+"\"]";
        pugi::xpath_node scaleNode = node.select_node(surfaceScaleXpath.c_str());
        if(scaleNode){
            glm::vec3 scaleRead;
            IOUtility::readVector(scaleNode.node(), scaleRead);
            if(scale != nullptr){
                *scale = scaleRead;
            } else {
                scale = new glm::vec3(scaleRead);
            }
        }

        static const string surfaceRotXPath =
                "./"+IOUtility::MATRIX+"[@"+IOUtility::USAGE+"=\""+IOUtility::SURFACE_ROTATION+"\"]";
        pugi::xpath_node rotNode = node.select_node(surfaceRotXPath.c_str());
        if(rotNode){
            glm::mat4 rotRead;
            IOUtility::readMatrix(rotNode.node(), rotRead);
            if(rotation != nullptr){
                *rotation = rotRead;
            } else {
                rotation = new glm::mat4(rotRead);
            }
        }

        static const string surfaceTransXPath = "./"+IOUtility::VECTOR+"[@"+IOUtility::USAGE+"=\""+IOUtility::SURFACE_TRANSLATE+"\"]";
        pugi::xpath_node tranNode = node.select_node(surfaceTransXPath.c_str());
        if(tranNode){
            glm::vec3 transRead;
            IOUtility::readVector(tranNode.node(), transRead);
            if(translate != nullptr){
                *translate = transRead;
            } else {
                translate = new glm::vec3(transRead);
            }
        }

        static const string subSurfaceXPath = "./"+IOUtility::SURFACE
                    +"[@"+IOUtility::NUM_OF_VERTICES+" and @"+IOUtility::SURFACE_VISIBLE+" and@"+IOUtility::CONNECTIVE_VISIBLE+"]";
        pugi::xpath_node_set subSurfaceNodeSet = node.select_nodes(subSurfaceXPath.c_str());
        if(subSurfaces == nullptr){
            subSurfaces = new QVector<Surface*>;
        }
        for(pugi::xpath_node subSurfaceNode : subSurfaceNodeSet){
            Surface* sub = new Surface(this);
            if(sub->load(subSurfaceNode.node())){
                subSurfaces->push_back(sub);
            } else {
                delete sub;
            }
        }

        if(boundingBox != nullptr){
            delete boundingBox;
        }
        boundingBox = new Surface::BoundingBox(*localVerticies);
        boundingBox->generateTexture(*localVerticies);

        //finally generate shapes
        if(independShape != nullptr){
            delete independShape;
        }
        independShape = new bg_Polygon;
        if(parentialShape != nullptr){
            delete parentialShape;
        }
        parentialShape = new bg_Polygon;

        QVector<Surface::Vertex*> verticies;
        getLocalVertices(verticies);
        glm::mat4 transform = getTransformFromParent();
        for(QVector<Surface::Vertex*>::iterator vertex = verticies.begin();
            vertex != verticies.end(); ++vertex){
            Surface::Vertex* v = (*vertex);
            independShape->outer().push_back(bg_Point(v->x(), v->y()));
            transformVertex(transform, *v);
            parentialShape->outer().push_back(bg_Point(v->x(), v->y()));
            delete v;
        }

        updateSurfaceMesh();
        updateConnectionMesh();

        return true;
    } else {
        return false;
    }
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

        baseVertex->w(0.0);
        baseVertex->h(0.0);
        derivedVertex->w(0.0);
        derivedVertex->h(derivedVertex->distance(*baseVertex));

        connectiveVertices->push_back(baseVertex);
        connectiveVertices->push_back(derivedVertex);

        for(int index = 1; index != base.size(); ++index){
            Surface::Vertex* vb = new Surface::Vertex(*base[index]);
            Surface::Vertex* vd = new Surface::Vertex(*derived[index]);

            vb->w(baseVertex->distance(*vb) + baseVertex->w());
            vb->h(0.0);
            vd->w(derivedVertex->distance(*vd) + derivedVertex->w());
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

        connectiveVertices->push_back(endBaseVertex);
        connectiveVertices->push_back(endDerivedVertex);

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

        if(CONN_DEBUG){
            Surface::vertexLogger(*connectiveVertices, *connectiveIndices,
                                  "connective surface in world coordinate system");
        }

    } else {
        if(CONN_DEBUG){
            qDebug()<<"no connective surface for Surface ("<<this<<")";
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
            qDebug() <<"Tessellation begin with target Surface is not null";
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
        qDebug()<<"gluTessBeginPolygon(tess, 0);";
    }
    {
        gluTessBeginContour(tess);
        if(TESS_DEBUG){
            qDebug()<<"\tgluTessBeginContour(tess);";
        }

        targetSurface->getLocalVertices(*verticies);
        for(QVector<Surface::Vertex*>::iterator vertex = verticies->begin();
            vertex != verticies->end(); ++vertex){
            Surface::Vertex* vertexData = new Surface::Vertex(**vertex);
            GLdouble *vertexDataElement = vertexData->getData();
            gluTessVertex(tess, vertexDataElement, vertexDataElement);
            if(TESS_DEBUG){
                qDebug() <<"\t\tgluTessVertex(tess, vertexDataElement, vertexDataElement);";
                qDebug() <<" ("<<vertexData->x()<<","<<vertexData->y()<<","<<vertexData->z()<<")";
            }
        }

        gluTessEndContour(tess);
        if(TESS_DEBUG){
            qDebug()<<"\tgluTessEndContour(tess);";
        }

        for(QVector<Surface*>::iterator subSurface = targetSurface->subSurfaces->begin();
            subSurface != targetSurface->subSurfaces->end(); ++subSurface){
            verticies->clear();
            (*subSurface)->getVerticesOnParent(*verticies);

            gluTessBeginContour(tess);
            if(TESS_DEBUG){
                qDebug()<<"\tgluTessBeginContour(tess);";
            }

            for(QVector<Surface::Vertex*>::iterator vertex = verticies->begin();
                vertex != verticies->end(); ++vertex){
                Surface::Vertex* vertexData = new Surface::Vertex(**vertex);
                targetSurface->boundingBox->generateTexture(*vertexData);
                GLdouble* vertexDataElement = vertexData->getData();
                gluTessVertex(tess, vertexDataElement, vertexDataElement);
                if(TESS_DEBUG){
                    qDebug()<<"\t\tgluTessVertex(tess, vertexDataElement, vertexDataElement);";
                    qDebug()<<" ("<<vertexData->x()<<","<<vertexData->y()<<","<<vertexData->z()<<")";
                }
            }

            gluTessEndContour(tess);
            if(TESS_DEBUG){
                qDebug()<<"\tgluTessEndContour(tess);";
            }
        }
    }
    gluTessEndPolygon(tess);
    if(TESS_DEBUG){
        qDebug()<<"gluTessEndPolygon(tess);";
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
        qDebug()<<"tessellation process has began for Surface : ("
            <<targetSurface<<")";
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
        targetSurface->renderingIndicies->push_back(targetSurface->renderingVerticies->size() - 1);
    }
}

void Surface::tessEnd(){
    if(TESS_DEBUG){
        vertexLogger(*(targetSurface->renderingVerticies),
                     *(targetSurface->renderingIndicies),
                     "tessellated result in local coordinate system ");
    }
}

void Surface::tessEdge(){return;}

void Surface::tessError(GLenum type){
    qDebug()<<"Tessellation Error has been called";
    const GLubyte *errStr;
    errStr = gluErrorString(type);
    qDebug()<<"[ERROR] : "<<errStr;
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
//
//int main() {
//    glm::vec3 vp1(0.0f, 0.0f, -1.0f);
//    glm::vec3 vp2(0.0f);
//    glm::vec3 vp3(1.0f, 0.0f, 0.0f);
//
//    glm::vec3 lineOne = vp1 - vp2;
//    glm::vec3 lineTwo = vp3 - vp2;
//    glm::vec3 normal = glm::cross(lineTwo, lineOne);
//    normal = glm::normalize(normal);
//
//    return 0;
//}

static inline bool is_point_at_same_face(glm::vec3 pa, glm::vec3 pb, glm::vec3 pc) {
    return glm::abs(glm::dot(glm::cross(pb - pa, pc - pa), pb - pc)) > 0.01f;
}

glm::vec3 Surface::getPlanNormal(const QVector<glm::vec3> &points) {
    for (int i = 0; i < (points.size() - 2); i++) {
        if (is_point_at_same_face(points[i], points[i+1], points[i+2])) {
            return NON_NORMAL;
        }
    }
    if (is_point_at_same_face(points[0], points[1], points.last()))
        return NON_NORMAL;
    if (is_point_at_same_face(points[0], points.last(), points.at(points.size() - 2)))
        return NON_NORMAL;

    glm::vec3 normal(0.0f);
    for (int i = 0; i < (points.size() - 1); i++) {
        normal += glm::cross(points.at(i), points.at(i+1));
    }
    normal += glm::cross(points.last(), points.first());

    return glm::normalize(normal);
}

//glm::vec3 Surface::getPlanNormal(const QVector<glm::vec3> &points){
//    if(points.size() < 3){
//        return NON_NORMAL;
//    }
//
//    glm::vec3 sampleNormal = sameLine(points.back(), points.front(), points[1]) ?
//                            NON_NORMAL: getNormal(points.back(),
//                                                  points.front(),
//                                                  points[1]);
//    for(int index = 1; index != (points.size() -1); ++index){
//        const glm::vec3 &p1 = points[index - 1];
//        const glm::vec3 &p2 = points[index];
//        const glm::vec3 &p3 = points[index + 1];
//
//        if(!sameLine(p1, p2, p3)){
//            glm::vec3 testNormal = getNormal(p1, p2, p3);
//            if(sampleNormal == NON_NORMAL){
//                sampleNormal = testNormal;
//            }
//            if(!equalVecr(testNormal,sampleNormal)){
//                return NON_NORMAL;
//            }
//        }
//    }
//    return sampleNormal;

    //
//    glm::vec3 sampleNormal = getNormal(points.back(),
//                                  points.front(),
//                                  points[1]);
//    for(int index = 1; index != (points.size() -1); ++index){
//        glm::vec3 testNormal = getNormal(points[index -1],
//                                    points[index],
//                                    points[index + 1]);
//        if(!equalVecr(testNormal,sampleNormal)){
//            return NON_NORMAL;
//        }
//    }
//    return sampleNormal;
//}

bool Surface::sameLine(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3){
//    return 0.0 == ((p1.x * (p2.y -p3.y) + p2.x * (p2.y - p1.y) + p3.x * (p1.y - p2.y))/2);
    float test = glm::abs(glm::dot(glm::normalize(p2 - p1),
                                   glm::normalize(p3 - p1)));
    return test > 0.98;
}

void Surface::getRotation(const glm::vec3 &source,
                          const glm::vec3 &dest, glm::mat4 &matrix){
    glm::vec3 tempSource = source;
    tempSource = glm::normalize(tempSource);
    if(tempSource == dest){
        matrix = glm::mat4(1.0f);
    } else {
        GLfloat sourceLen = glm::length(source);
        GLfloat destLen = glm::length(dest);
        if(sourceLen == 0.0f || destLen == 0.0f){
            matrix = glm::mat4(1.0f);
        } else {
            glm::vec3 crossVec = glm::cross(source, dest);
            if(crossVec == NON_NORMAL){
                crossVec.y = 1.0f;
            }
            matrix =glm::rotate(
                    glm::acos(glm::dot(source, dest)/(sourceLen * destLen)),
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

void Surface::writeVerticies(pugi::xml_node &node, const QVector<Surface::Vertex *> &verticies, const string tag){
    if(verticies.size() != 0){
        node.append_attribute(IOUtility::NUM_OF_VERTICES.c_str()).set_value(verticies.size());
        if(tag.size() != 0){
            node.append_attribute(IOUtility::USAGE.c_str()).set_value(tag.c_str());
        }
        int cnt = 0;
        for(QVector<Surface::Vertex*>::const_iterator vertex = verticies.begin();
            vertex != verticies.end(); ++vertex){
            pugi::xml_node vertexNode = node.append_child(IOUtility::VERTEX.c_str());
            vertexNode.append_attribute(IOUtility::INDEX.c_str()).set_value(cnt++);
            vertexNode.append_attribute(IOUtility::X.c_str()).set_value((*vertex)->x());
            vertexNode.append_attribute(IOUtility::Y.c_str()).set_value((*vertex)->y());
            vertexNode.append_attribute(IOUtility::Z.c_str()).set_value((*vertex)->z());
            vertexNode.append_attribute(IOUtility::W.c_str()).set_value((*vertex)->w());
            vertexNode.append_attribute(IOUtility::H.c_str()).set_value((*vertex)->h());
        }
    }
}

bool Surface::readVerticies(const pugi::xml_node &node, QVector<Surface::Vertex *> *ret){
    static std::string xpath = "./"+IOUtility::VERTEX + "[" +
            "@"+ IOUtility::X+" and @"+ IOUtility::Y+" and @"+ IOUtility::Z+
            +" and @"+ IOUtility::W+" and @"+ IOUtility::H+" and @"+ IOUtility::INDEX+
            "]";

    pugi::xml_attribute nov = node.attribute(IOUtility::NUM_OF_VERTICES.c_str());

    if(ret != nullptr){
        for(QVector<Surface::Vertex*>::iterator vertex = ret->begin();
            vertex != ret->end(); ++vertex){
            delete *vertex;
        }
        ret->clear();
    } else {
        ret = new QVector<Surface::Vertex*>;
    }

    for(int i = 0; i != nov.as_int(); ++i){
        ret->push_back(nullptr);
    }

    pugi::xpath_node_set vertexNodes = node.select_nodes(xpath.c_str());
    for(pugi::xpath_node vertexNode : vertexNodes){
        int index = vertexNode.node().attribute(IOUtility::INDEX.c_str()).as_int();
        Surface::Vertex* vertex = new Surface::Vertex();
        vertex->x(vertexNode.node().attribute(IOUtility::X.c_str()).as_double());
        vertex->y(vertexNode.node().attribute(IOUtility::Y.c_str()).as_double());
        vertex->z(vertexNode.node().attribute(IOUtility::Z.c_str()).as_double());
        vertex->w(vertexNode.node().attribute(IOUtility::W.c_str()).as_double());
        vertex->h(vertexNode.node().attribute(IOUtility::H.c_str()).as_double());
        if(ret->operator [](index) != nullptr){
            for(QVector<Surface::Vertex*>::iterator v = ret->begin();
                v != ret->end(); ++v){
                delete *v;
            }
            return false;
        }
        ret->operator [](index) = vertex;
    }
    return true;
}

void Surface::vertexLogger(const QVector<Surface::Vertex *> &verticies,
                           const QVector<GLushort> &indicies,
                           const string title){
    qDebug()<<title.c_str()<<" : ";
    qDebug()<<"{";
    for(QVector<Surface::Vertex*>::const_iterator vertex = verticies.begin();
        vertex != verticies.end(); ++vertex){
        qDebug()<<"{"<<(*vertex)->x()<<", "<<(*vertex)->y()<<", "<<(*vertex)->z()<<", "
           <<(*vertex)->w()<<", "<<(*vertex)->h()<<"},";
    }
    qDebug()<<"}";

    qDebug()<<"indices = "<<"{";
    int cnt = 0;
    for(QVector<GLushort>::const_iterator index = indicies.begin();
        index != indicies.end(); ++index){
        qDebug()<<*index<<",";
        if(cnt++ % 3 == 0){
            qDebug();
        }
    }
    qDebug()<<"}";
}

Surface::Vertex::Vertex(){
    data = new GLdouble[SIZE];
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
    data = new GLdouble[SIZE];
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

Surface::BoundingBox::BoundingBox(const QVector<Surface::Vertex*> &verticies){
    init();
    for(QVector<Surface::Vertex*>::const_iterator vertex = verticies.begin();
        vertex != verticies.end(); ++vertex){
        glm::vec3 v((*vertex)->x(), (*vertex)->y(), (*vertex)->z());
        setMinimal(v);
        setMaximal(v);
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
    xMax = numeric_limits<double>::lowest();
    yMax = numeric_limits<double>::lowest();
    zMax = numeric_limits<double>::lowest();
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
