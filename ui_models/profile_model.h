#pragma once
#include <string>


namespace UIModel {


    struct ProfileMainModel {
        std::string avatarPath;
        std::string login;
        std::string authId;
        int64_t lastOnlineTimestamp;
    };


} // namespace UIModel


namespace Events {


} // namespace Events
