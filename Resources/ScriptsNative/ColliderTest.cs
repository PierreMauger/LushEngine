using System;

public class ColliderTest : Entity
{
    private Transform transform;

    public void onInit()
    {
        transform = getComponent<Transform>();
    }

    public void onUpdate(float deltaTime)
    {
        Vector3 tmpPos = transform.position;
        // tmpPos.y = (float)(Math.Sin(InternalCalls.GetTime()) * 2.0f);
        // transform.position = tmpPos;
    }

    public void onCollisionEnter(ulong otherId)
    {
        this.log("Collision enter", Toast.Info);
        // Console.WriteLine("Collision with " + otherId);
    }
}
