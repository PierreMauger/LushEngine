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
            object instance = InternalCalls.Entity_GetScriptInstance(this.id, typeof(T).Name);
            return instance as T;
        } else {
            T component = new T() { entity = this };
            return component;
        }
    }

    public bool hasComponent<T>() where T : Component, new()
    {
        return InternalCalls.Entity_HasComponent(this.id, typeof(T).Name);
    }

    public void addComponent(string componentName)
    {
        InternalCalls.Entity_AddComponent(this.id, componentName);
    }

    public void removeComponent(string componentName)
    {
        InternalCalls.Entity_RemoveComponent(this.id, componentName);
    }

    public void setParent(ulong id)
    {
        InternalCalls.Entity_SetParent(this.id, id);
    }

    public void delete()
    {
        InternalCalls.Entity_Delete(this.id);
    }

    public string getName()
    {
        return InternalCalls.Entity_GetName(this.id);
    }

    public void log(string message, Toast type = Toast.None)
    {
        InternalCalls.Entity_Log(this.id, message, (int)type);
    }
}
