#include "editor/style_package.h"

#include "editor/surface.h"

using namespace std;
using namespace klm::design;
using namespace gl3d;

common_style_package::common_style_package() {
    this->style_name.clear();
    this->room_name_to_ground_res.clear();
    this->room_name_to_wall_res.clear();
}

common_style_package::~common_style_package() {
    return;
}

int common_style_package::apply_style(scheme *sketch) {
    Q_FOREACH(room * rit, *(sketch->get_rooms())) {
            if (this->room_name_to_wall_res.contains(rit->name)) {
                rit->set_material(this->room_name_to_wall_res.value(rit->name));
            }
            if (this->room_name_to_ground_res.contains(rit->name)) {
                rit->ground->setSurfaceMaterial(new Surfacing(this->room_name_to_ground_res.value(rit->name)));
            }
        }

    return 0;
}

bool common_style_package::save_to_style(scheme *sketch) {
    Q_FOREACH(room * rit, *(sketch->get_rooms())) {
            if (rit->name.size() > 0) {
                this->room_name_to_ground_res.insert(rit->name, rit->ground->getSurfaceMaterial()->getID());
                QSet<klm::Surface *> sfcs;
                sfcs.clear();
                rit->get_relate_surfaces(sfcs);
                this->room_name_to_wall_res.insert(rit->name, sfcs.toList().at(0)->getSurfaceMaterial()->getID());
            }
        }

    return true;
}

bool common_style_package::save_to_xml(pugi::xml_node &node) {
    node.append_attribute("type").set_value("common_style_package");
    node.append_attribute("name").set_value(this->style_name.toStdString().c_str());
    pugi::xml_node child = node.append_child("room_name_to_ground_res");
    for (auto sit = room_name_to_ground_res.begin();
            sit != room_name_to_ground_res.end();
            sit++) {
        if (sit.key().size() > 0) {
            child.append_attribute(sit.key().c_str()).set_value(sit.value().c_str());
        }
    }

    child = node.append_child("room_name_to_wall_res");
    for (auto sit = room_name_to_wall_res.begin();
            sit != room_name_to_wall_res.end();
            sit++) {
        if (sit.key().size() > 0) {
            child.append_attribute(sit.key().c_str()).set_value(sit.value().c_str());
        }
    }

    return true;
}

bool common_style_package::load_from_xml(pugi::xml_node node) {
    this->style_name.clear();
    this->room_name_to_ground_res.clear();
    this->room_name_to_wall_res.clear();
    QString type(node.attribute("type").as_string());
    if (type != "common_style_package")
        return false;

    // read from node
    this->style_name = QString(node.attribute("name").as_string());
    pugi::xml_node child = node.child("room_name_to_ground_res");
    for (auto ait = child.attributes_begin();
            ait != child.attributes_end();
            ait++) {
        this->room_name_to_ground_res.insert(string(ait->name()), string(ait->as_string()));
    }
    child = node.child("room_name_to_wall_res");
    for (auto ait = child.attributes_begin();
         ait != child.attributes_end();
         ait++) {
        this->room_name_to_wall_res.insert(string(ait->name()), string(ait->as_string()));
    }

    return true;
}

QString common_style_package::get_style_name() {
    return this->style_name;
}