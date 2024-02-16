using System;

public class TestScript : CustomComponent
{
    // private Transform transform;
    public float time = 0.0f;
    private Model model;

    public void onInit()
    {
        // transform = this.entity.getComponent<Transform>();
        model = this.entity.getComponent<Model>();
        // colors = model.materials;
    }

    public void onUpdate(float deltaTime)
    {
        time += deltaTime;

        Vector3 tmpColor = model.materials["orangeFur"];
        tmpColor.x = (float)Math.Sin(time) * 0.5f + 0.5f;
        model.materials["orangeFur"] = tmpColor;
        // this.entity.log("Color: " + tmpColor.x, Toast.Warning);
    }
}
