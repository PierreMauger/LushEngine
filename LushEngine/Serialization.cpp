#include "Serialization.hpp"

using namespace Lush;

namespace boost::serialization
{
    void serialize(boost::archive::binary_oarchive &ar, std::unordered_map<std::type_index, Component *> &map, [[maybe_unused]] const unsigned int version)
    {
        ar &map.size();
        for (auto &[type, value] : map) {
            std::string type_name = type.name();
            ar &type_name;

            if (type == typeid(Transform)) {
                ar &*static_cast<Transform *>(value);
            } else if (type == typeid(Model)) {
                ar &*static_cast<Model *>(value);
            } else if (type == typeid(Camera)) {
                ar &*static_cast<Camera *>(value);
            } else if (type == typeid(Light)) {
                ar &*static_cast<Light *>(value);
            } else if (type == typeid(Cubemap)) {
                ar &*static_cast<Cubemap *>(value);
            } else if (type == typeid(Billboard)) {
                ar &*static_cast<Billboard *>(value);
            } else if (type == typeid(Map)) {
                ar &*static_cast<Map *>(value);
            } else if (type == typeid(RigidBody)) {
                ar &*static_cast<RigidBody *>(value);
            } else if (type == typeid(CharacterController)) {
                ar &*static_cast<CharacterController *>(value);
            }
        }
    }

    void serialize(boost::archive::binary_iarchive &ar, std::unordered_map<std::type_index, Component *> &map, [[maybe_unused]] const unsigned int version)
    {
        size_t size;
        ar &size;

        for (size_t i = 0; i < size; i++) {
            std::string type;
            ar &type;
            if (type == typeid(Transform).name()) {
                Transform elem{};
                ar &elem;
                map[typeid(Transform)] = new Transform(elem);
            } else if (type == typeid(Model).name()) {
                Model elem{};
                ar &elem;
                map[typeid(Model)] = new Model(elem);
            } else if (type == typeid(Camera).name()) {
                Camera elem{};
                ar &elem;
                map[typeid(Camera)] = new Camera(elem);
            } else if (type == typeid(Light).name()) {
                Light elem{};
                ar &elem;
                map[typeid(Light)] = new Light(elem);
            } else if (type == typeid(Cubemap).name()) {
                Cubemap elem{};
                ar &elem;
                map[typeid(Cubemap)] = new Cubemap(elem);
            } else if (type == typeid(Billboard).name()) {
                Billboard elem{};
                ar &elem;
                map[typeid(Billboard)] = new Billboard(elem);
            } else if (type == typeid(Map).name()) {
                Map elem{};
                ar &elem;
                map[typeid(Map)] = new Map(elem);
            } else if (type == typeid(RigidBody).name()) {
                RigidBody elem{};
                ar &elem;
                map[typeid(RigidBody)] = new RigidBody(elem);
            } else if (type == typeid(CharacterController).name()) {
                CharacterController elem{};
                ar &elem;
                map[typeid(CharacterController)] = new CharacterController(elem);
            }
        }
    }
}
