// the proper way would be to compile the engine as a static library
// but this is easier

#include <async.cpp>
#include <audio.cpp>
#include <core.cpp>
#include <gui.cpp>
#include <menu.cpp>
#include <navigation.cpp>
#include <ui.cpp>
#include <worldcell.cpp>

#include <components/controllercomponent.cpp>
#include <components/entitycomponents.cpp>
#include <components/rendercomponent.cpp>
#include <components/armaturecomponent.cpp>
#include <components/spritecomponent.cpp>
#include <components/particlecomponent.cpp>

#include <entities/crate.cpp>
#include <entities/entity.cpp>
#include <entities/player.cpp>
#include <entities/lamp.cpp>
#include <entities/staticworldobject.cpp>

#include <physics/collisionmodel.cpp>
#include <physics/physics.cpp>

#include <render/armature.cpp>
#include <render/armaturecomponent.cpp>
#include <render/material.cpp>
#include <render/model.cpp>
#include <render/opengl_renderer.cpp>
#include <render/opengl_shader.cpp>
#include <render/render.cpp>
#include <render/render_misc.cpp>
#include <render/sprite.cpp>