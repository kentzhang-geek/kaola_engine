#include "utils//io_utility.h"

using namespace klm;
using namespace std;

const string IOUtility::VALUE_TOKEN = "~";
const string IOUtility::MATRIX = "Matrix";
const string IOUtility::USAGE = "usage";
const string IOUtility::VECTOR = "Vector";
const string IOUtility::SURFACE = "surface";
const string IOUtility::NUM_OF_INDICIES = "noi";
const string IOUtility::INDEX = "index";
const string IOUtility::VERTEX = "Vertex";
const string IOUtility::X = "x";
const string IOUtility::Y = "y";
const string IOUtility::Z = "z";
const string IOUtility::W = "w";
const string IOUtility::H = "h";
const string IOUtility::VISIBLE = "Visible";
const string IOUtility::TRANSFORM_FROM_PPARENT ="tfp";
const string IOUtility::SURFACE_SCALE = "ss";
const string IOUtility::SURFACE_TRANSLATE = "st";
const string IOUtility::SURFACE_ROTATION = "sr";
const string IOUtility::SURFACE_MATEIRAL = "sm";
const string IOUtility::CONNECTIVE_MATERIAL = "cm";
const string IOUtility::LOCAL_VERTIICES = "lv";
const string IOUtility::LOCAL_INDICES = "li";
const string IOUtility::RENDERING_VERTICIES = "rv";
const string IOUtility::RENDERING_INDICES = "ri";
const string IOUtility::CONNECTIVE_VERTICIES = "cv";
const string IOUtility::CONNECTIVE_INDICES = "ci";


const int IOUtility::MATRIX_SIZE = 16;
const int IOUtility::VECTOR_SIZE = 3;

void IOUtility::writeMatrix(pugi::xml_node node, const glm::mat4 &matrix, const string tag){
    pugi::xml_node subNode = node.append_child(MATRIX.c_str());
    writeFloatsToNode(subNode,glm::value_ptr(matrix), MATRIX_SIZE);    
    if(tag.size() != 0){
        subNode.append_attribute(USAGE.c_str()).set_value(tag.c_str());
    }
}

void IOUtility::readMatrix(const pugi::xml_node node, glm::mat4 &matrix){
    QVector<GLfloat> data;
    readFloatsFromNode(node, MATRIX, data);
    if(data.size() == MATRIX_SIZE){
        GLfloat* matrixData = new GLfloat[MATRIX_SIZE];
        for(int index = 0 ; index != MATRIX_SIZE; ++index){
            matrixData[index] = data[index];
        }
        matrix = glm::make_mat4(matrixData);
    }
}

void IOUtility::writeVector(pugi::xml_node node, const glm::vec3 &vector, const string tag){
    pugi::xml_node subNode = node.append_child(VECTOR.c_str());
    writeFloatsToNode(subNode, glm::value_ptr(vector), VECTOR_SIZE);
    if(tag.size() != 0){
        subNode.append_attribute(USAGE.c_str()).set_value(tag.c_str());
    }
}

void IOUtility::readVector(const pugi::xml_node &node, glm::vec3 &vector){
    QVector<GLfloat> data;
    readFloatsFromNode(node, VECTOR, data);
    if(data.size() == VECTOR_SIZE){
        vector.x = data[0];
        vector.y = data[1];
        vector.z = data[2];
    }
}

void IOUtility::writeIndicies(pugi::xml_node &node, const QVector<GLushort> &indicies, const string tag){
    ostringstream oss;

//    for(QVector<GLushort>::const_iterator num = indicies.begin();
//        num != indicies.end(); ++num){
    for(int i = 0; i != indicies.size(); ++i){
        oss<<indicies[i];
        if(i != (indicies.size()-1)){
            oss<<VALUE_TOKEN;
        }
    }
    if(tag.size() != 0){
        node.append_attribute(USAGE.c_str()).set_value(tag.c_str());
    }
    node.append_child(pugi::node_pcdata).set_value(oss.str().c_str());
}

std::string IOUtility::readIndicies(const pugi::xml_node &node, QVector<GLushort> &indicies){
     std::string usage = node.attribute(USAGE.c_str()).value();
     std::string value = node.child_value();
     indicies.clear();

     size_t pos = string::npos;
     do{
         pos = value.find(VALUE_TOKEN);
         indicies.push_back(std::stoi(value.substr(0, pos).c_str()));
         if(string::npos != pos){
             value = value.substr(pos + VALUE_TOKEN.size());
         }
     }while(string::npos != pos);

     return usage;
}

void IOUtility::writeFloatsToNode(pugi::xml_node &node,
                                  const GLfloat* data,
                                  const unsigned int size){
    string strResult = "";
    for(int i = 0; i != size; ++i){
        ostringstream oss;
        oss<<data[i];
        strResult += string(oss.str());
        if(i != (size -1)){
            strResult += VALUE_TOKEN;
        }
    }
    node.append_child(pugi::node_pcdata).set_value(strResult.c_str());
}

void IOUtility::readFloatsFromNode(const pugi::xml_node &node,
                                   const std::string &targetName,
                                   QVector<GLfloat> &ret){
    if(targetName.compare(node.name()) == 0){
        std::string value = node.child_value();
        ret.clear();
        size_t pos = string::npos;
        do{
            pos = value.find(VALUE_TOKEN);
            ret.push_back(std::atof(value.substr(0, pos).c_str()));
            if(string::npos != pos){
                value = value.substr(pos + VALUE_TOKEN.size());
            }
        }while(string::npos != pos);
    }
}
