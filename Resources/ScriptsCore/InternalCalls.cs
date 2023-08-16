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
	internal extern static void Log(ulong id, string message, int type = 0);
	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static string GetName(ulong id);

	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static bool HasComponent(ulong id, string componentName);

	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static void Transform_GetPosition(ulong id, out Vector3 position);
	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static void Transform_SetPosition(ulong id, ref Vector3 position);

	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static void Transform_GetRotation(ulong id, out Vector3 rotation);
	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static void Transform_SetRotation(ulong id, ref Vector3 rotation);

	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static void Transform_GetScale(ulong id, out Vector3 scale);
	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static void Transform_SetScale(ulong id, ref Vector3 scale);

	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static void Camera_GetForward(ulong id, out Vector3 forward);
	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static void Camera_SetForward(ulong id, ref Vector3 forward);

	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static void Collider_GetTag(ulong id, out string tag);

	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static object GetScriptInstance(ulong entityId, string scriptName);

	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	public extern static bool IsKeyDown(int key);

	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	public extern static float GetMouseMovementX();
	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	public extern static float GetMouseMovementY();
}
