#pragma once
class Task
{
    bool cancelled;
public:
    Task(bool cancelled = false) : cancelled(cancelled) {};

    //Not necessary to implement as they are defined. They can still be overriden.
    virtual bool Cancel() { this->cancelled = true; return this->cancelled; }
    virtual bool Initialize() { return true; };
    virtual bool Finalize() { return true; };
    /// <summary>
    /// Runs the task
    /// </summary>
    /// <param name="deltaTime">Time passed since last frame update</param>
    virtual void Run(float deltaTime) = 0;
};