using System;

public class ColliderTest : CustomComponent
{
    public void onInit()
    {
    }

    public void onUpdate(float deltaTime)
    {
    }

    public void onCollisionEnter(ulong otherId)
    {
        Entity other = new Entity(otherId);
        if (other.hasComponent<Controllable>())
        {
            this.entity.log("Collision with Controllable", Toast.Warning);
            Controllable controllable = other.getComponent<Controllable>();
            controllable.speed = 0.0f;
        }
    }
}
