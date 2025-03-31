#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <unordered_set>

#include "../event_bus/event_bus.h"
#include "../ui_models/message_model.h"

namespace Connector {

    class MessagesService {
    public:
        MessagesService(Event::EventBus* eventBus);
        void SetUp(const std::string& login);
        void InitializeTestData(const std::string& username);
        void AddChat(const UIModel::Chat& chat);

    private:
        void HandleSendMessage(const ConnectorEvents::SendMessageEvent& event);
        void HandleJoinToChannel(const ConnectorEvents::JoinToChannelEvent& event);
        std::vector<UIModel::Message> GenerateTestMessages(int count, const std::unordered_set<std::string>& users);

        Event::EventBus* evBus;
        std::string currentUser;
        std::unordered_map<std::string, UIModel::Chat> channelToChats;
    };

} // namespace Connector
