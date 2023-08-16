using System;
using System.Runtime.CompilerServices;

public class Entity
{
    public readonly ulong id;

    public Entity(ulong id)
    {
        this.id = id;
    }

    public T getComponent<T>() where T : Component, new()
    {
        if (typeof(T).IsSubclassOf(typeof(CustomComponent))) {
            object instance = InternalCalls.GetScriptInstance(this.id, typeof(T).Name);
            return instance as T;
        } else {
            T component = new T() { entity = this };
            return component;
        }
    }

    public bool hasComponent<T>() where T : Component, new()
    {
        return InternalCalls.HasComponent(this.id, typeof(T).Name);
    }

    public string getName()
    {
        return InternalCalls.GetName(this.id);
    }

    public void log(string message, Toast type = Toast.None)
    {
        InternalCalls.Log(this.id, message, (int)type);
    }
}
