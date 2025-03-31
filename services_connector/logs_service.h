#pragma once
#include "../event_bus/event_bus.h"


namespace Connector {


    class LogsService {
    public:
        LogsService(Event::EventBus* eventBus) : evBus(eventBus) {}


        void SetUp();
        static void Log(const char* format, ...);


    private:
        Event::EventBus* evBus;
    };


} // Connector
