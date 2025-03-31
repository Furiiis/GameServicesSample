#pragma once
#include "ui_models/logs_model.h"
#include "event_bus/event_bus.h"

namespace Windows {

    class LogsView {
    public:
        LogsView(Event::EventBus* eventBus, UIModel::LogMainModel logs);

        void ShowWindow(bool& check);

    private:
        void AddLogEvent(const Events::AddLogEvent& event);

        Event::EventBus* evBus;
        UIModel::LogMainModel data;
        bool  AutoScroll;  // Keep scrolling if already at the bottom.
    };

} // namespace Windows
