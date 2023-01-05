using System;
using System.Runtime.CompilerServices;

public class Spin : Entity
{
    private Transform transform;
    private float time;

    public void onInit()
    {
        Console.WriteLine("Wow!");
        transform = getComponent<Transform>();
    }

    public void onUpdate(float deltaTime)
    {
        time += deltaTime;
        Vector3 tmp = transform.position;
        tmp.x += (float) Math.Cos(time) * 0.05f;
        tmp.z += (float) Math.Sin(time) * 0.05f;
        transform.position = tmp;
    }
}
