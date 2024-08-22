using System;

public struct Vector3
{
    public float x;
    public float y;
    public float z;

    public Vector3(float value)
    {
        x = value;
        y = value;
        z = value;
    }

    public Vector3(float x, float y, float z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public Vector3 Normalize()
    {
        float length = Length();
        return length == 0.0f ? new Vector3(0.0f) : this / length;
    }

    public float Length()
    {
        return MathF.Sqrt(x * x + y * y + z * z);
    }

    public static Vector3 Cross(Vector3 a, Vector3 b)
    {
        return new Vector3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    public static float Dot(Vector3 a, Vector3 b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    public static Vector3 QuaternionToEuler(Quaternion q)
    {
        return new Vector3(
            MathF.Atan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y)),
            MathF.Asin(2.0f * (q.w * q.y - q.z * q.x)),
            MathF.Atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z))
        );
    }

    public static Vector3 operator *(Vector3 a, float b)
    {
        return new Vector3(a.x * b, a.y * b, a.z * b);
    }

    public static Vector3 operator /(Vector3 a, float b)
    {
        return new Vector3(a.x / b, a.y / b, a.z / b);
    }

    public static Vector3 operator +(Vector3 a, Vector3 b)
    {
        return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    public static Vector3 operator -(Vector3 a, Vector3 b)
    {
        return new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    public static bool operator ==(Vector3 a, Vector3 b)
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    public static bool operator !=(Vector3 a, Vector3 b)
    {
        return !(a == b);
    }

    public override bool Equals(object obj)
    {
        return obj is Vector3 other && this == other;
    }

    public override int GetHashCode()
    {
        return HashCode.Combine(x, y, z);
    }

    public static Vector3 operator *(Vector3 a, Quaternion b)
    {
        Quaternion conjugate = b.Conjugate();
        Quaternion result = b * a * conjugate;
        return new Vector3(result.x, result.y, result.z);
    }

    public Vector3 RadiansToDegrees()
    {
        return new Vector3(
            RadiansToDegrees(x),
            RadiansToDegrees(y),
            RadiansToDegrees(z)
        );
    }

    public static float DegreesToRadians(float degrees)
    {
        return degrees * MathF.PI / 180.0f;
    }

    public static float RadiansToDegrees(float radians)
    {
        return radians * 180.0f / MathF.PI;
    }

    public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
    {
        return a + (b - a) * t;
    }

    public static Vector3 Up => new Vector3(0.0f, 1.0f, 0.0f);
}
