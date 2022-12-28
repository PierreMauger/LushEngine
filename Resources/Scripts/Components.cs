using System;

public struct Vector3
{
    public float x;
    public float y;
    public float z;
}

public abstract class Component
{
    public Entity entity { get; internal set; }
}

public class Transform : Component
{
    public Vector3 position {
        get {
            InternalCalls.Transform_Get(entity.id, out Vector3 position);
            return position;
        }
        set {
            InternalCalls.Transform_Set(entity.id, ref value);
        }
    }
    // public Vector3 rotation;
    // public Vector3 scale;
}
