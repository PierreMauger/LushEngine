using System;

public class TestScript : CustomComponent
{
    private Transform transform;
    public float time = 5.0f;

    public void onInit()
    {
        transform = this.entity.getComponent<Transform>();
    }

    public void onUpdate(float deltaTime)
    {
        time += deltaTime;

        Vector3 tmpRot = transform.rotation;
        tmpRot.y = time % 360.0f;
        transform.rotation = tmpRot;
    }
}
