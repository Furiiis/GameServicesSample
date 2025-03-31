#include <algorithm>


#include "imgui.h"


#include "message_view.h"
#include "os/utils.h"
#include "ui_models/login_model.h"


namespace Windows {


    namespace {


        std::string ConstructChatName(const std::string& receiver, const std::unordered_set<std::string>& users) {
            std::string chatName;
            for (const std::string& user : users) {
                if (user == receiver) {
                    continue;
                }

                if (!chatName.empty()) {
                    chatName += ",";
                }
                chatName += user;
            }

            return chatName;
        }


    } // namespace


    MessageView::MessageView(Event::EventBus* eventBus, UIModel::MessagesMainModel message)
        : evBus(eventBus), data(std::move(message)) {
        Event::Listener getMessageEventListener;
        getMessageEventListener.Listen<Events::GetMessageEvent>(
            [this](const Events::GetMessageEvent& event) {
                this->GetMessageEvent(event);
            });
        evBus->AddListener(std::move(getMessageEventListener));

        Event::Listener joinedToChannelEventListener;
        joinedToChannelEventListener.Listen<Events::JoinedToChannelEvent>(
            [this](const Events::JoinedToChannelEvent& event) {
                this->JoinedToChannelEvent(event);
            });
        evBus->AddListener(std::move(joinedToChannelEventListener));

        Event::Listener successfulLoginEvent;
        successfulLoginEvent.Listen<ConnectorEvents::SuccessfulLoginEvent>(
            [this](const ConnectorEvents::SuccessfulLoginEvent& event) {
                data.receiver = event.login;
            });
        evBus->AddListener(std::move(successfulLoginEvent));

        Event::Listener LoadCachedMessagesEventListener;
        LoadCachedMessagesEventListener.Listen<Events::LoadCachedMessagesEvent>(
            [this](const Events::LoadCachedMessagesEvent& event) {
                this->LoadCachedMessagesEvent(event);
            });
        evBus->AddListener(std::move(LoadCachedMessagesEventListener));
    }


    void MessageView::LoadCachedMessagesEvent(const Events::LoadCachedMessagesEvent &event) {
        if (event.messages.channelToChats.empty()) {
            return;
        }

        data = event.messages;

        if (event.messages.lastUpdatedTimeToChannels.empty()) {
            for (const auto& [channel, chat] : data.channelToChats) {
                data.lastUpdatedTimeToChannels.insert({chat.lastUpdatedTimestamp, channel});
            }
        }
    }


    void MessageView::GetMessageEvent(const Events::GetMessageEvent& event) {
        if (data.channelToChats.contains(event.channel)) {
            data.channelToChats[event.channel].messages.push_back({event.message});

            data.lastUpdatedTimeToChannels.erase(data.channelToChats[event.channel].lastUpdatedTimestamp);
            data.channelToChats[event.channel].lastUpdatedTimestamp = event.message.timestamp;
            data.lastUpdatedTimeToChannels.insert({event.message.timestamp, event.channel});
        } else {
            UIModel::Chat chat;
            chat.channelName = event.channel;
            chat.users = {event.message.sender.name, data.receiver};
            chat.messages.push_back({event.message});
            chat.chatName = ConstructChatName(data.receiver, chat.users);
            chat.lastUpdatedTimestamp = event.message.timestamp;
            data.channelToChats[event.channel] = std::move(chat);
            data.lastUpdatedTimeToChannels.insert({event.message.timestamp, event.channel});
        }
    }


    void MessageView::JoinedToChannelEvent(const Events::JoinedToChannelEvent &event) {
        if (!data.channelToChats.contains(event.chat.channelName)) {
            selectedChatChannel = event.chat.channelName;

            data.channelToChats.insert({event.chat.channelName, event.chat});
            data.lastUpdatedTimeToChannels.insert({event.chat.lastUpdatedTimestamp, event.chat.channelName});
        }
    }


    void MessageView::SetActiveUser(const std::string &activeUser) {
        for (int i = 0; const auto & [key, chat] : data.channelToChats) {
            if (chat.users.size() != 2) {
                continue;
            }
            if (chat.users.contains(activeUser)) {
                selectedChatChannel = chat.channelName;
                return;
            }
            i++;
        }

        evBus->PostEvent<ConnectorEvents::JoinToChannelForUsersEvent>({{activeUser}});
    }


    void MessageView::SendMessage() {
        if (selectedChatChannel.empty()) {
            return;
        }
        if (inputBuffer[0] == '\0') {
            return;
        }
        if (!data.channelToChats.contains(selectedChatChannel)) {
            return;
        }

        UIModel::Message newMessage = {
                data.receiver,
                inputBuffer,
                OS::Utils::GetCurrentTime1(),
                UIModel::MessageStatus::Sent
        };
        data.channelToChats[selectedChatChannel].messages.push_back(newMessage);
        inputBuffer[0] = '\0';  // Очистка буфера

        data.lastUpdatedTimeToChannels.erase(data.channelToChats[selectedChatChannel].lastUpdatedTimestamp);
        data.channelToChats[selectedChatChannel].lastUpdatedTimestamp = newMessage.timestamp;
        data.lastUpdatedTimeToChannels.insert({newMessage.timestamp, selectedChatChannel});

        evBus->PostEvent<ConnectorEvents::SendMessageEvent>({selectedChatChannel, newMessage});
    }

    void MessageView::ShowWindow(bool& check) {
        ImGui::Begin("Messages", &check, ImGuiWindowFlags_MenuBar);

        // Создайте меню бар внутри окна
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Options")) {
                ImGui::MenuItem("Join to channel", NULL, &optionsCheck);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Проверка на открытое модальное окно
        if (optionsCheck) {
            ImGui::OpenPopup("Join to channel");
            if (ImGui::BeginPopupModal("Join to channel", NULL)) {
                ImGui::Text("Set channel:");
                static char inputBuffer[256] = "";  // Буфер для ввода текста (статический)

                ImGui::InputText("channel", inputBuffer, 256);

                // Обработка нажатия клавиши Enter
                if (ImGui::IsItemActivated() && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
                    // Действие при нажатии Enter
                    optionsCheck = false;  // Сбросить флаг
                    ImGui::CloseCurrentPopup();  // Закрыть модальное окно
                }

                // Кнопка OK

                if (ImGui::Button("OK")) {
                    evBus->PostEvent<ConnectorEvents::JoinToChannelEvent>({.channel= inputBuffer});
                    optionsCheck = false;  // Сбросить флаг
                    ImGui::CloseCurrentPopup();  // Закрыть модальное окно
                }

                if (ImGui::Button("Cancel")) {
                    optionsCheck = false;  // Сбросить флаг
                    ImGui::CloseCurrentPopup();  // Закрыть модальное окно
                }
            }
            ImGui::EndPopup();
        }

        // Обработка отображения информации о чате
        if (showChatInfo && !selectedChatForInfo.empty()) {
            ImGui::OpenPopup("Chat info");
            if (ImGui::BeginPopupModal("Chat info", NULL)) {
                assert(data.channelToChats.contains(selectedChatForInfo));

                UIModel::Chat & chat = data.channelToChats[selectedChatForInfo];
                // Статический буфер для редактирования имени чата
                static char nameBuffer[256] = "";

                // Отображаем имя чата как текст или поле ввода в зависимости от режима
                ImGui::Text("Chat name: ");
                ImGui::SameLine();

                if (!chatNameEditMode) {
                    // Отображаем текст как Selectable
                    ImGui::Selectable(chat.chatName.c_str(), false, ImGuiSelectableFlags_DontClosePopups);

                    // Проверяем двойной клик отдельно
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                        chatNameEditMode = true;
                        strncpy(nameBuffer, chat.chatName.c_str(), sizeof(nameBuffer) - 1);
                    }
                } else {
                    // Показываем поле ввода
                    if (ImGui::InputText("##chatname", nameBuffer, sizeof(nameBuffer),
                        ImGuiInputTextFlags_EnterReturnsTrue)) {
                        chat.chatName = nameBuffer;
                        chatNameEditMode = false;
                    }

                    // Кнопки для сохранения/отмены изменений
                    ImGui::SameLine();
                    if (ImGui::Button("Save")) {
                        chat.chatName = nameBuffer;
                        chatNameEditMode = false;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel")) {
                        chatNameEditMode = false;
                    }
                }

                ImGui::Text("Users count: %zu", chat.users.size());

                std::string usersString;
                for (const auto &user: chat.users) {
                    if (!usersString.empty()) {
                        usersString += ", ";
                    }
                    usersString += user;
                }
                ImGui::Text("Users: %s", usersString.c_str());

                // Отображаем текст кода канала как кликабельный элемент
                ImGui::Text("ChannelCode: ");
                ImGui::SameLine();

                if (ImGui::Selectable(chat.channelName.c_str(), false,
                                      ImGuiSelectableFlags_DontClosePopups)) {
                    ImGui::SetClipboardText(chat.channelName.c_str());
                }

                if (ImGui::Button("Close") || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    showChatInfo = false;
                    chatNameEditMode = false; // Сбрасываем флаг редактирования
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
        ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.24f, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX, window_flags);

        for (int i = 0; const auto& [lastUpdatedTime, channelName] : data.lastUpdatedTimeToChannels) {
            // Проверяем выделен ли текущий элемент
            bool isSelected = selectedChatChannel == channelName;
            
            if (ImGui::Selectable(data.channelToChats[channelName].chatName.c_str(), isSelected)) {
                selectedChatChannel = channelName;
            }
            
            // Открываем ChatInfo сразу по правому клику
            if (ImGui::IsItemClicked(1)) { // 1 = правая кнопка мыши
                selectedChatForInfo = channelName;
                showChatInfo = true;
            }
            
            i++;
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("MessagesArea", ImVec2(0, 0), true);
        // Создаем прокручиваемую область для сообщений
        // todo delete magic number 50
        if (!selectedChatChannel.empty()) {
            ImGui::BeginChild("MessagesChild", ImVec2(0, ImGui::GetContentRegionAvail().y - 50), true);
            if (!selectedChatChannel.empty()) {
                assert(data.channelToChats.contains(selectedChatChannel));

                for (const auto &msg : data.channelToChats[selectedChatChannel].messages) {
                    // todo smth with GetTime fix
                    //std::string timeStr = OS::Utils::GetTime(msg.timestamp);
                    std::string timeStr = "14:15:15";
                    if (msg.sender.name == data.receiver) {
                        // Устанавливаем прозрачность в зависимости от статуса
                        float alpha = (msg.status == UIModel::MessageStatus::Sent) ? 1.0f : 0.5f;
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, alpha));
                        ImGui::Text("[%s] %s: %s", timeStr.c_str(), msg.sender.name.c_str(), msg.message.c_str());
                        ImGui::PopStyleColor();
                    } else {
                        ImGui::Text("[%s] %s: %s", timeStr.c_str(), msg.sender.name.c_str(), msg.message.c_str());
                    }
                }
            }
            ImGui::EndChild(); // Закрываем прокручиваемую область

            // Получаем доступное пространство для ввода
            ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_CtrlEnterForNewLine
                                                 | ImGuiInputTextFlags_EnterReturnsTrue;
            if (ImGui::InputTextMultiline("Input", inputBuffer, sizeof(inputBuffer), ImVec2(-FLT_MIN, 50),
                                          inputTextFlags)) {
                SendMessage();
            }

            ImGui::EndChild();
        } else {
            ImGui::EndChild();
        }

        ImGui::End();

        /* if (!ImGui::Begin("Messages", &check))
         {
             ImGui::End();
             return;
         }

         // Options menu
         if (ImGui::BeginPopup("Options"))
         {
             ImGui::Checkbox("Auto-scroll", &AutoScroll);
             ImGui::EndPopup();
         }

         // Main window
         if (ImGui::Button("Options"))
             ImGui::OpenPopup("Options");
         ImGui::SameLine();
         bool clear = ImGui::Button("Clear");
         ImGui::SameLine();
         data.Filter.Draw("Filter", -100.0f);

         ImGui::Separator();

         if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
         {
             if (clear)
                 Clear();
             if (copy)
                 ImGui::LogToClipboard();

             ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
             const char* buf = data.Buf.begin();
             const char* buf_end = data.Buf.end();
             if (data.Filter.IsActive())
             {
                 // In this example we don't use the clipper when Filter is enabled.
                 // This is because we don't have random access to the result of our filter.
                 // A real application processing logs with ten of thousands of entries may want to store the result of
                 // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
                 for (int line_no = 0; line_no < data.LineOffsets.Size; line_no++)
                 {
                     const char* line_start = buf + data.LineOffsets[line_no];
                     const char* line_end = (line_no + 1 < data.LineOffsets.Size) ? (buf + data.LineOffsets[line_no + 1] - 1) : buf_end;
                     if (data.Filter.PassFilter(line_start, line_end))
                         ImGui::TextUnformatted(line_start, line_end);
                 }
             }
             else
             {
                 // The simplest and easy way to display the entire buffer:
                 //   ImGui::TextUnformatted(buf_begin, buf_end);
                 // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
                 // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
                 // within the visible area.
                 // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
                 // on your side is recommended. Using ImGuiListClipper requires
                 // - A) random access into your data
                 // - B) items all being the  same height,
                 // both of which we can handle since we have an array pointing to the beginning of each line of text.
                 // When using the filter (in the block of code above) we don't have random access into the data to display
                 // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
                 // it possible (and would be recommended if you want to search through tens of thousands of entries).
                 ImGuiListClipper clipper;
                 clipper.Begin(data.LineOffsets.Size);
                 while (clipper.Step())
                 {
                     for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                     {
                         const char* line_start = buf + data.LineOffsets[line_no];
                         const char* line_end = (line_no + 1 < data.LineOffsets.Size) ? (buf + data.LineOffsets[line_no + 1] - 1) : buf_end;
                         ImGui::TextUnformatted(line_start, line_end);
                     }
                 }
                 clipper.End();
             }
             ImGui::PopStyleVar();

             // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
             // Using a scrollbar or mouse-wheel will take away from the bottom edge.
             if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                 ImGui::SetScrollHereY(1.0f);
         }
         ImGui::EndChild();
         ImGui::End();
         */
    }

} // namespace Windows
