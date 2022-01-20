#pragma once
#include <list>
#include <sdl2/SDL.h>
#include "../Task/Task.h"

class Kernel
{
private:
    /// <summary>
    /// These tasks will only run once.
    /// </summary>
    std::list < Task*> tasksToInitialize;
    /// <summary>
    /// This tasks will run in a loop.
    /// </summary>
    std::list < Task*> runningTasks;
    Uint64 millisecondsPreviousFrame;
    double deltaTime = 1.f/60.f;
    bool exit;
public:

    Kernel()
    {
        millisecondsPreviousFrame = SDL_GetTicks64();
    }

    void InitializeTask(Task& task)
    {
        tasksToInitialize.push_back(&task);
    }

    void AddRunningTask(Task& task)
    {
        runningTasks.push_back(&task);
    }


    // This will set the task to be on the front of the task list.
    void AddPriorizedRunningTask(Task& task)
    {
        runningTasks.push_front(&task);
    }

    void Execute();
    void Stop()
    {
        exit = true;
        SDL_Quit();
    }
    
    ~Kernel() = default;
};