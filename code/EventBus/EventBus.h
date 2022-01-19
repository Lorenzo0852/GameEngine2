#pragma once
#include <spdlog/spdlog.h>
#include <map>
#include <typeindex>
#include "Event.h"

class IEventCallback
{
private:
	virtual void Call(Event& e) = 0;
public:
	virtual ~IEventCallback() = default;

	void Execute(Event& e)
	{
		Call(e);
	}
};

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback
{
private:
	typedef void (TOwner::*CallbackFunction)(TEvent&);

	TOwner* ownerInstance;
	CallbackFunction callbackFunction;

	virtual void Call(Event& e) override
	{
		std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
	}

public:
	EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction)
	{
		this->ownerInstance = ownerInstance;
		this->callbackFunction = callbackFunction;
	}

	virtual ~EventCallback() override = default;
};

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus
{
private:
	/// <summary>
	/// Maps the relation between the type of event (ex: KeyPressEvent) and a list of the functions it dispatches.
	/// </summary>
	std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

public:
	EventBus()
	{
		spdlog::info("EventBus constructor called");
	}
	~EventBus()
	{
		spdlog::info("EventBus destructor called");
	}
	
	/// <summary>
	/// Subscribe to an event of type T.
	/// </summary>
	template <typename TEvent, typename TOwner>
	void AddEventListener(TOwner*ownerInstance, void (TOwner::*callbackFunction)(TEvent&))
	{
		//Usage objective: eventBus->SubscribeToEvent<TEvent>(function);

		// What this means is:
		// Use this function (TOwner::*callbackFunction), with this parameter (TEvent&).

		if (!subscribers[typeid(TEvent)].get())
		{
			subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
		}

		auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
		//Add one more entry to the map.
		//[KEY]	  -> EventType
		//[VALUE] -> unique ptr of the instance subscribing.
		//We use std::move to change ownership from one unique_ptr to another unique_ptr;
		subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
	}

	/// <summary>
	/// Emit an event of type T. As soon as something emits an event,
	/// we go ahead and execute all listener callback functions.
	/// </summary>
	template <typename TEvent, typename ...TArgs>
	void FireEvent(TArgs&& ...args)
	{
		//Usage objective: eventBus->FireEvent<TEvent>(TArgs);
		auto handlers = subscribers[typeid(TEvent)].get();
		if (handlers)
		{
			for (auto it = handlers->begin(); it != handlers->end(); it++)
			{
				auto handler = it->get();
				TEvent event(std::forward<TArgs>(args)...);
				handler->Execute(event);
			}
		}
	}
};