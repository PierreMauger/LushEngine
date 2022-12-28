using System;
using System.Runtime.CompilerServices;

public class Spin : Entity
{
    private Transform transform;

    public void onInit()
    {
        Console.WriteLine("Wow!");
        transform = getComponent<Transform>();
    }

    public void onUpdate(float deltaTime)
    {
        Vector3 tmp = transform.position;
        tmp.x += (float) Math.Cos(deltaTime) * 0.05f;
        tmp.z += (float) Math.Sin(deltaTime) * 0.05f;
        transform.position = tmp;
    }
}
