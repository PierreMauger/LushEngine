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
}

public class Collider : Component
{
    public string tag {
        get {
            InternalCalls.Collider_GetTag(entity.id, out string tag);
            return tag;
        }
    }
}
