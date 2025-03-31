#include "messages_connector.h"
#include <random>
#include <algorithm>
#include "../os/utils.h"


namespace {


    // Список возможных автоматических ответов
    const std::vector<std::string> kAutoResponses = {
            "Ok, I'll check it",
            "Thanks for the update",
            "Sounds good!",
            "I'll get back to you soon",
            "Let me think about it",
            "Great idea!",
            "Can you provide more details?",
            "I agree with you",
            "Let's discuss it later",
            "Nice work!",
            "Interesting approach",
            "Keep me posted",
            "I'll handle it",
            "No problem",
            "Perfect timing"
    };


} // namespace


namespace Connector {

    MessagesService::MessagesService(Event::EventBus* eventBus) : evBus(eventBus) {}

    void MessagesService::SetUp(const std::string& login) {
        currentUser = login;
        InitializeTestData(login);

        // Подписываемся на события
        Event::Listener sendMessageEventListener;
        sendMessageEventListener.Listen<ConnectorEvents::SendMessageEvent>(
            [this](const ConnectorEvents::SendMessageEvent& event) {
                HandleSendMessage(event);
            });
        evBus->AddListener(std::move(sendMessageEventListener));

        Event::Listener joinToChannelEventListener;
        joinToChannelEventListener.Listen<ConnectorEvents::JoinToChannelEvent>(
            [this](const ConnectorEvents::JoinToChannelEvent& event) {
                HandleJoinToChannel(event);
            });
        evBus->AddListener(std::move(joinToChannelEventListener));

        Event::Listener joinToChannelForUsersEventListener;
        joinToChannelForUsersEventListener.Listen<ConnectorEvents::JoinToChannelForUsersEvent>(
                [this](const ConnectorEvents::JoinToChannelForUsersEvent& event) {
                    if (event.users.empty()) {
                        return;
                    }

                    UIModel::Chat chat;
                    for (const std::string& username : event.users) {
                        chat.channelName += username;
                    }
                    chat.channelName += currentUser;
                    chat.chatName = "chat with";
                    for (const std::string& user : event.users) {
                        chat.chatName += " " + user;
                    }
                    chat.users = event.users;
                    chat.users.insert(currentUser);

                    channelToChats[chat.channelName] = chat;
                    evBus->PostEvent<Events::JoinedToChannelEvent>({chat});
                });
        evBus->AddListener(std::move(joinToChannelForUsersEventListener));
    }

    void MessagesService::InitializeTestData(const std::string& username) {
        std::unordered_map<std::string, UIModel::Chat> chats;
        
        // Чат 1: Общий чат
        UIModel::Chat generalChat;
        generalChat.channelName = "general";
        generalChat.chatName = "Obshii chat";
        generalChat.users = {"User1", "User2", "User3", username};
        generalChat.messages = GenerateTestMessages(5, generalChat.users);
        generalChat.lastUpdatedTimestamp = 0;
        chats[generalChat.channelName] = generalChat;

        // Чат 2: Чат с одним пользователем
        UIModel::Chat privateChat;
        privateChat.channelName = "private_" + username + "_user1";
        privateChat.chatName = "Chat s User1";
        privateChat.users = {username, "User1"};
        privateChat.messages = GenerateTestMessages(4, privateChat.users);
        privateChat.lastUpdatedTimestamp = 1;
        chats[privateChat.channelName] = privateChat;

        // Чат 3: Групповой чат
        UIModel::Chat groupChat;
        groupChat.channelName = "group1";
        groupChat.chatName = "Gruppa razrabotchikov";
        groupChat.users = {username, "User1", "User2", "User4"};
        groupChat.messages = GenerateTestMessages(6, groupChat.users);
        groupChat.lastUpdatedTimestamp = 2;
        chats[groupChat.channelName] = groupChat;

        // Чат 4: Чат с другим пользователем
        UIModel::Chat anotherPrivateChat;
        anotherPrivateChat.channelName = "private_" + username + "_user2";
        anotherPrivateChat.chatName = "Chat s User2";
        anotherPrivateChat.users = {username, "User2"};
        anotherPrivateChat.messages = GenerateTestMessages(3, anotherPrivateChat.users);
        anotherPrivateChat.lastUpdatedTimestamp = 3;
        chats[anotherPrivateChat.channelName] = anotherPrivateChat;

        channelToChats = chats;

        evBus->PostEvent<Events::LoadCachedMessagesEvent>({
            .messages = { .receiver = username, .channelToChats = chats}
        });
    }

    void MessagesService::AddChat(const UIModel::Chat& chat) {
        if (!channelToChats.contains(chat.channelName)) {
            channelToChats.insert({chat.channelName, chat});
        }
    }

    void MessagesService::HandleSendMessage(const ConnectorEvents::SendMessageEvent& event) {
        if (currentUser.empty()) {
            return;
        }

        channelToChats[event.channel].messages.push_back(event.message);

        if (channelToChats.contains(event.channel)) {
            std::vector<std::string> otherUsers;
            for (const std::string& user : channelToChats[event.channel].users) {
                if (user != currentUser) {
                    otherUsers.push_back(user);
                }
            }

            if (!otherUsers.empty()) {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> userDis(0, otherUsers.size() - 1);
                std::uniform_int_distribution<> msgDis(0, kAutoResponses.size() - 1);

                UIModel::Message responseMessage;
                responseMessage.sender.name = otherUsers[userDis(gen)];
                responseMessage.message = kAutoResponses[msgDis(gen)];
                int64_t responseDelay = std::uniform_int_distribution<>(1, 3)(gen);
                responseMessage.timestamp = event.message.timestamp + responseDelay;
                responseMessage.status = UIModel::MessageStatus::Received;

                evBus->PostEvent<Events::GetMessageEvent>({
                    .channel = event.channel,
                    .message = responseMessage
                });
            }
        }
    }

    void MessagesService::HandleJoinToChannel(const ConnectorEvents::JoinToChannelEvent& event) {
        if (currentUser.empty()) {
            return;
        }

        if (channelToChats.contains(event.channel)) {
            evBus->PostEvent<Events::JoinedToChannelEvent>({channelToChats[event.channel]});
            return;
        } else {
            UIModel::Chat newChat;
            newChat.channelName = event.channel;
            newChat.chatName = "New chat";
            newChat.users = {currentUser};
            AddChat(newChat);
            evBus->PostEvent<Events::JoinedToChannelEvent>({newChat});
        }
    }

    std::vector<UIModel::Message> MessagesService::GenerateTestMessages(int count, const std::unordered_set<std::string>& users) {
        std::vector<UIModel::Message> messages;
        messages.reserve(count);
        
        std::vector<std::string> usersVec(users.begin(), users.end());
        
        for (int i = 0; i < count; ++i) {
            UIModel::Message msg;
            msg.sender.name = usersVec[rand() % usersVec.size()];
            msg.message = "Test message " + std::to_string(i + 1);
            msg.timestamp = time(nullptr) - (count - i) * 60;
            msg.status = UIModel::MessageStatus::Sent;
            
            messages.push_back(std::move(msg));
        }
        
        return messages;
    }

} // namespace Connector
