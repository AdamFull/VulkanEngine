#pragma once
#include "chaiscript/chaiscript.hpp"
#include "util/Transform.hpp"
#include "graphics/scene/objects/RenderObject.h"

namespace engine
{
    namespace scripting
    {
        namespace modules
        {
            static std::pair<const std::string, chaiscript::ModulePtr> engine_structs()
            {
                auto pModule = chaiscript::ModulePtr(new chaiscript::Module());

                pModule->add(chaiscript::user_type<FTransform>(), "FTransform");
                pModule->add(chaiscript::constructor<FTransform()>(), "FTransform");

                pModule->add(chaiscript::fun(&FTransform::pos), "pos");
                pModule->add(chaiscript::fun(&FTransform::rot), "rot");
                pModule->add(chaiscript::fun(&FTransform::scale), "scale");

                pModule->add(chaiscript::fun(static_cast<FTransform& (FTransform::*)(const FTransform&)>(&FTransform::operator+=)), "+=");

                pModule->add(chaiscript::fun(&FTransform::getPosition), "getPosition");
                pModule->add(chaiscript::fun(&FTransform::setPosition), "setPosition");
                pModule->add(chaiscript::fun(&FTransform::getRotation), "getRotation");
                pModule->add(chaiscript::fun(&FTransform::setRotation), "setRotation");
                pModule->add(chaiscript::fun(&FTransform::getScale), "getScale");
                pModule->add(chaiscript::fun(&FTransform::setScale), "setScale");

                pModule->add(chaiscript::fun(&FTransform::getModel), "getModel");

                return std::make_pair("engine_structs", pModule);;
            };

            static std::pair<const std::string, chaiscript::ModulePtr> engine_objects()
            {
                auto pModule = chaiscript::ModulePtr(new chaiscript::Module());

                using namespace engine::core::scene;
                pModule->add(chaiscript::user_type<CRenderObject>(), "CRenderObject");
                pModule->add(chaiscript::constructor<CRenderObject()>(), "CRenderObject");
                pModule->add(chaiscript::constructor<CRenderObject(const std::string&)>(), "CRenderObject");
                pModule->add(chaiscript::fun(&CRenderObject::create), "create");
                pModule->add(chaiscript::fun(&CRenderObject::reCreate), "reCreate");
                //pModule->add(chaiscript::fun(&CRenderObject::render), "render");
                pModule->add(chaiscript::fun(&CRenderObject::update), "update");

                pModule->add(chaiscript::fun(&CRenderObject::setVisible), "setVisible");
                pModule->add(chaiscript::fun(&CRenderObject::isVisible), "isVisible");
                pModule->add(chaiscript::fun(&CRenderObject::setEnable), "setEnable");
                pModule->add(chaiscript::fun(&CRenderObject::isEnabled), "isEnabled");

                pModule->add(chaiscript::fun(static_cast<utl::ref_ptr<CRenderObject>& (CRenderObject::*)(const std::string&)>(&CRenderObject::find)), "find");
                pModule->add(chaiscript::fun(static_cast<utl::ref_ptr<CRenderObject>& (CRenderObject::*)(uint64_t)>(&CRenderObject::find)), "find");

                pModule->add(chaiscript::fun(&CRenderObject::addChild), "addChild");
                pModule->add(chaiscript::fun(&CRenderObject::setParent), "setParent");
                pModule->add(chaiscript::fun(&CRenderObject::attach), "attach");
                pModule->add(chaiscript::fun(&CRenderObject::detach), "detach");

                pModule->add(chaiscript::fun(&CRenderObject::getId), "getId");
                pModule->add(chaiscript::fun(&CRenderObject::getName), "getName");
                pModule->add(chaiscript::fun(&CRenderObject::getParent), "getParent");
                pModule->add(chaiscript::fun(&CRenderObject::getChilds), "getChilds");

                pModule->add(chaiscript::fun(&CRenderObject::getTransform), "getTransform");
                pModule->add(chaiscript::fun(&CRenderObject::getLocalTransform), "getLocalTransform");
                pModule->add(chaiscript::fun(&CRenderObject::getModel), "getModel");
                pModule->add(chaiscript::fun(&CRenderObject::getModelOld), "getModelOld");
                pModule->add(chaiscript::fun(&CRenderObject::getPosition), "getPosition");
                pModule->add(chaiscript::fun(&CRenderObject::getLocalPosition), "getLocalPosition");
                pModule->add(chaiscript::fun(&CRenderObject::getRotation), "getRotation");
                pModule->add(chaiscript::fun(&CRenderObject::getLocalRotation), "getLocalRotation");
                pModule->add(chaiscript::fun(&CRenderObject::getScale), "getScale");
                pModule->add(chaiscript::fun(&CRenderObject::getLocalScale), "getLocalScale");

                pModule->add(chaiscript::fun(&CRenderObject::setName), "setName");

                pModule->add(chaiscript::fun(&CRenderObject::setTransform), "setTransform");
                pModule->add(chaiscript::fun(&CRenderObject::setPosition), "setPosition");
                pModule->add(chaiscript::fun(&CRenderObject::setRotation), "setRotation");
                pModule->add(chaiscript::fun(&CRenderObject::setScale), "setScale");

                pModule->add(chaiscript::fun(&CRenderObject::setIndex), "setIndex");
                pModule->add(chaiscript::fun(&CRenderObject::getIndex), "getIndex");

                //pModule->add(chaiscript::fun(&CRenderObject::setMesh), "setMesh");
                //pModule->add(chaiscript::fun(&CRenderObject::setCamera), "setCamera");
                //pModule->add(chaiscript::fun(&CRenderObject::setLight), "setLight");
                //pModule->add(chaiscript::fun(&CRenderObject::setScript), "setScript");

                //pModule->add(chaiscript::fun(&CRenderObject::getMesh), "getMesh");
                //pModule->add(chaiscript::fun(&CRenderObject::getCamera), "getCamera");
                //pModule->add(chaiscript::fun(&CRenderObject::getLight), "getLight");
                //pModule->add(chaiscript::fun(&CRenderObject::getScript), "getScript");

                return std::make_pair("engine_objects", pModule);;
            }
        }
    }
}