using System;
using System.Runtime.CompilerServices;

public static class InternalCalls
{
	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static void Transform_Get(ulong id, out Vector3 position);

	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	internal extern static void Transform_Set(ulong id, ref Vector3 position);
}
