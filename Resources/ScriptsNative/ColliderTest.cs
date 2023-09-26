using System;

public class ColliderTest : CustomComponent
{
    public string name = "Green";

    public void onInit()
    {
        this.entity.log(name, Toast.Warning);
    }

    public void onUpdate(float deltaTime)
    {
    }

    public void onCollisionEnter(ulong otherId)
    {
        Entity other = new Entity(otherId);

        if (other.getComponent<Collider>().tag == "ennemy") {
            this.entity.log("Collision with Ennemy", Toast.Warning);
            other.getComponent<Collider>().tag = "dead";
        }

        if (other.hasComponent<Controllable>()) {
            this.entity.log("Collision with Controllable " + other.getName(), Toast.Warning);
            other.getComponent<Controllable>().speed = 5.0f;
        }
    }
}
