using System;

public class ColliderTest : Entity
{
    public void onInit()
    {
    }

    public void onUpdate(float deltaTime)
    {
    }

    public void onCollisionEnter(ulong otherId)
    {
        this.log("Collision enter with entity : " + otherId, Toast.Info);
        Entity other = new Entity(otherId);
        if (other.hasScriptComponent<Controlable>())
        {
            Controlable controlable = other.getScriptComponent<Controlable>();
            controlable.speed = 0.0f;
        }
        // Transform otherTransform = other.getComponent<Transform>();
        // Vector3 tmpPos = otherTransform.position;
        // tmpPos.y += 5.0f;
        // otherTransform.position = tmpPos;
    }
}
