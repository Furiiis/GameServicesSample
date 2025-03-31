#include <cassert>
#include <iostream>
#include <random>

#include "login_service.h"
#include "logs_service.h"
#include "../ui_models/profile_model.h"
#include "../ui_models/login_model.h"
#include "../ui_models/main_model.h"
#include "../os/utils.h"
#include "services_models/login_model.h"


namespace Connector {


    std::string GenerateAuthToken() {
        // Генерация случайного токена в формате 000000000000000-0000000-000000
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 9);
        
        std::string token;
        // Первая часть: 15 цифр
        for(int i = 0; i < 15; ++i) {
            token += std::to_string(dis(gen));
        }
        token += "-";
        // Вторая часть: 7 цифр
        for(int i = 0; i < 7; ++i) {
            token += std::to_string(dis(gen));
        }
        token += "-";
        // Третья часть: 6 цифр
        for(int i = 0; i < 6; ++i) {
            token += std::to_string(dis(gen));
        }
        
        return token;
    }

    void LoginService::SetUp() {
        Event::Listener loginEventListener;
        loginEventListener.Listen<ConnectorEvents::LoginEvent>(
                [this](const ConnectorEvents::LoginEvent& event) {
                    Login(event.login);
                });
        evBus->AddListener(std::move(loginEventListener));

        Event::Listener showUserProfileByLoginListener;
        showUserProfileByLoginListener.Listen<ConnectorEvents::ShowUserProfileByLoginEvent>(
                [this](const ConnectorEvents::ShowUserProfileByLoginEvent& event) {
                    evBus->PostEvent<Events::ShowProfileWindowEvent>({
                        .profile = {
                                .login = event.login,
                                .authId = authCache.GetAuthByLogin(event.login)
                        }
                    });
                });
        evBus->AddListener(std::move(showUserProfileByLoginListener));

        Event::Listener showMainUserProfileEventListener;
        showMainUserProfileEventListener.Listen<ConnectorEvents::ShowMainUserProfileEvent>(
                [this](const ConnectorEvents::ShowMainUserProfileEvent& event) {
                    evBus->PostEvent<Events::ShowProfileWindowEvent>({
                        .profile = {
                                .login = currentUser,
                                .authId = authCache.GetAuthByLogin(currentUser)
                        }
                    });
                });
        evBus->AddListener(std::move(showMainUserProfileEventListener));
    }

    bool LoginService::Login(const UIModel::LoginMainModel& loginData) {
        if (!loginData.userName.empty()) {
            isLoggedIn = true;
            currentUser = loginData.userName;
            auth = authCache.GetAuthByLogin(loginData.userName);
            evBus->PostEvent<ServiceEvents::SuccessLoginEvent>({loginData.userName});
            evBus->PostEvent<Events::SuccessLoginEvent>({});
            return true;
        }
        return false;
    }


} // Connector
