#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "../event_bus/event_bus.h"
#include "../ui_models/friends_model.h"

namespace Connector {

    class FriendsService {
    public:
        FriendsService(Event::EventBus* eventBus) : evBus(eventBus) {}

        void SetUp() {
            InitializeTestData();

            Event::Listener acceptFriendEventListener;
            acceptFriendEventListener.Listen<ConnectorEvents::AcceptFriendInviteEvent>(
                [this](const ConnectorEvents::AcceptFriendInviteEvent& event) {
                    HandleAcceptFriend(event);
                });
            evBus->AddListener(std::move(acceptFriendEventListener));

            Event::Listener deleteFriendEventListener;
            deleteFriendEventListener.Listen<ConnectorEvents::DeleteFriendEvent>(
                    [this](const ConnectorEvents::DeleteFriendEvent& event) {
                        auto friendIt = std::find_if(userFriends.begin(), userFriends.end(), [&event](const UIModel::Friend& fr) {
                            return fr.name == event.user;
                        });
                        if (friendIt != userFriends.end()) {
                            evBus->PostEvent<Events::RemoveFriendEvent>({friendIt->name});
                            userFriends.erase(friendIt);
                        }
                    });
            evBus->AddListener(std::move(deleteFriendEventListener));

            evBus->PostEvent<Events::UpdateFriendsStatusEvent>({userFriends});
            for (const UIModel::Friend& aFriend : incomingRequests) {
                evBus->PostEvent<Events::AddFriendInviteEvent>({aFriend.name});
            }
            std::vector<std::string> outRequests;
            for (const UIModel::Friend& aFriend : outgoingRequests) {
                outRequests.push_back(aFriend.name);
            }
            evBus->PostEvent<Events::AddOutgoingInvitesEvent>({std::move(outRequests)});
        }


    private:
        Event::EventBus* evBus;
        std::string currentUser;
        
        // Хранение данных о друзьях для каждого пользователя
        std::vector<UIModel::Friend> userFriends;
        std::vector<UIModel::Friend> incomingRequests;
        std::vector<UIModel::Friend> outgoingRequests;

        void InitializeTestData() {
            // Создаем тестовых друзей
            std::vector<UIModel::Friend> friends{
                {"user1", true},
                {"user2", true},
                {"user3", false},
                {"user4", true}
            };

            // Создаем входящие запросы
            std::vector<UIModel::Friend> incoming = {
                {"user5", false},
                {"user6", false},
                {"user7", false}
            };

            // Создаем исходящие запросы
            std::vector<UIModel::Friend> outgoing = {
                {"user8", false},
                {"user9", false}
            };

            userFriends = friends;
            incomingRequests = incoming;
            outgoingRequests = outgoing;
        }


        void HandleAcceptFriend(const ConnectorEvents::AcceptFriendInviteEvent& event) {
            // Находим и удаляем запрос из входящих
            auto it = std::find_if(incomingRequests.begin(), incomingRequests.end(),
                [&event](const UIModel::Friend& f) { return f.name == event.fromUser; });
            
            if (it != incomingRequests.end()) {
                // Добавляем в друзья обоим пользователям
                userFriends.push_back(*it);

                evBus->PostEvent<Events::AddFriendEvent>({*it});

                // Удаляем запросы
                incomingRequests.erase(it);
            }
        }
    };

} // namespace Connector
