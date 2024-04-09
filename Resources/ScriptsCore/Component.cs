using System;
using System.Collections.Generic;

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
        get => InternalCalls.Transform_GetPosition(entity.id, out Vector3 position) ? position : new Vector3();
        set => InternalCalls.Transform_SetPosition(entity.id, ref value);
    }
    public Quaternion rotation {
        get => InternalCalls.Transform_GetRotation(entity.id, out Quaternion rotation) ? rotation : new Quaternion();
        set => InternalCalls.Transform_SetRotation(entity.id, ref value);
    }
    public Vector3 scale {
        get => InternalCalls.Transform_GetScale(entity.id, out Vector3 scale) ? scale : new Vector3();
        set => InternalCalls.Transform_SetScale(entity.id, ref value);
    }
}

public class Model : Component
{
    public class Material
    {
        public readonly ulong id;

        public Material(ulong id)
        {
            this.id = id;
        }

        public Vector3 this[string materialName]
        {
            get => InternalCalls.Model_GetMaterialColor(id, materialName, out Vector3 color) ? color : new Vector3();
            set => InternalCalls.Model_SetMaterialColor(id, materialName, ref value);
        }
    }

    public Material materials => new Material(entity.id);

    public string name {
        get => InternalCalls.Model_GetName(entity.id, out string name) ? name : String.Empty;
        set => InternalCalls.Model_SetName(entity.id, value);
    }
}

public class Camera : Component
{
    public Vector3 forward {
        get => InternalCalls.Camera_GetForward(entity.id, out Vector3 forward) ? forward : new Vector3();
        set => InternalCalls.Camera_SetForward(entity.id, ref value);
    }
    public float fov {
        get => InternalCalls.Camera_GetFov(entity.id, out float fov) ? fov : 0.0f;
        set => InternalCalls.Camera_SetFov(entity.id, value);
    }
    public float near {
        get => InternalCalls.Camera_GetNear(entity.id, out float near) ? near : 0.0f;
        set => InternalCalls.Camera_SetNear(entity.id, value);
    }
    public float far {
        get => InternalCalls.Camera_GetFar(entity.id, out float far) ? far : 0.0f;
        set => InternalCalls.Camera_SetFar(entity.id, value);
    }
}

public class Light : Component
{
    public float intensity {
        get => InternalCalls.Light_GetIntensity(entity.id, out float intensity) ? intensity : 0.0f;
        set => InternalCalls.Light_SetIntensity(entity.id, value);
    }
    public Vector3 color {
        get => InternalCalls.Light_GetColor(entity.id, out Vector3 color) ? color : new Vector3();
        set => InternalCalls.Light_SetColor(entity.id, ref value);
    }
}

public class Cubemap : Component
{
    public string name {
        get => InternalCalls.Cubemap_GetName(entity.id, out string name) ? name : String.Empty;
        set => InternalCalls.Cubemap_SetName(entity.id, value);
    }
}

public class Billboard : Component
{
    public string name {
        get => InternalCalls.Billboard_GetName(entity.id, out string name) ? name : String.Empty;
        set => InternalCalls.Billboard_SetName(entity.id, value);
    }
    public float lockYAxis {
        get => InternalCalls.Billboard_GetLockYAxis(entity.id, out float lockYAxis) ? lockYAxis : 0.0f;
        set => InternalCalls.Billboard_SetLockYAxis(entity.id, value);
    }
}

public class Collider : Component
{
    public string tag {
        get => InternalCalls.Collider_GetTag(entity.id, out string tag) ? tag : String.Empty;
        set => InternalCalls.Collider_SetTag(entity.id, value);
    }
}
