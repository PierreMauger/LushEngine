#include "ECS/System/ASystem.hpp"

using namespace Lush;

ASystem::ASystem(float updateFrequency) : _updateInterval(1.0f / updateFrequency)
{
}

bool ASystem::shouldUpdate(float deltaTime)
{
    this->_timeSinceLastUpdate += deltaTime;
    if (this->_timeSinceLastUpdate >= this->_updateInterval) {
        this->_timeSinceLastUpdate -= this->_updateInterval;
        return true;
    }
    return false;
}

float ASystem::getDeltaTime() const
{
    return this->_updateInterval;
}
