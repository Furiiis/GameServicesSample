#pragma once
#include "profile_model.h"


namespace Events {


    struct ShowProfileWindowEvent {
        UIModel::ProfileMainModel profile;
    };


    struct ShowMessagesWindowEvent {
        std::string user;
    };


    struct SuccessLoginEvent {
    };


} // EventModels


namespace ConnectorEvents {


    struct ShowUserProfileByLoginEvent {
        std::string login;
    };


    struct ShowMainUserProfileEvent {
    };


} // namespace ConnectorEvents

