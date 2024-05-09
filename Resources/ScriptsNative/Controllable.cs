using System;

public class Controllable : CustomComponent
{
    private Transform transform;
    private Camera camera;
    public Entity cameraEntity = null;
    public float speed = 10.0f;

    public void onInit()
    {
        transform = this.entity.getComponent<Transform>();
        if (cameraEntity == null)
            cameraEntity = new Entity(InternalCalls.GetEntityFromName("MainCamera"));
        camera = cameraEntity.getComponent<Camera>();
    }

    public void onUpdate(float deltaTime)
    {
        Vector3 tmpCamForward = new Vector3(camera.forward.x, 0.0f, camera.forward.z);

        // Calculate movement direction
        Vector3 moveDirection = new Vector3();
        if (InternalCalls.IsKeyDown(0x57)) // W
            moveDirection += tmpCamForward;
        if (InternalCalls.IsKeyDown(0x41)) // A
            moveDirection += new Vector3(tmpCamForward.z, 0.0f, -tmpCamForward.x);
        if (InternalCalls.IsKeyDown(0x53)) // S
            moveDirection -= tmpCamForward;
        if (InternalCalls.IsKeyDown(0x44)) // D
            moveDirection += new Vector3(-tmpCamForward.z, 0.0f, tmpCamForward.x);
        moveDirection = moveDirection.normalize();

        if (moveDirection != new Vector3() && speed != 0.0f) {
            // Apply movement
            Vector3 tmpPos = transform.position;
            tmpPos += moveDirection * speed * deltaTime;
            transform.position = tmpPos;

            // Align character rotation with movement direction
            Quaternion targetRotation = Quaternion.lookAt(moveDirection, new Vector3(0.0f, 1.0f, 0.0f));
            transform.rotation = Quaternion.slerp(transform.rotation, targetRotation, deltaTime * speed);
        }
    }
}
