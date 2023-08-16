using System;

public class Maxwell : CustomComponent
{
    private Transform transform;
    public float time;

    public void onInit()
    {
        transform = this.entity.getComponent<Transform>();
        time = 0.0f;
    }

    public void onUpdate(float deltaTime)
    {
        time += deltaTime;
        Vector3 tmpRot = transform.rotation;
        Vector3 tmpPos = transform.position;

        float rot = MathF.Pow(MathF.Asin(MathF.Sin(time * 4)), 2) * MathF.Sin(time * 4);
        tmpRot.x = rot * 15.0f;

        transform.rotation = tmpRot;
        transform.position = tmpPos;
    }
}
