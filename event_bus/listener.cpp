#include "listener.h"


namespace Event {


    void Listener::Notify(const std::any &event) const {
        if (eventType_ == std::type_index(event.type())) {
            callback(event);
        }
    }


    std::type_index Listener::GetEventTypeIndex() const {
        return eventType_;
    }


} // Event
