using System;

public struct Quaternion
{
    public float x;
    public float y;
    public float z;
    public float w;

    public Quaternion(float x, float y, float z, float w) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.w = w;
    }

    public Quaternion normalize()
    {
        float length = (float)Math.Sqrt(x * x + y * y + z * z + w * w);
        return new Quaternion(x / length, y / length, z / length, w / length);
    }

    public static Quaternion lookAt(Vector3 direction, Vector3 up)
    {
        Vector3 forward = direction.normalize();
        Vector3 right = Vector3.cross(up, forward).normalize();
        up = Vector3.cross(forward, right).normalize();

        // always case 1 or 3 because the up vector is always (0, 1, 0)
        // case 2 and 4 may need to have value swapped
        float trace = right.x + up.y + forward.z;
        if (trace > 0.0f) {
            float s = (float)Math.Sqrt(trace + 1.0f);
            float w = s * 0.5f;
            s = 0.5f / s;
            return new Quaternion((right.y + up.x) * s, w, (up.z - forward.y) * s, (right.z - forward.x) * s);
        } else if (right.x > up.y && right.x > forward.z) {
            float s = (float)Math.Sqrt(1.0f + right.x - up.y - forward.z);
            float x = s * 0.5f;
            s = 0.5f / s;
            return new Quaternion(x, (right.y + up.x) * s, (forward.x + right.z) * s, (up.z - forward.y) * s);
        } else if (up.y > forward.z) {
            float s = (float)Math.Sqrt(1.0f + up.y - right.x - forward.z);
            float y = s * 0.5f;
            s = 0.5f / s;
            return new Quaternion((up.z + forward.y) * s, (right.z - forward.x) * s, (right.y - up.x) * s, y);
        } else {
            float s = (float)Math.Sqrt(1.0f + forward.z - right.x - up.y);
            float z = s * 0.5f;
            s = 0.5f / s;
            return new Quaternion((forward.x + right.z) * s, (up.z + forward.y) * s, z, (right.y - up.x) * s);
        }
    }

    public static Quaternion angleAxis(float angle, Vector3 axis)
    {
        float halfAngle = angle * 0.5f;
        float sin = (float)Math.Sin(halfAngle);
        float cos = (float)Math.Cos(halfAngle);

        return new Quaternion(axis.x * sin, axis.y * sin, axis.z * sin, cos);
    }

    public static Quaternion operator *(Quaternion a, Vector3 b)
    {
        return new Quaternion(
            a.w * b.x + a.y * b.z - a.z * b.y,
            a.w * b.y + a.z * b.x - a.x * b.z,
            a.w * b.z + a.x * b.y - a.y * b.x,
            a.x * b.x + a.y * b.y + a.z * b.z
        );
    }

    public static Quaternion operator *(Quaternion a, Quaternion b)
    {
        float x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
        float y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
        float z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
        float w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
        return new Quaternion(x, y, z, w);
    }
}
