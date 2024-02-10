#include "Serialization.hpp"

using namespace Lush;

namespace boost::serialization
{
    void serialize(boost::archive::binary_oarchive &ar, std::unordered_map<std::type_index, Component *> &componentMap, [[maybe_unused]] const unsigned int version)
    {
        ar &componentMap.size();
        for (auto &[type, value] : componentMap) {
            std::string typeName = type.name();
            ar &typeName;

            serializeIf<Transform>(typeName, componentMap, ar);
            serializeIf<Model>(typeName, componentMap, ar);
            serializeIf<Camera>(typeName, componentMap, ar);
            serializeIf<Light>(typeName, componentMap, ar);
            serializeIf<Cubemap>(typeName, componentMap, ar);
            serializeIf<Billboard>(typeName, componentMap, ar);
            serializeIf<Map>(typeName, componentMap, ar);
            serializeIf<RigidBody>(typeName, componentMap, ar);
            serializeIf<Collider>(typeName, componentMap, ar);
            serializeIf<CharacterController>(typeName, componentMap, ar);
        }
    }

    void serialize(boost::archive::binary_iarchive &ar, std::unordered_map<std::type_index, Component *> &componentMap, [[maybe_unused]] const unsigned int version)
    {
        std::size_t size;
        ar &size;

        for (std::size_t i = 0; i < size; i++) {
            std::string type;
            ar &type;
            deserializeIf<Transform>(type, componentMap, ar);
            deserializeIf<Model>(type, componentMap, ar);
            deserializeIf<Camera>(type, componentMap, ar);
            deserializeIf<Light>(type, componentMap, ar);
            deserializeIf<Cubemap>(type, componentMap, ar);
            deserializeIf<Billboard>(type, componentMap, ar);
            deserializeIf<Map>(type, componentMap, ar);
            deserializeIf<RigidBody>(type, componentMap, ar);
            deserializeIf<Collider>(type, componentMap, ar);
            deserializeIf<CharacterController>(type, componentMap, ar);
        }
    }
}
