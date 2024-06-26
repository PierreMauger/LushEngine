#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include "ECS/Component/Component.hpp"
#include "Includes.hpp"

namespace boost::serialization
{
    template <class Archive> void serialize(Archive &ar, glm::vec2 &v, [[maybe_unused]] const unsigned int version)
    {
        ar &v.x;
        ar &v.y;
    }

    template <class Archive> void serialize(Archive &ar, glm::vec3 &v, [[maybe_unused]] const unsigned int version)
    {
        ar &v.x;
        ar &v.y;
        ar &v.z;
    }


    template <class Archive> void serialize(Archive &ar, glm::vec4 &v, [[maybe_unused]] const unsigned int version)
    {
        ar &v.x;
        ar &v.y;
        ar &v.z;
        ar &v.w;
    }

    template <class Archive> void serialize(Archive &ar, glm::quat &q, [[maybe_unused]] const unsigned int version)
    {
        ar &q.x;
        ar &q.y;
        ar &q.z;
        ar &q.w;
    }

    template <class Archive> void serialize(Archive &ar, glm::mat4 &mat, [[maybe_unused]] const unsigned int version)
    {
        ar &mat[0][0];
        ar &mat[0][1];
        ar &mat[0][2];
        ar &mat[0][3];
        ar &mat[1][0];
        ar &mat[1][1];
        ar &mat[1][2];
        ar &mat[1][3];
        ar &mat[2][0];
        ar &mat[2][1];
        ar &mat[2][2];
        ar &mat[2][3];
        ar &mat[3][0];
        ar &mat[3][1];
        ar &mat[3][2];
        ar &mat[3][3];
    }

    template <typename T> void serialize(boost::archive::binary_oarchive &ar, std::vector<T> &vec, [[maybe_unused]] const unsigned int version)
    {
        ar &vec.size();

        for (auto &elem : vec) {
            ar &elem;
        }
    }

    template <typename T> void serialize(boost::archive::binary_iarchive &ar, std::vector<T> &vec, [[maybe_unused]] const unsigned int version)
    {
        std::size_t size;
        ar &size;

        for (std::size_t i = 0; i < size; i++) {
            T elem{};
            ar &elem;
            vec.push_back(elem);
        }
    }

    template <class Archive, typename T> void serialize(Archive &ar, std::map<std::string, T> &map, [[maybe_unused]] const unsigned int version)
    {
        std::size_t size = map.size();
        ar &size;
        for (auto &[name, value] : map) {
            ar &boost::serialization::make_nvp(name.c_str(), value);
        }
    }

    template <typename T> void serialize(boost::archive::binary_oarchive &ar, std::unordered_map<std::string, T> &map, [[maybe_unused]] const unsigned int version)
    {
        std::size_t size = map.size();
        ar &size;
        for (auto &[name, value] : map) {
            ar &name;
            ar &value;
        }
    }

    template <typename T> void serialize(boost::archive::binary_iarchive &ar, std::unordered_map<std::string, T> &map, [[maybe_unused]] const unsigned int version)
    {
        std::size_t size;
        ar &size;

        for (std::size_t i = 0; i < size; i++) {
            std::string name;
            T elem{};
            ar &name;
            ar &elem;
            map[name] = elem;
        }
    }

    void serialize(boost::archive::binary_oarchive &ar, std::unordered_map<std::type_index, Lush::Component *> &map, [[maybe_unused]] const unsigned int version);
    void serialize(boost::archive::binary_iarchive &ar, std::unordered_map<std::type_index, Lush::Component *> &map, [[maybe_unused]] const unsigned int version);

    template <typename T> void deserializeIf(std::string &type, std::unordered_map<std::type_index, Lush::Component *> &componentMap, boost::archive::binary_iarchive &ar)
    {
        if (type == typeid(T).name()) {
            T elem{};
            ar &elem;
            componentMap[typeid(T)] = new T(elem);
        }
    }

    template <typename T> void serializeIf(std::string &type, std::unordered_map<std::type_index, Lush::Component *> &componentMap, boost::archive::binary_oarchive &ar)
    {
        if (type == typeid(T).name()) {
            ar &*static_cast<T *>(componentMap[typeid(T)]);
        }
    }
}

#endif // SERIALIZATION_HPP
