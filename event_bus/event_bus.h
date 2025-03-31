#pragma once
#include <unordered_map>
#include <thread>

#include "listener.h"


namespace Event {


    class EventBus {
    public:
        template<typename T>
        void PostEvent(const T &event) {
            std::any eventCopy = event;
            auto tempRange = tempListeners.equal_range(std::type_index(typeid(T)));
            for (auto it = tempRange.first; it != tempRange.second; ++it) {
                std::thread([&listener = it->second, eventCopy]() {
                    listener->Notify(eventCopy);
                }).detach();
                //std::thread(&Listener::Notify, &it->second, std::any(event)).detach();
                //std::thread([&it, &event]{
                //    it->second->Notify(event);
                //}).detach();
            }

            auto permanentRange = permanentListeners.equal_range(std::type_index(typeid(T)));
            for (auto it = permanentRange.first; it != permanentRange.second; ++it) {
                //std::thread(&Listener::Notify, &it->second, std::any(event)).detach();
                std::thread([&listener = it->second, eventCopy]() {
                    listener.Notify(eventCopy);
                }).detach();
                //std::thread([Listener listener = &it->second, &event]{
                //    it->second.Notify(event);
                //}).detach();
            }

        }


        void AddListener(const Listener* listener);
        void AddListener(const Listener&& listener);
        void RemoveListener(const Listener* listener);


    private:
        std::unordered_multimap<std::type_index, const Listener*> tempListeners;
        std::unordered_multimap<std::type_index, const Listener> permanentListeners;
    };


} // Event
