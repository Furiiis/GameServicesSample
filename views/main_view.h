#pragma once
#include "ui_models/main_model.h"
#include "event_bus/event_bus.h"
#include "window.h" // todo change header name
#include "views/friends_view.h"
#include "views/logs_view.h"
#include "views/message_view.h"
#include "views/profile_view.h"
#include "views/login_view.h"


namespace Windows {

    class MainView {
    public:
        MainView(Event::EventBus* eventBus);

        void ShowWindow();

    private:
        void ShowProfileWindowEvent(const Events::ShowProfileWindowEvent& event);
        void ShowMessagesWindowEvent(const Events::ShowMessagesWindowEvent& event);
        void SuccessLoginEvent(const Events::SuccessLoginEvent& event);

        Event::EventBus* evBus;

        bool isUserLogged = false;

        bool loginCheck = false;
        LoginView loginWindow;

        bool friendsCheck = false;
        FriendsView friendsWindow;

        bool logsCheck = false;
        LogsView logsWindow;

        bool messagesCheck = false;
        MessageView messagesWindow;

        bool profileCheck = false;
        ProfileView profileWindow;

        std::vector<IndependentWindow> independentWindows;
    };
} // namespace Windows