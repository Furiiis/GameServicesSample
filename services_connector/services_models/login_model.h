#pragma once
#include <string>


namespace ServiceEvents {


    struct SuccessLoginEvent {
        std::string login;
    };


    struct FriendsServiceIsOnlineEvent {
    };


    struct MessagesServiceIsOnlineEvent {
    };



} // namespace ServiceEvents
