#pragma once
#include <typeindex>
#include <functional>
#include <any>


namespace Event {


    class Listener {
    public:

        template<typename T>
        void Listen(std::function<void(const T &event)> callbackForEvent) {
            if (callback) {
                return;
            }

            callback = [callbackForEvent](const std::any& event) {
                callbackForEvent(std::any_cast<const T &>(event));
            };

            eventType_ = std::type_index(typeid(T));
        }


        void Notify(const std::any &event) const;
        std::type_index GetEventTypeIndex() const;


    private:
        std::type_index eventType_ = typeid(char);
        std::function<void(const std::any &)> callback;
    };


} // Event
