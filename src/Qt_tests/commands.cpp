#include "commands.h"

InsertCommand::InsertCommand(gl3d::gl3d_wall* theModel):QUndoCommand() {
    this->mModel = theModel;
}

InsertCommand::~InsertCommand() {
}

void InsertCommand::undo() {
    mModels->removeAt(mModels->length());

}

void InsertCommand::redo() {
    mModels->push_back(this->mModel);
}
