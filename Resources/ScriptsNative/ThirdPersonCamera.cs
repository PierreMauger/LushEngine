using System;

public class ThirdPersonCamera : CustomComponent
{
    private Transform transform;
    private Camera camera;
    private Transform playerTransform;
    public Entity playerEntity = null;
    public float distance = 10.0f;
    public float sensibility = 0.1f;

    public void onInit()
    {
        transform = this.entity.getComponent<Transform>();
        camera = this.entity.getComponent<Camera>();
        if (playerEntity == null) {
            playerEntity = new Entity(1);
        }
        playerTransform = playerEntity.getComponent<Transform>();
    }

    public void onUpdate(float deltaTime)
    {
        // rotate camera around player
        float mouseX = InternalCalls.GetMouseMovementX();
        float mouseY = InternalCalls.GetMouseMovementY();

        Vector3 up = Vector3.Up;
        Vector3 right = Vector3.Cross(camera.forward, up).Normalize();

        // movement X
        Quaternion horizontalRotation = Quaternion.AngleAxis(Vector3.DegreesToRadians(-mouseX) * sensibility, up);
        camera.forward = (camera.forward * horizontalRotation).Normalize();
        // movement Y
        Quaternion verticalRotation = Quaternion.AngleAxis(Vector3.DegreesToRadians(mouseY) * sensibility, right);
        Vector3 forward = (camera.forward * verticalRotation).Normalize();
        float dotProduct = Vector3.Dot(forward, up);
        if (dotProduct < 0.9f && dotProduct > -0.9f)
            camera.forward = forward;

        // set camera behind player
        Vector3 tmpPosition = transform.position;
        tmpPosition = Vector3.Lerp(tmpPosition, playerTransform.position - camera.forward * distance, 1.0f);
        transform.position = tmpPosition;
    }
}
