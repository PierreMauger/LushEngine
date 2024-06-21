using System;

public class Maxwell : CustomComponent
{
    // private Transform transform;
    public float time;
    // private Vector3 startPos;

    public void onInit()
    {
        // transform = this.entity.getComponent<Transform>();
        // startPos = transform.position;
        // time = 0.0f;
    }

    public void onUpdate(float deltaTime)
    {
        time += deltaTime;

        // Vector3 tmpPos = transform.position;
        // tmpPos.x = startPos.x + MathF.Sin(time / 4) * 4;
        // transform.position = tmpPos;

        if (InternalCalls.IsKeyDown(0x20)) {
            this.entity.log("Space key pressed", Toast.Info);
            this.entity.addComponent("RigidBody");
        }
        // Vector3 tmpRot = transform.rotation;
        // tmpRot.x = MathF.Sin(time * 4f) * 15f;
        // transform.rotation = tmpRot;
    }
}
