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

        Vector3 up = new Vector3(0.0f, 1.0f, 0.0f);
        Vector3 right = Vector3.cross(camera.forward, up).normalize();

        Quaternion q = new Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
        // movement X
        q = Quaternion.angleAxis(Vector3.degreesToRadians(-mouseX) * sensibility, up);
        camera.forward = (camera.forward * q).normalize();
        // movement Y
        q = Quaternion.angleAxis(Vector3.degreesToRadians(mouseY) * sensibility, right);
        Vector3 forward = (camera.forward * q).normalize();
        if (Vector3.dot(forward, up) < 0.9f && Vector3.dot(forward, up) > -0.9f)
            camera.forward = forward;

        // set transform rotation
        Quaternion rotation = Quaternion.lookAt(camera.forward, up);
        transform.rotation = Vector3.quaternionToEuler(rotation).radiansToDegrees();

        // set camera behind player
        Vector3 tmpPosition = transform.position;
        tmpPosition.x = playerTransform.position.x - camera.forward.x * distance;
        tmpPosition.y = playerTransform.position.y - camera.forward.y * distance;
        tmpPosition.z = playerTransform.position.z - camera.forward.z * distance;
        transform.position = tmpPosition;
    }
}
