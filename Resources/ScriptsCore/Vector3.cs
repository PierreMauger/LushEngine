using System;

public struct Vector3
{
    public float x;
    public float y;
    public float z;

    public Vector3(float x, float y, float z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public Vector3 normalize()
    {
        float length = MathF.Sqrt(x * x + y * y + z * z);
        if (length == 0.0f)
            return new Vector3(0.0f, 0.0f, 0.0f);
        return new Vector3(x / length, y / length, z / length);
    }

    public static Vector3 cross(Vector3 first, Vector3 second)
    {
        return new Vector3(first.y * second.z - first.z * second.y, first.z * second.x - first.x * second.z, first.x * second.y - first.y * second.x);
    }

    public static float dot(Vector3 first, Vector3 second)
    {
        return first.x * second.x + first.y * second.y + first.z * second.z;
    }

    public static Vector3 quaternionToEuler(Quaternion q)
    {
        Vector3 euler = new Vector3();
        euler.x = MathF.Atan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y));
        euler.y = MathF.Asin(2.0f * (q.w * q.y - q.z * q.x));
        euler.z = MathF.Atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));
        return euler;
    }

    public static Vector3 operator +(Vector3 a, Vector3 b)
    {
        return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    public static Vector3 operator *(Vector3 a, Quaternion b)
    {
        Quaternion conjugate = new Quaternion(-b.x, -b.y, -b.z, b.w);
        Quaternion result = b * a * conjugate;
        return new Vector3(result.x, result.y, result.z);
    }

    public Vector3 radiansToDegrees()
    {
        return new Vector3(radiansToDegrees(x), radiansToDegrees(y), radiansToDegrees(z));
    }

    public static float degreesToRadians(float degrees)
    {
        return degrees * MathF.PI / 180.0f;
    }

    public static float radiansToDegrees(float radians)
    {
        return radians * 180.0f / MathF.PI;
    }

}
