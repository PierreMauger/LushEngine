using System;
using System.Runtime.CompilerServices;

public class ThirdPersonCamera : Entity
{
    private Transform transform;
    private Camera camera;
    private Transform playerTransform;
    public Entity playerEntity = null;
    public float distance = 10.0f;


    public void onInit()
    {
        transform = getComponent<Transform>();
        camera = getComponent<Camera>();
        if (playerEntity == null) {
            playerEntity = new Entity(1);
        }
        playerTransform = playerEntity.getComponent<Transform>();
    }

    public void onUpdate(float deltaTime)
    {
        Vector3 tmpPosition = transform.position;
        // tmpPosition.x = playerTransform.position.x - camera.forward.x * distance;
        // tmpPosition.y = playerTransform.position.y - camera.forward.y * distance;
        // tmpPosition.z = playerTransform.position.z - camera.forward.z * distance;
        transform.position = tmpPosition;
    }
}
