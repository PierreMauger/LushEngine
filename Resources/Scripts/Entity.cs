using System;
using System.Runtime.CompilerServices;

public class Entity
{
    protected Entity()
    {
        this.id = 0;
    }

    internal Entity(ulong id)
    {
        this.id = id;
    }

    public readonly ulong id;

    public T getComponent<T>() where T : Component, new()
    {
        T component = new T() { entity = this };
        return component;
    }

    public void log(string message)
    {
        InternalCalls.Log(this.id, message);
    }
}
