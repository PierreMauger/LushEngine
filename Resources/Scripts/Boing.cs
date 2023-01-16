using System;
using System.Runtime.CompilerServices;

public class Boing : Entity
{
    private Transform transform;
    public float time;

    public void onInit()
    {
        this.log("Start Jumping!");
        transform = getComponent<Transform>();
    }

    public void onUpdate(float deltaTime)
    {
        time += deltaTime;
        Vector3 tmp = transform.position;
        tmp.y += (float) Math.Cos(time) * 0.05f;
        transform.position = tmp;
    }
}
