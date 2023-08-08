using System;

public class Controlable : Entity
{
    private Transform transform;
    private Camera camera;
    public Entity cameraEntity = null;
    public float speed = 10.0f;

    public void onInit()
    {
        transform = getComponent<Transform>();
        if (cameraEntity == null) {
            Console.WriteLine("No camera entity specified, using default camera (1).");
            cameraEntity = new Entity(1);
        }
        camera = cameraEntity.getComponent<Camera>();
    }

    public void onUpdate(float deltaTime)
    {
        Vector3 tmpCamForward = camera.forward;
        float tmpCamForwardLength = (float)Math.Sqrt(tmpCamForward.x * tmpCamForward.x + tmpCamForward.z * tmpCamForward.z);
        tmpCamForward.x /= tmpCamForwardLength;
        tmpCamForward.z /= tmpCamForwardLength;

        Vector3 tmpRot = transform.rotation;
        tmpRot.y = (float)(-Math.Atan2(tmpCamForward.z, tmpCamForward.x) * 180.0f / Math.PI);
        transform.rotation = tmpRot;

        if (InternalCalls.IsKeyDown(0x57)) {
            Vector3 tmpPos = transform.position;
            tmpPos.x += tmpCamForward.x * speed * deltaTime;
            tmpPos.z += tmpCamForward.z * speed * deltaTime;
            transform.position = tmpPos;
        }
    }

    public void onCollisionStay(Entity other)
    {
        // Console.WriteLine("Collision with " + other.id);
    }
}
