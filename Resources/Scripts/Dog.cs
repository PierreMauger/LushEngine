using System;
using System.Runtime.CompilerServices;

public class Dog
{
    public Transform transform;

    public void onInit()
    {
        Console.WriteLine("woof!");
        transform = new Transform();
    }

    public void onUpdate(float deltaTime)
    {
        Vector3 tmp = transform.position;
        tmp.x += (float) Math.Cos(deltaTime) * 0.05f;
        tmp.z += (float) Math.Sin(deltaTime) * 0.05f;
        transform.position = tmp;
    }
}
