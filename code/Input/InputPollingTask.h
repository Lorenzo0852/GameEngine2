#pragma once
#include <Task/Task.h>
#include <sdl2/SDL.h>
#include <EventBus/EventBus.h>
#include <Events/InputEvent.h>
#include <spdlog/spdlog.h>

class InputPollingTask : public Task
{
private:
    std::shared_ptr<EventBus> eventBus;
public:

    InputPollingTask(std::shared_ptr<EventBus> eventBus)
    {
        this->eventBus = eventBus;
    }

	void Run(float deltaTime)
	{
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent))
        {
            if (sdlEvent.type == SDL_KEYDOWN)
            {
                switch (sdlEvent.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::QUIT, 1);
                    break;
                case SDLK_w:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::FORWARD, 1);
                    break;
                case SDLK_a:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::LEFT, 1);
                    break;
                case SDLK_s:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::BACKWARDS, 1);
                    break;
                case SDLK_d:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::RIGHT, 1);
                    break;
                case SDLK_LEFT:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::LEFT_ROTATION, 1);
                    break;
                case SDLK_RIGHT:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::RIGHT_ROTATION, 1);
                    break;
                }
            }
            //If != 0, the key is repeated
            else if (sdlEvent.type == SDL_KEYUP && sdlEvent.key.repeat == 0)
            {
                switch (sdlEvent.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::QUIT, 0);
                    break;
                case SDLK_w:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::FORWARD, 0);
                    break;
                case SDLK_a:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::LEFT, 0);
                    break;
                case SDLK_s:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::BACKWARDS, 0);
                    break;
                case SDLK_d:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::RIGHT, 0);
                    break;
                case SDLK_LEFT:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::LEFT_ROTATION, 0);
                    break;
                case SDLK_RIGHT:
                    eventBus->FireEvent<InputEvent>(InputEvent::Action::RIGHT_ROTATION, 0);
                    break;
                }
            }

            switch (sdlEvent.type)
            {
            //Event that the system triggers when the user closes the window
            case SDL_QUIT:
                eventBus->FireEvent<InputEvent>(InputEvent::Action::QUIT, 1);
                break;
            }
        }
	}
};