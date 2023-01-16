using System;
using System.Runtime.CompilerServices;

public class Spin : Entity
{
    private Transform transform;
    public float time;

    public void onInit()
    {
        Console.WriteLine("Important log!");
        this.log("Start spinning!");
        transform = getComponent<Transform>();
    }

    public void onUpdate(float deltaTime)
    {
        time += deltaTime;
        Vector3 tmp = transform.position;
        tmp.x += (float) Math.Cos(time) * 0.05f;
        tmp.z += (float) Math.Sin(time) * 0.05f;
        transform.position = tmp;

        tmp = transform.rotation;
        tmp.y += 1.0f * deltaTime * 50.0f;
        tmp.y %= 360.0f;
        transform.rotation = tmp;
    }
}
