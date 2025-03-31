#pragma once
#include "login_service.h"
#include "friends_service.h"
#include "login_service.h"
#include "messages_connector.h"
#include "logs_service.h"


namespace Connector {


    class ServicesConnector {
    public:
        ServicesConnector(Event::EventBus *eventBus);

        void SetUp();


    private:
        bool AreAllServicesConnected() const;

    private:
        Event::EventBus* evBus;
        LogsService logs;
        LoginService login;
        bool isFriendsServiceOnline = false;
        FriendsService friends;

        bool isMessagesServiceIsOnline = false;
        MessagesService messages;
    };


} // Connector
