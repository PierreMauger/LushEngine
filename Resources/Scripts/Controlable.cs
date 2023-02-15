using System;
using System.Runtime.CompilerServices;

public class Controlable : Entity
{
    private Transform transform;
    private Camera camera;
    private Entity cameraEntity;
    public ulong cameraEntityId;

    public void onInit()
    {
        transform = getComponent<Transform>();
        cameraEntityId = 1;
        cameraEntity = new Entity(cameraEntityId);
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
