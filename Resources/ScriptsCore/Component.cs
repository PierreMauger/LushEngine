using System;

public abstract class Component
{
    public Entity entity { get; internal set; }

    protected Component()
    {
        this.entity = new Entity(0);
    }

    public Component(ulong id)
    {
        this.entity = new Entity(id);
    }
}

// wrapper to dissociate from native components
public class CustomComponent : Component
{
}

public class Transform : Component
{
    public Vector3 position {
        get {
            InternalCalls.Transform_GetPosition(entity.id, out Vector3 position);
            return position;
        }
        set {
            InternalCalls.Transform_SetPosition(entity.id, ref value);
        }
    }
    public Vector3 rotation {
        get {
            InternalCalls.Transform_GetRotation(entity.id, out Vector3 rotation);
            return rotation;
        }
        set {
            InternalCalls.Transform_SetRotation(entity.id, ref value);
        }
    }
    public Vector3 scale {
        get {
            InternalCalls.Transform_GetScale(entity.id, out Vector3 scale);
            return scale;
        }
        set {
            InternalCalls.Transform_SetScale(entity.id, ref value);
        }
    }
}

public class Model : Component
{
    public string name {
        get {
            InternalCalls.Model_GetName(entity.id, out string name);
            return name;
        }
        set {
            InternalCalls.Model_SetName(entity.id, value);
        }
    }
}

public class Camera : Component
{
    public Vector3 forward {
        get {
            InternalCalls.Camera_GetForward(entity.id, out Vector3 forward);
            return forward;
        }
        set {
            InternalCalls.Camera_SetForward(entity.id, ref value);
        }
    }
    public float fov {
        get {
            InternalCalls.Camera_GetFov(entity.id, out float fov);
            return fov;
        }
        set {
            InternalCalls.Camera_SetFov(entity.id, value);
        }
    }
    public float near {
        get {
            InternalCalls.Camera_GetNear(entity.id, out float near);
            return near;
        }
        set {
            InternalCalls.Camera_SetNear(entity.id, value);
        }
    }
    public float far {
        get {
            InternalCalls.Camera_GetFar(entity.id, out float far);
            return far;
        }
        set {
            InternalCalls.Camera_SetFar(entity.id, value);
        }
    }
}

public class Light : Component
{
    public float intensity {
        get {
            InternalCalls.Light_GetIntensity(entity.id, out float intensity);
            return intensity;
        }
        set {
            InternalCalls.Light_SetIntensity(entity.id, value);
        }
    }
    public Vector3 color {
        get {
            InternalCalls.Light_GetColor(entity.id, out Vector3 color);
            return color;
        }
        set {
            InternalCalls.Light_SetColor(entity.id, ref value);
        }
    }
}

public class Cubemap : Component
{
    public string name {
        get {
            InternalCalls.Cubemap_GetName(entity.id, out string name);
            return name;
        }
        set {
            InternalCalls.Cubemap_SetName(entity.id, value);
        }
    }
}

public class Billboard : Component
{
    public string name {
        get {
            InternalCalls.Billboard_GetName(entity.id, out string name);
            return name;
        }
        set {
            InternalCalls.Billboard_SetName(entity.id, value);
        }
    }
    public float lockYAxis {
        get {
            InternalCalls.Billboard_GetLockYAxis(entity.id, out float lockYAxis);
            return lockYAxis;
        }
        set {
            InternalCalls.Billboard_SetLockYAxis(entity.id, value);
        }
    }
}

public class Collider : Component
{
    public string tag {
        get {
            InternalCalls.Collider_GetTag(entity.id, out string tag);
            return tag;
        }
        set {
            InternalCalls.Collider_SetTag(entity.id, value);
        }
    }
}
