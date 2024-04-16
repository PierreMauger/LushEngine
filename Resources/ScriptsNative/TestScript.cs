using System;

public class TestScript : CustomComponent
{
    public float time = 0.0f;

    public void onInit()
    {
    }

    public void onUpdate(float deltaTime)
    {
        time += deltaTime;
    }

    public void onDestroy()
    {
        this.entity.log("Destroyed");
    }

    public void onCollisionEnter(ulong otherId)
    {
        Entity other = new Entity(otherId);

        if (other.getComponent<Collider>().tag == "player") {
            this.entity.setParent(otherId);
        }
        // this.entity.addComponent("Billboard");
        // this.entity.getComponent<Billboard>().name = "Lush.png";
        // this.entity.delete();
    }
}
