#pragma once
#include "ui_models/message_model.h"
#include "event_bus/event_bus.h"

namespace Windows {

    class MessageView {
    public:

        MessageView(Event::EventBus* eventBus, UIModel::MessagesMainModel message);

        void ShowWindow(bool& check);
        void SetActiveUser(const std::string& activeUser);

    private:
        void GetMessageEvent(const Events::GetMessageEvent& event);
        void JoinedToChannelEvent(const Events::JoinedToChannelEvent& event);
        void LoadCachedMessagesEvent(const Events::LoadCachedMessagesEvent& event);

        void SendMessage();

        Event::EventBus* evBus;
        UIModel::MessagesMainModel data;
        std::string windowName;
        char inputBuffer[256] = "";  // Буфер для ввода текста
        std::string selectedChatChannel;
        bool optionsCheck = false;
        
        // Новые переменные для окна информации о чате
        bool showChatInfo = false;
        std::string selectedChatForInfo;
        bool chatNameEditMode = false; // Флаг для отслеживания режима редактирования имени чата
    };
} // namespace Windows