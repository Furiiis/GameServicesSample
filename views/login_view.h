#pragma once
#include "ui_models/login_model.h"
#include "event_bus/event_bus.h"

namespace Windows {

    class LoginView {
    public:
        LoginView(Event::EventBus* eventBus, UIModel::LoginMainModel login);

        void ShowWindow(bool& check);

    private:
        Event::EventBus* evBus;
        UIModel::LoginMainModel data;
    };

} // namespace Windows
