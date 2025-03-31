#include "event_bus.h"


namespace Event {


    void EventBus::AddListener(const Listener* listener) {
        tempListeners.insert({listener->GetEventTypeIndex(), listener});
    }


    void EventBus::AddListener(const Listener&& listener) {
        permanentListeners.insert({listener.GetEventTypeIndex(), listener});
    }


    void EventBus::RemoveListener(const Listener *listener) {
        auto range = tempListeners.equal_range(listener->GetEventTypeIndex());
        for (auto it = range.first; it != range.second; ++it) {
            if(it->second == listener) {
                tempListeners.erase(it);
            }
        }
    }


} // Event
