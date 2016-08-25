#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>

#include "kaola_engine/gl3d.hpp"
#include "kaola_engine/kaola_engine.h"
#include "kaola_engine/model_manager.hpp"
#include "kaola_engine/gl3d_render_process.hpp"
#include "kaola_engine/gl3d_obj_authority.h"
#include <QThread>

#include "editor/gl3d_wall.h"
#include "utils/gl3d_global_param.h"
#include "utils/gl3d_path_config.h"
#include "resource_and_network/klm_resource_manager.h"

#include "QVector"

class InsertCommand : public QUndoCommand {
public:
    InsertCommand(gl3d::gl3d_wall* theModel);
    ~InsertCommand();

public:
    virtual void undo();
    virtual void redo();

private:
    QVector<gl3d::gl3d_wall>* mModels;
};

#endif // COMMANDS_H
