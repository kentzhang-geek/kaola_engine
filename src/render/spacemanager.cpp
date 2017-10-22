#include "kaola_engine/spacemanager.h"

using namespace gl3d;
static SpaceManager * __global_manager = nullptr;

SpaceManager::SpaceManager() {
    this->rootSpace = nullptr;
    this->culledObjects.clear();
}

SpaceManager* SpaceManager::Manager() {
    if (nullptr == __global_manager) {
        __global_manager = new SpaceManager;
    }
    return __global_manager;
}
