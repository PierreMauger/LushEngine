#ifndef BONE_HPP
#define BONE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <assimp/scene.h>
#include <glm/gtx/quaternion.hpp>

#include "Includes.hpp"

namespace Lush
{
    typedef struct {
        glm::vec3 position;
        float timeStamp;
    } KeyPosition;

    typedef struct {
        glm::quat orientation;
        float timeStamp;
    } KeyRotation;

    typedef struct {
        glm::vec3 scale;
        float timeStamp;
    } KeyScale;

    class Bone
    {
        private:
            std::vector<KeyPosition> _positions;
            std::vector<KeyRotation> _rotations;
            std::vector<KeyScale> _scales;
            int _numPositions;
            int _numRotations;
            int _numScalings;

            std::string _name;
            glm::mat4 _localTransform;
            int _ID;

        public:
            Bone(const std::string &name, int ID, const aiNodeAnim &channel);
            ~Bone() = default;

            void update(float animationTime);

            glm::mat4 getLocalTransform() const;
            std::string getName() const;
            int getID() const;

            int getPosIndex(float animationTime) const;
            int getRotationIndex(float animationTime) const;
            int getScaleIndex(float animationTime) const;
            float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const;
            glm::mat4 InterpolatePosition(float animationTime) const;
            glm::mat4 InterpolateRotation(float animationTime) const;
            glm::mat4 InterpolateScale(float animationTime) const;
    };
}

#endif // BONE_HPP
