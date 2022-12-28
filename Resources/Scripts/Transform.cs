using System;

public struct Vector3
{
    public float x;
    public float y;
    public float z;
}

public class Transform
{
    public Vector3 position {
        get {
            InternalCalls.Transform_Get(0, out Vector3 position);
            return position;
        }
        set {
            InternalCalls.Transform_Set(0, ref value);
        }
    }
    // public Vector3 rotation;
    // public Vector3 scale;
}
