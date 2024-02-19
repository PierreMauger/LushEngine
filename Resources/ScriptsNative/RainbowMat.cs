using System;

public class RainbowMat : CustomComponent
{
    public float time = 0.0f;
    public string materialName;
    private Model model;

    public void onInit()
    {
        model = this.entity.getComponent<Model>();
    }

    public void onUpdate(float deltaTime)
    {
        time += deltaTime;

        Vector3 tmpColor = model.materials[materialName];
        tmpColor.x = MathF.Cos(time) * 0.5f + 0.5f;
        tmpColor.y = MathF.Cos(time + 2) * 0.5f + 0.5f;
        tmpColor.z = MathF.Cos(time + 4) * 0.5f + 0.5f;
        model.materials[materialName] = tmpColor;
    }
}
