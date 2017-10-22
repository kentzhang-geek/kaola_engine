#ifndef SPACEMANAGER_H
#define SPACEMANAGER_H

#include "gl3d_object.h"
#include "glheaders.h"
#include "utils/gl3d_math.h"

namespace gl3d {
    class SpaceManager {
    public:
        static SpaceManager * Manager();
        void initWithDepthAndSize(int maxDepth = 6,
                                  glm::vec3 maxBoundary = glm::vec3(1000.0f),
                                  glm::vec3 minBoundary = glm::vec3(-1000.0f));
        void destroy();
        bool insertObject(object * obj);
        bool removeObject(object * obj);
        void cullObjects();
        QList<gl3d::abstract_object *> getCulledObjects();

    private:
        QList<gl3d::abstract_object *> culledObjects;
        class Space {
        public:
            typedef enum _tag_Space_Type {
                ROOT,                   //根
                BOTTOM_LEFT_FRONT,      // 1
                BOTTOM_RIGHT_FRONT,     // 2
                BOTTOM_LEFT_BACK,       // 3
                BOTTOM_RIGHT_BACK,      // 4
                TOP_LEFT_FRONT,         // 5
                TOP_RIGHT_FRONT,        // 6
                TOP_LEFT_BACK,          // 7
                TOP_RIGHT_BACK,         // 8
                LEAF
            } SpaceType;

            // methods:
            Space(Space * parent, int currentDepth, int maxDepth);
            ~Space();

            // values:
            SpaceType type; // type of this space
            glm::vec3 maxBoundary;
            glm::vec3 minBoundary;
            int currentDepth;
            int maxDepth;
            Space *parent;
            Space *bottom_left_front_node;
            Space *bottom_right_front_node;
            Space *bottom_left_back_node;
            Space *bottom_right_back_node;
            Space *top_left_front_node;
            Space *top_right_front_node;
            Space *top_left_back_node;
            Space *top_right_back_node;
        };
        Space * rootSpace;
        SpaceManager(); // should not call constructor outside
    };
}

#endif // SPACEMANAGER_H