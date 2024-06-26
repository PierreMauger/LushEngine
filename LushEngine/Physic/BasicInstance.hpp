#ifndef BASIC_INSTANCE_HPP
#define BASIC_INSTANCE_HPP

#include "ECS/Component/Component.hpp"
#include "Includes.hpp"

namespace Lush
{
    class BasicInstance
    {
        private:
            std::size_t _id;

        public:
            explicit BasicInstance(std::size_t id) : _id(id)
            {
            }

            virtual ~BasicInstance() = default;

            [[nodiscard]] std::size_t getId() const
            {
                return this->_id;
            }

            static btCollisionShape *initCollider(Transform &transform, Collider &collider)
            {
                if (collider.size == glm::vec3(0.0f))
                    collider.size = transform.scale;
                switch (collider.type) {
                case ColliderType::BOX:
                    return new btBoxShape(btVector3(collider.size.x, collider.size.y, collider.size.z));
                case ColliderType::SPHERE:
                    return new btSphereShape(collider.size.x);
                case ColliderType::CAPSULE:
                    return new btCapsuleShape(collider.size.x, collider.size.y);
                default:
                    return new btBoxShape(btVector3(collider.size.x, collider.size.y, collider.size.z));
                }
            }

            [[nodiscard]] virtual btCollisionObject *getCollisionObject() const = 0;
            virtual void preUpdate(Transform &transform, const Transform &parentTransform = Transform()) = 0;
            virtual void postUpdate(Transform &transform) = 0;

            virtual void addToWorld(btDiscreteDynamicsWorld *world) = 0;
            virtual void removeFromWorld(btDiscreteDynamicsWorld *world) = 0;

            virtual glm::vec3 getVelocity() const = 0;
            virtual void setVelocity(const glm::vec3 &velocity) = 0;
            virtual void addForce(const glm::vec3 &force) = 0;
    };
}

#endif // BASIC_INSTANCE_HPP
