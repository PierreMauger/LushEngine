using System;
using System.Runtime.CompilerServices;

public enum Toast
{
    None = 0,
    Success = 1,
    Info = 2,
    Warning = 3,
    Error = 4
}

public static class InternalCalls
{
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static object Entity_GetScriptInstance(ulong entityId, string scriptName);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Entity_HasComponent(ulong id, string componentName);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Entity_AddComponent(ulong id, string componentName);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Entity_RemoveComponent(ulong id, string componentName);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Entity_SetParent(ulong id, ulong parentId);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Entity_Delete(ulong id);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static string Entity_GetName(ulong id);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Entity_Log(ulong id, string message, int type = 0);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Transform_GetPosition(ulong id, out Vector3 position);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Transform_SetPosition(ulong id, ref Vector3 position);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Transform_GetRotation(ulong id, out Quaternion rotation);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Transform_SetRotation(ulong id, ref Quaternion rotation);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Transform_GetScale(ulong id, out Vector3 scale);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Transform_SetScale(ulong id, ref Vector3 scale);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Model_GetName(ulong id, out string name);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Model_SetName(ulong id, string name);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Model_GetMaterialColor(ulong id, string materialName, out Vector3 color);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Model_SetMaterialColor(ulong id, string materialName, ref Vector3 color);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Camera_GetForward(ulong id, out Vector3 forward);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Camera_SetForward(ulong id, ref Vector3 forward);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Camera_GetFov(ulong id, out float fov);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Camera_SetFov(ulong id, float fov);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Camera_GetNear(ulong id, out float near);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Camera_SetNear(ulong id, float near);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Camera_GetFar(ulong id, out float far);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Camera_SetFar(ulong id, float far);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Light_GetIntensity(ulong id, out float type);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Light_SetIntensity(ulong id, float type);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Light_GetColor(ulong id, out Vector3 color);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Light_SetColor(ulong id, ref Vector3 color);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Cubemap_GetName(ulong id, out string name);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Cubemap_SetName(ulong id, string name);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Billboard_GetName(ulong id, out string name);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Billboard_SetName(ulong id, string name);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Billboard_GetLockYAxis(ulong id, out float lockYAxis);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Billboard_SetLockYAxis(ulong id, float lockYAxis);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static bool Collider_GetTag(ulong id, out string tag);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    internal extern static void Collider_SetTag(ulong id, string tag);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern static bool IsKeyDown(int key);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern static ulong GetEntityFromName(string id);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern static float GetMouseMovementX();
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern static float GetMouseMovementY();
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern static void SetMouseHidden(bool state);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern static void SetScene(string sceneName);
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern static void ResetScene();
}
