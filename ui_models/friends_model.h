#pragma once
#include <vector>
#include <string>


namespace UIModel {


    struct Friend {
        std::string name;
        bool isOnline = false;
    };


    struct FriendsMainModel {
        std::vector<Friend> friends;
        std::vector<std::string> incomingRequests;
        std::vector<std::string> outgoingRequests;
    };


} // namespace UIModel


namespace Events {


    struct UpdateFriendsStatusEvent {
        std::vector<UIModel::Friend> friendsList;
    };


    struct UpdateFriendStatusEvent {
        UIModel::Friend user;
    };


    struct AddFriendEvent {
        UIModel::Friend user;
    };


    struct RemoveFriendEvent {
        std::string user;
    };


    struct AddFriendInviteEvent {
        std::string fromUser;
    };


    struct AddOutgoingInvitesEvent {
        std::vector<std::string> users;
    };


} // namespace Events


namespace ConnectorEvents {


    struct AcceptFriendInviteEvent {
        std::string fromUser;
    };


    struct SendFriendInviteEvent {
        std::string userLogin;
    };


    struct DeleteFriendEvent {
        std::string user;
    };


    struct GetFriendsListEvent {
    };


} // namespace ConnectorEvents
