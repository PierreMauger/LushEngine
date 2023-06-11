using System;

public class Maxwell : Entity
{
    private Transform transform;
    public float time;
    public float time2;

    public void onInit()
    {
        transform = getComponent<Transform>();
        time = 5.0f;
        this.log("hello", Toast.Success);
    }

    public void onUpdate(float deltaTime)
    {
        time += deltaTime;
        Vector3 tmpRot = transform.rotation;
        Vector3 tmpPos = transform.position;

        float sin = (float)(Math.Asin(Math.Sin(time * 8.0f + Math.PI / 2.0f)));
        sin = sin > 0.5f || sin < -0.4f ? sin : 0.0f;

        float sinBis = (float)(Math.Asin(Math.Sin(time * 16.0f)));
        sinBis = sinBis > 0.5f || sinBis < -0.5f ? sinBis : 0.0f;

        tmpRot.x += sin * 180.0f * deltaTime;
        tmpPos.y += sinBis * 4.0f * deltaTime;

        transform.rotation = tmpRot;
        transform.position = tmpPos;
    }
}
