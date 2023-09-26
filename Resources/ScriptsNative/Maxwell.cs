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
        tmpRot.x = (float)MathF.Sin(time * 4f) * 15f;
        transform.rotation = tmpRot;
    }
}
