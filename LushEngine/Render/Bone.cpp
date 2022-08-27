#include "Bone.hpp"

using namespace Lush;

Bone::Bone(const std::string &name, int ID, const aiNodeAnim &channel)
{
    this->_name = name;
    this->_ID = ID;
    this->_localTransform = glm::mat4(1.0f);

    this->_numPositions = channel.mNumPositionKeys;
    for (int i = 0; i < this->_numPositions; i++) {
        KeyPosition key;
        key.position = glm::vec3(channel.mPositionKeys[i].mValue.x, channel.mPositionKeys[i].mValue.y, channel.mPositionKeys[i].mValue.z);
        key.timeStamp = channel.mPositionKeys[i].mTime;
        this->_positions.push_back(key);
    }

    this->_numRotations = channel.mNumRotationKeys;
    for (int i = 0; i < this->_numRotations; i++) {
        KeyRotation key;
        key.orientation = glm::quat(channel.mRotationKeys[i].mValue.w, channel.mRotationKeys[i].mValue.x, channel.mRotationKeys[i].mValue.y, channel.mRotationKeys[i].mValue.z);
        key.timeStamp = channel.mRotationKeys[i].mTime;
        this->_rotations.push_back(key);
    }

    this->_numScalings = channel.mNumScalingKeys;
    for (int i = 0; i < this->_numScalings; i++) {
        KeyScale key;
        key.scale = glm::vec3(channel.mScalingKeys[i].mValue.x, channel.mScalingKeys[i].mValue.y, channel.mScalingKeys[i].mValue.z);
        key.timeStamp = channel.mScalingKeys[i].mTime;
        this->_scales.push_back(key);
    }
}

void Bone::update(float animationTime)
{
    glm::mat4 translation = this->InterpolatePosition(animationTime);
    glm::mat4 rotation = this->InterpolateRotation(animationTime);
    glm::mat4 scale = this->InterpolateScale(animationTime);
    this->_localTransform = translation * rotation * scale;
}

glm::mat4 Bone::getLocalTransform() const
{
    return this->_localTransform;
}

std::string Bone::getName() const
{
    return this->_name;
}

int Bone::getID() const
{
    return this->_ID;
}

int Bone::getPosIndex(float animationTime) const
{
    for (int index = 0; index < this->_numPositions; index++)
        if (animationTime < this->_positions[index + 1].timeStamp)
            return index;
    return 0;
}

int Bone::getRotationIndex(float animationTime) const
{
    for (int index = 0; index < this->_numRotations; index++)
        if (animationTime < this->_rotations[index + 1].timeStamp)
            return index;
    return 0;
}

int Bone::getScaleIndex(float animationTime) const
{
    for (int index = 0; index < this->_numScalings; index++)
        if (animationTime < this->_scales[index + 1].timeStamp)
            return index;
    return 0;
}

float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const
{
    float factor = (animationTime - lastTimeStamp) / (nextTimeStamp - lastTimeStamp);

    return factor;
}

glm::mat4 Bone::InterpolatePosition(float animationTime) const
{
    if (this->_numPositions == 1)
        return glm::translate(glm::mat4(1.0f), this->_positions[0].position);

    int positionIndex = this->getPosIndex(animationTime);
    int nextPositionIndex = positionIndex + 1;
    float factor = this->getScaleFactor(this->_positions[positionIndex].timeStamp, this->_positions[nextPositionIndex].timeStamp, animationTime);
    glm::vec3 position = glm::mix(this->_positions[positionIndex].position, this->_positions[nextPositionIndex].position, factor);
    return glm::translate(glm::mat4(1.0f), position);
}

glm::mat4 Bone::InterpolateRotation(float animationTime) const
{
    if (this->_numRotations == 1) {
        auto rotation = glm::normalize(this->_rotations[0].orientation);
        return glm::mat4_cast(rotation);
    }

    int rotationIndex = this->getRotationIndex(animationTime);
    int nextRotationIndex = rotationIndex + 1;
    float factor = this->getScaleFactor(this->_rotations[rotationIndex].timeStamp, this->_rotations[nextRotationIndex].timeStamp, animationTime);
    glm::quat rotation = glm::slerp(this->_rotations[rotationIndex].orientation, this->_rotations[nextRotationIndex].orientation, factor);
    rotation = glm::normalize(rotation);
    return glm::mat4_cast(rotation);
}

glm::mat4 Bone::InterpolateScale(float animationTime) const
{
    if (this->_numScalings == 1)
        return glm::scale(glm::mat4(1.0f), this->_scales[0].scale);

    int scaleIndex = this->getScaleIndex(animationTime);
    int nextScaleIndex = scaleIndex + 1;
    float factor = this->getScaleFactor(this->_scales[scaleIndex].timeStamp, this->_scales[nextScaleIndex].timeStamp, animationTime);
    glm::vec3 scale = glm::mix(this->_scales[scaleIndex].scale, this->_scales[nextScaleIndex].scale, factor);
    return glm::scale(glm::mat4(1.0f), scale);
}
