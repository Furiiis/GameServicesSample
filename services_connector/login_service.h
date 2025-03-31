#pragma once
#include <string>

#include "../event_bus/event_bus.h"
#include "../ui_models/login_model.h"
#include "services_models/auth_cache.h"


namespace Connector {


    class LoginService {
    public:
        LoginService(Event::EventBus* eventBus) : evBus(eventBus) {}


        void SetUp();
        
        // Методы для тестирования
        bool Login(const UIModel::LoginMainModel& loginData);
        bool IsLoggedIn() const { return isLoggedIn; }
        std::string GetCurrentUser() const { return currentUser; }
        std::string GetCurrentAuth() const { return auth; }
        
        // Методы для работы с кэшем
        std::string GetAuthByLogin(const std::string& login) {
            return authCache.GetAuthByLogin(login);
        }
        std::string GetLoginByAuth(const std::string& auth) {
            return authCache.GetLoginByAuth(auth);
        }


    private:
        Event::EventBus* evBus;
        bool isLoggedIn = false;
        std::string currentUser;
        std::string auth; // Токен авторизации в формате 000000000000000-0000000-000000
        AuthCache authCache;
    };

} // Connector
