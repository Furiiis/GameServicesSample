#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>


// lastTimeUpdated

namespace UIModel {


    enum class MessageStatus {
        Sent,
        Received,
        Delivered,
        Failed
    };


    struct User {
        std::string name;
    };


    struct Message {
        User sender;
        std::string message;
        int64_t timestamp;
        MessageStatus status;
    };


    struct Chat {
        std::string chatName;
        std::string channelName;
        std::unordered_set<std::string> users;
        std::vector<Message> messages;
        int64_t lastUpdatedTimestamp;
    };


    struct MessagesMainModel {
        std::string receiver;
        std::unordered_map<std::string, Chat> channelToChats;
        std::unordered_map<int64_t, std::string> lastUpdatedTimeToChannels;
    };


} // namespace UIModel


namespace Events {


    struct GetMessageEvent {
        std::string channel;
        UIModel::Message message;
    };


    struct JoinedToChannelEvent {
        UIModel::Chat chat;
    };


    struct LoadCachedMessagesEvent {
        UIModel::MessagesMainModel messages;
    };


} // namespace Events


namespace ConnectorEvents {


    struct SendMessageEvent {
        std::string channel;
        UIModel::Message message;
    };


    struct JoinToChannelEvent {
        std::string channel;
    };


    struct JoinToChannelForUsersEvent {
        std::unordered_set<std::string> users;
    };


} // namespace ConnectorEvents