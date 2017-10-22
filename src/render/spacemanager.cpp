#include "kaola_engine/spacemanager.h"

using namespace gl3d;

static const QString OBJ_DATA_KEY_SPACE = "space";

SpaceManager::SpaceManager() {
    this->rootSpace = nullptr;
    this->culledObjects.clear();
}

SpaceManager::Space::Space(Space *p,
                           int curDpt, int maxDpt,
                           glm::vec3 maxBd,
                           glm::vec3 minBd,
                           SpaceType inType) {
    this->maxBoundary = maxBd;
    this->minBoundary = minBd;
    this->currentDepth = curDpt;
    this->maxDepth = maxDpt;
    this->containObjects.clear();
    this->parent = p;
    if (curDpt == maxDpt) {
        this->type = LEAF;
        this->left_bottom_near = nullptr;
        this->left_bottom_far = nullptr;
        this->left_top_far = nullptr;
        this->left_top_near = nullptr;
        this->right_bottom_near = nullptr;
        this->right_bottom_far = nullptr;
        this->right_top_far = nullptr;
        this->right_top_near = nullptr;
        return;
    }
    this->type = inType;
    glm::vec3 midVec = (maxBoundary + minBoundary) / 2.0f;

    this->left_top_far = new Space(this, currentDepth + 1, maxDepth,
                                   glm::vec3(
                                           midVec.x,
                                           maxBoundary.y,
                                           midVec.z
                                   ),
                                   glm::vec3(
                                           minBoundary.x,
                                           midVec.y,
                                           minBoundary.z
                                   ),
                                   LEFT_TOP_FAR);
    this->left_top_near = new Space(this, currentDepth + 1, maxDepth,
                                    glm::vec3(
                                            midVec.x,
                                            maxBoundary.y,
                                            maxBoundary.z
                                    ),
                                    glm::vec3(
                                            minBoundary.x,
                                            midVec.y,
                                            midVec.z
                                    ),
                                    LEFT_TOP_NEAR);
    this->left_bottom_far = new Space(this, currentDepth + 1, maxDepth,
                                      glm::vec3(
                                              midVec.x,
                                              midVec.y,
                                              midVec.z
                                      ),
                                      glm::vec3(
                                              minBoundary.x,
                                              minBoundary.y,
                                              minBoundary.z
                                      ),
                                      LEFT_BOTTOM_FAR);
    this->left_bottom_near = new Space(this, currentDepth + 1, maxDepth,
                                       glm::vec3(
                                               midVec.x,
                                               midVec.y,
                                               maxBoundary.z
                                       ),
                                       glm::vec3(
                                               minBoundary.x,
                                               minBoundary.y,
                                               midVec.z
                                       ),
                                       LEFT_BOTTOM_NEAR);
    this->right_top_far = new Space(this, currentDepth + 1, maxDepth,
                                    glm::vec3(
                                            maxBoundary.x,
                                            maxBoundary.y,
                                            midVec.z
                                    ),
                                    glm::vec3(
                                            midVec.x,
                                            midVec.y,
                                            minBoundary.z
                                    ),
                                    RIGHT_TOP_FAR);
    this->right_top_near = new Space(this, currentDepth + 1, maxDepth,
                                     glm::vec3(
                                             maxBoundary.x,
                                             maxBoundary.y,
                                             maxBoundary.z
                                     ),
                                     glm::vec3(
                                             midVec.x,
                                             midVec.y,
                                             midVec.z
                                     ),
                                     RIGHT_TOP_NEAR);
    this->right_bottom_far = new Space(this, currentDepth + 1, maxDepth,
                                       glm::vec3(
                                               maxBoundary.x,
                                               midVec.y,
                                               midVec.z
                                       ),
                                       glm::vec3(
                                               midVec.x,
                                               minBoundary.y,
                                               minBoundary.z
                                       ),
                                       RIGHT_BOTTOM_FAR);
    this->right_bottom_near = new Space(this, currentDepth + 1, maxDepth,
                                        glm::vec3(
                                                maxBoundary.x,
                                                midVec.y,
                                                maxBoundary.z
                                        ),
                                        glm::vec3(
                                                midVec.x,
                                                minBoundary.y,
                                                midVec.z
                                        ),
                                        RIGHT_BOTTOM_NEAR);
}

SpaceManager::Space::~Space() {
    for (auto objit : this->containObjects)
        objit->userData.remove(OBJ_DATA_KEY_SPACE);
    if (this->type != LEAF) {   // remove child tree
        delete this->left_top_far;
        delete this->left_bottom_far;
        delete this->left_top_near;
        delete this->left_bottom_near;
        delete this->left_top_far;
        delete this->left_bottom_far;
        delete this->left_top_near;
        delete this->left_bottom_near;
    }
    if (parent)
        this->parent->type = LEAF;
}

SpaceManager::Space * SpaceManager::Space::searchLeafSpace(glm::vec3 point) {
    if (gl3d::math::point_in_range(point, minBoundary, maxBoundary)) {
        if (LEAF == this->type) {
            return this;
        } else {
            glm::vec3 midVec = (maxBoundary + minBoundary) / 2.0f;
            bool inright = (point.x > midVec.x);
            bool intop = (point.y > midVec.y);
            bool innear = (point.z > midVec.z);
            if (innear) {
                if (intop) {
                    if (inright)
                        return right_top_near->searchLeafSpace(point);
                    else
                        return left_top_near->searchLeafSpace(point);
                } else {
                    if (inright)
                        return right_bottom_near->searchLeafSpace(point);
                    else
                        return left_bottom_near->searchLeafSpace(point);
                }
            } else {
                if (intop) {
                    if (inright)
                        return right_top_far->searchLeafSpace(point);
                    else
                        return left_top_far->searchLeafSpace(point);
                } else {
                    if (inright)
                        return right_bottom_far->searchLeafSpace(point);
                    else
                        return left_bottom_far->searchLeafSpace(point);
                }
            }
        }
    }
    else {
        return nullptr;
    }
}

void SpaceManager::initWithDepthAndSize(int maxDepth, glm::vec3 maxBoundary, glm::vec3 minBoundary) {
    if (nullptr != this->rootSpace) {
        delete this->rootSpace;
    }
    this->rootSpace = new Space(nullptr, 0, maxDepth, maxBoundary, minBoundary, Space::ROOT);
}

void SpaceManager::destroy() {
    delete this->rootSpace;
    this->rootSpace = nullptr;
    this->culledObjects.clear();
}

bool SpaceManager::insertObject(gl3d::abstract_object *obj) {
    if (rootSpace) {
        glm::vec4 pos = obj->get_translation_mat() * glm::vec4((obj->getMaxBoundry() + obj->getMinBoundry()) / 2.0f, 1.0f);
        pos = pos / pos.w;
        Space * spc = rootSpace->searchLeafSpace(glm::vec3(pos));
        if (spc) {
            obj->userData.insert(OBJ_DATA_KEY_SPACE, spc);
            spc->containObjects.insert(obj);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool SpaceManager::removeObject(gl3d::abstract_object *obj) {
    if (obj->userData.contains(OBJ_DATA_KEY_SPACE)) {
        Space * spc = (Space *) obj->userData.value(OBJ_DATA_KEY_SPACE);
        spc->containObjects.remove(obj);
    }
    return true;
}

QList<gl3d::abstract_object *> SpaceManager::getCulledObjects() {
    return culledObjects;
}

SpaceManager::Space * SpaceManager::getSpace(glm::vec3 point) {
    if (rootSpace) {
        return rootSpace->searchLeafSpace(point);
    } else {
        return nullptr;
    }
}

void SpaceManager::cullObjects(gl3d::viewer *watcher) {

}