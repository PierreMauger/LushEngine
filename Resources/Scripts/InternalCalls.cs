using System;
using System.Runtime.CompilerServices;

public static class InternalCalls
{
	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static void Log(ulong id, string message);

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
}
