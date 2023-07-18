using System;
using System.Runtime.CompilerServices;

public class Entity
{
    public readonly ulong id;

    protected Entity()
    {
        this.id = 0;
    }

    public Entity(ulong id)
    {
        this.id = id;
    }

    public T getComponent<T>() where T : Component, new()
    {
        T component = new T() { entity = this };
        return component;
    }

    public T getScriptComponent<T>() where T : Entity, new()
    {
        object instance = InternalCalls.GetScriptInstance(this.id);
        return instance as T;
    }

    public void log(string message, Toast type = Toast.None)
    {
        InternalCalls.Log(this.id, message, (int)type);
    }
}