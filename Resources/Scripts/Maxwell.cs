using System;
using System.Runtime.CompilerServices;

public class Maxwell : Entity
{
    private Transform transform;
    private float time;

    public void onInit()
    {
        transform = getComponent<Transform>();
        time = 0.0f;

        Vector3 tmpRot = transform.rotation;
        tmpRot.x = -45.0f;
        transform.rotation = tmpRot;
    }

    public void onUpdate(float deltaTime)
    {
        time += deltaTime;
        Vector3 tmpRot = transform.rotation;
        Vector3 tmpPos = transform.position;

        tmpRot.x += (float)Math.Sin(time * 8.0f) * 300.0f * deltaTime;
        tmpPos.y += (float)Math.Sin(time * 16.0f) * 2f * deltaTime;

        transform.rotation = tmpRot;
        transform.position = tmpPos;
    }
}
