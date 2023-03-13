using System;
using System.Runtime.CompilerServices;

public class Test : Entity
{
    private Transform transform;
    // public Entity maxEntity = new Entity(0);
    // private Maxwell maxwell;
    public float time;

    public void onInit()
    {
        transform = getComponent<Transform>();
        time = 0.0f;
        this.log("hello");
    }

    public void onUpdate(float deltaTime)
    {
        // maxwell = maxEntity.getScriptComponent<Maxwell>();
        // time = maxwell.time;
        time += deltaTime;

        Vector3 tmp = transform.rotation;
        tmp.y += 1.0f * deltaTime * 50.0f;
        tmp.y %= 360.0f;
        transform.rotation = tmp;
    }
}
