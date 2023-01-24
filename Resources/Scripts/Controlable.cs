using System;
using System.Runtime.CompilerServices;

public class Controlable : Entity
{
    private Transform transform;
    private Entity cameraEntity;
    private Camera camera;

    public void onInit()
    {
        transform = getComponent<Transform>();
        cameraEntity = new Entity(1);
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
            tmpPos.x += tmpCamForward.x * 10.0f * deltaTime;
            tmpPos.z += tmpCamForward.z * 10.0f * deltaTime;
            transform.position = tmpPos;
        }
    }
}
