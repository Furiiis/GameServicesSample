#pragma once
#include "ui_models/friends_model.h"
#include "event_bus/event_bus.h"

namespace Windows {

    class FriendsView {
    public:
        FriendsView(Event::EventBus* eventBus, UIModel::FriendsMainModel friends);
        void SetUp();

        void ShowWindow(bool& check);

    private:
        void UpdateFriendsStatusEvent(const Events::UpdateFriendsStatusEvent& event);
        void UpdateFriendStatusEvent(const Events::UpdateFriendStatusEvent& event);
        void AddFriendEvent(const Events::AddFriendEvent& event);
        void RemoveFriendEvent(const Events::RemoveFriendEvent& event);
        void AddFriendInviteEvent(const Events::AddFriendInviteEvent& event);
        void AddOutgoingInvitesEvent(const Events::AddOutgoingInvitesEvent& event);

        Event::EventBus* evBus;
        UIModel::FriendsMainModel data;
        bool friendInventCheck = false;

    };
} // namespace Windows