#pragma once
#include "ui_models/profile_model.h"
#include "event_bus/event_bus.h"
#include "window.h"

namespace Windows {

    class ProfileView : public IWindow {
    public:
        ProfileView(Event::EventBus* eventBus, UIModel::ProfileMainModel profile);
        ~ProfileView() {}

        void ShowWindow(bool& check);

    public:
        UIModel::ProfileMainModel data;

    private:
        Event::EventBus* evBus;
        std::string windowName;
    };

} // namespace Windows
