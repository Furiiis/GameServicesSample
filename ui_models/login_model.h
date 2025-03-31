#pragma once
#include <string>


namespace UIModel {


    enum class AuthorizationProvider {
        DEVELOPER
    };


    struct LoginMainModel {
        AuthorizationProvider provider;
        std::string userName;
    };


} // namespace UIModel


namespace ConnectorEvents {


    struct LoginEvent {
        UIModel::LoginMainModel login;
    };


    struct SuccessfulLoginEvent {
        std::string login;
    };


} // namespace ConnectorEvents
