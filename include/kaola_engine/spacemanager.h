#ifndef SPACEMANAGER_H
#define SPACEMANAGER_H

#include "gl3d_object.h"
#include "glheaders.h"
#include "utils/gl3d_math.h"
#include "gl3d_viewer.h"

namespace gl3d {
    class SpaceManager {
    public:
        class Space {
        public:
            typedef enum _tag_Space_Type {
                ROOT,                   //根
                LEFT_TOP_NEAR,
                LEFT_TOP_FAR,
                LEFT_BOTTOM_NEAR,
                LEFT_BOTTOM_FAR,
                RIGHT_TOP_NEAR,
                RIGHT_TOP_FAR,
                RIGHT_BOTTOM_NEAR,
                RIGHT_BOTTOM_FAR,
                LEAF
            } SpaceType;

            // methods:
            Space(Space *parent, int currentDepth, int maxDepth, glm::vec3 maxBd, glm::vec3 minBd,
                  SpaceType inType);
            Space * searchLeafSpace(glm::vec3 point);
            ~Space();

            // values:
            SpaceType type; // type of this space
            glm::vec3 maxBoundary;
            glm::vec3 minBoundary;
            int currentDepth;
            int maxDepth;
            Space *parent;
            QSet<gl3d::abstract_object *> containObjects;
            Space *left_top_near;
            Space *left_top_far;
            Space *left_bottom_near;
            Space *left_bottom_far;
            Space *right_top_near;
            Space *right_top_far;
            Space *right_bottom_near;
            Space *right_bottom_far;
        };
        SpaceManager();
        void initWithDepthAndSize(int maxDepth = 6,
                                  glm::vec3 maxBoundary = glm::vec3(1000.0f),
                                  glm::vec3 minBoundary = glm::vec3(-1000.0f));
        void destroy();
        bool insertObject(gl3d::abstract_object * obj);
        bool removeObject(gl3d::abstract_object * obj);
        void cullObjects(gl3d::viewer * watcher);
        QList<gl3d::abstract_object *> getCulledObjects();
        Space * getSpace(glm::vec3 point);

    private:
        QList<gl3d::abstract_object *> culledObjects;
        Space * rootSpace;
    };
}

#endif // SPACEMANAGER_H