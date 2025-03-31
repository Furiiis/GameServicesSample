#include "connector.h"
#include "services_models/login_model.h"
#include "ui_models/main_model.h"


namespace Connector {


    ServicesConnector::ServicesConnector(Event::EventBus *eventBus)
      : evBus(eventBus), login(eventBus), friends(eventBus), logs(eventBus),
        messages(eventBus) {
        Event::Listener friendsServiceIsOnlineEventListener;
        friendsServiceIsOnlineEventListener.Listen<ServiceEvents::FriendsServiceIsOnlineEvent>(
                [this](const ServiceEvents::FriendsServiceIsOnlineEvent& event) {
                    isFriendsServiceOnline = true;
                    if (AreAllServicesConnected()) {
                        evBus->PostEvent<Events::SuccessLoginEvent>({});
                    }
                });
        evBus->AddListener(std::move(friendsServiceIsOnlineEventListener));

        Event::Listener messagesServiceIsOnlineEventListener;
        messagesServiceIsOnlineEventListener.Listen<ServiceEvents::MessagesServiceIsOnlineEvent>(
                [this](const ServiceEvents::MessagesServiceIsOnlineEvent& event) {
                    isMessagesServiceIsOnline = true;
                    if (AreAllServicesConnected()) {
                        evBus->PostEvent<Events::SuccessLoginEvent>({});
                    }
                });
        evBus->AddListener(std::move(messagesServiceIsOnlineEventListener));
    }


    bool ServicesConnector::AreAllServicesConnected() const {
        return isFriendsServiceOnline && isMessagesServiceIsOnline;
    }


void ServicesConnector::SetUp() {
   Event::Listener successLoginEventListener;
   successLoginEventListener.Listen<ServiceEvents::SuccessLoginEvent>(
           [this](const ServiceEvents::SuccessLoginEvent& event) {
               friends.SetUp();
               messages.SetUp(event.login);
           });
   evBus->AddListener(std::move(successLoginEventListener));

   logs.SetUp();
   login.SetUp();
}


} // Connector
