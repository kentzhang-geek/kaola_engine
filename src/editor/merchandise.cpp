#include "editor/merchandise.h"

using namespace klm;

Merchandise::Merchandise(const string &tid) {
    this->id = tid;
}

Merchandise::~Merchandise() {
    return;
}

glm::mat4 Merchandise::getRotation() const {
    return *this->rotation;
}

glm::vec3 Merchandise::getTranslation() const {
    return *this->translation;
}

glm::mat4 Merchandise::getTransformation() const {
    return glm::translate(glm::mat4(1.0f), (*this->translation)) * (*this->rotation);
}

void Merchandise::setRotation(const glm::mat4 &rotation) {
    *this->rotation = rotation;
}

void Merchandise::setTranslateiong(const glm::vec3 &translation) {
    *this->translation = translation;
}

string Merchandise::getID() const {
    return this->id;
}


Surfacing::Surfacing(const string &id) : Merchandise(id) {
}

Surfacing::~Surfacing() {
    return;
}

float Surfacing::getPrice() {
    GL3D_UTILS_THROW("Not implemented yet");
    return 0.0f;
}

Merchandise::MerchandiseType Surfacing::getType() {
    return Merchandise::MerchandiseType::Surfacing;
}

Furniture::Furniture(const string &id) : Merchandise(id) {
}

Furniture::~Furniture() {
    return ;
}

float Furniture::getPrice()  {
    GL3D_UTILS_THROW("Not implemented yet");
    return 0.0f;
}

Merchandise::MerchandiseType Furniture::getType() {
    return MerchandiseType::Furniture;
}

string Furniture::getPickID() {
    return this->pickID;
}

void Furniture::setPickID(string pickID) {
    this->pickID = pickID;
}
