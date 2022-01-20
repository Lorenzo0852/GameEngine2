/******************************************\
 *  Copyright (c) Lorenzo Herran - 2021   *
\******************************************/

#include "Kernel.h"

void Kernel::Execute()
{
    exit = false;
    do 
    {
        millisecondsPreviousFrame = SDL_GetTicks64();

        if (!tasksToInitialize.empty())
        {
            for (auto task : tasksToInitialize)
            {
                //Won't enter here if there isn't any tasks to initialize.
                task->Initialize();
            }
            tasksToInitialize.clear();
        }
        
        for (auto task : runningTasks)
        {
            task->Run(deltaTime);
        }
        deltaTime = (SDL_GetTicks64() - millisecondsPreviousFrame) * 0.001f;
    }
    while (!exit);
}