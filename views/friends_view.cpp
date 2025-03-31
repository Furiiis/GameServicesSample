#include <algorithm>


#include "imgui.h"


#include "friends_view.h"
#include "ui_models/main_model.h"


namespace Windows {

    FriendsView::FriendsView(Event::EventBus* eventBus, UIModel::FriendsMainModel friends)
        : evBus(eventBus), data(std::move(friends)) {
        Event::Listener updateFriendsStatusEventListener;
        updateFriendsStatusEventListener.Listen<Events::UpdateFriendsStatusEvent>(
            [this](const Events::UpdateFriendsStatusEvent& event) {
                this->UpdateFriendsStatusEvent(event);
            });
        evBus->AddListener(std::move(updateFriendsStatusEventListener));

        Event::Listener updateFriendStatusEventListener;
        updateFriendStatusEventListener.Listen<Events::UpdateFriendStatusEvent>(
                [this](const Events::UpdateFriendStatusEvent& event) {
                    this->UpdateFriendStatusEvent(event);
                });
        evBus->AddListener(std::move(updateFriendStatusEventListener));

        Event::Listener addFriendInviteEventListener;
        addFriendInviteEventListener.Listen<Events::AddFriendInviteEvent>(
            [this](const Events::AddFriendInviteEvent& event) {
                this->AddFriendInviteEvent(event);
            });
        evBus->AddListener(std::move(addFriendInviteEventListener));

        Event::Listener addFriendEventListener;
        addFriendEventListener.Listen<Events::AddFriendEvent>(
            [this](const Events::AddFriendEvent& event) {
                this->AddFriendEvent(event);
            });
        evBus->AddListener(std::move(addFriendEventListener));

        Event::Listener removeFriendEventListener;
        removeFriendEventListener.Listen<Events::RemoveFriendEvent>(
            [this](const Events::RemoveFriendEvent& event) {
                this->RemoveFriendEvent(event);
            });
        evBus->AddListener(std::move(removeFriendEventListener));

        Event::Listener AddOutgoingInvitesEventListener;
        AddOutgoingInvitesEventListener.Listen<Events::AddOutgoingInvitesEvent>(
            [this](const Events::AddOutgoingInvitesEvent& event) {
                this->AddOutgoingInvitesEvent(event);
            });
        evBus->AddListener(std::move(AddOutgoingInvitesEventListener));
    }


    void FriendsView::SetUp() {
        evBus->PostEvent<ConnectorEvents::GetFriendsListEvent>({});
    }


    void FriendsView::UpdateFriendsStatusEvent(const Events::UpdateFriendsStatusEvent& event) {
        data.friends.clear();
        data.friends.reserve(event.friendsList.size());
        for (const UIModel::Friend &fr: event.friendsList) {
            data.friends.push_back({fr.name, fr.isOnline});
        }
    }


    void FriendsView::UpdateFriendStatusEvent(const Events::UpdateFriendStatusEvent &event) {
        auto friendToDeleteIt = std::find_if(data.friends.begin(), data.friends.end(),
                                             [event](const UIModel::Friend& aFriend) {
                                                 return aFriend.name == event.user.name;
        });
        if (friendToDeleteIt != data.friends.end()) {
            *friendToDeleteIt = event.user;
        }
    }


    void FriendsView::AddFriendInviteEvent(const Events::AddFriendInviteEvent& event) {
        data.incomingRequests.push_back(event.fromUser);
        auto outRequestIt = std::find(data.outgoingRequests.begin(), data.outgoingRequests.end(), event.fromUser);
        if (outRequestIt != data.outgoingRequests.end()) {
            data.outgoingRequests.erase(outRequestIt);
        }
    }


    void FriendsView::AddFriendEvent(const Events::AddFriendEvent &event) {
        data.friends.push_back(event.user);
    }


    void FriendsView::RemoveFriendEvent(const Events::RemoveFriendEvent &event) {
        auto friendToDeleteIt = std::find_if(data.friends.begin(), data.friends.end(),
                                          [event](const UIModel::Friend& aFriend) {
            return aFriend.name == event.user;
        });
        if (friendToDeleteIt != data.friends.end()) {
            data.friends.erase(friendToDeleteIt);
        }
    }


    void FriendsView::AddOutgoingInvitesEvent(const Events::AddOutgoingInvitesEvent &event) {
        for (const std::string& user : event.users) {
            auto requestIt = std::find(data.outgoingRequests.begin(), data.outgoingRequests.end(), user);
            if (requestIt == data.outgoingRequests.end()) {
                data.outgoingRequests.push_back(user);
            }
        }
    }


    void FriendsView::ShowWindow(bool& check) {

        ImGui::Begin("Friends", &check, ImGuiWindowFlags_MenuBar);

// Создайте меню бар внутри окна
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Options")) {
                ImGui::MenuItem("Add friend", NULL, &friendInventCheck);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

// Проверка на открытое модальное окно
        if (friendInventCheck) {
            ImGui::OpenPopup("Send invite");
            if (ImGui::BeginPopupModal("Send invite", NULL)) {
                ImGui::Text("Set user login:");
                static char inputBuffer[256] = "";  // Буфер для ввода текста (статический)

                ImGui::InputText("loginInput", inputBuffer, 256);

                // Обработка нажатия клавиши Enter
                if (ImGui::IsItemActivated() && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
                    // Действие при нажатии Enter
                    friendInventCheck = false;  // Сбросить флаг
                    ImGui::CloseCurrentPopup();  // Закрыть модальное окно
                }

                // Кнопка OK

                if (ImGui::Button("OK")) {
                    auto outRequestIt = std::find(data.outgoingRequests.begin(), data.outgoingRequests.end(), inputBuffer);
                    if (outRequestIt == data.outgoingRequests.end()) {
                        data.outgoingRequests.push_back(inputBuffer);
                    }
                    evBus->PostEvent<ConnectorEvents::SendFriendInviteEvent>({.userLogin = inputBuffer});
                    friendInventCheck = false;  // Сбросить флаг
                    ImGui::CloseCurrentPopup();  // Закрыть модальное окно
                }

                if (ImGui::Button("Cancel")) {
                    friendInventCheck = false;  // Сбросить флаг
                    ImGui::CloseCurrentPopup();  // Закрыть модальное окно
                }
            }
            ImGui::EndPopup();
        }


        for (const UIModel::Friend& aFriend : data.friends)
        {
            if (strcmp(aFriend.name.c_str(), "") == 0) {
                continue;
            }

            ImGui::PushID(aFriend.name.c_str());

            ImGui::Selectable(aFriend.name.c_str(), false);

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::Button("Profile")) {
                    evBus->PostEvent<ConnectorEvents::ShowUserProfileByLoginEvent>({aFriend.name});
                    ImGui::CloseCurrentPopup();
                }

                if (ImGui::Button("Send Message")) {
                    evBus->PostEvent<Events::ShowMessagesWindowEvent>({
                                                                                   .user = aFriend.name
                                                                           });
                    ImGui::CloseCurrentPopup();
                }

                if (ImGui::Button("Delete from friends")) {
                    evBus->PostEvent<ConnectorEvents::DeleteFriendEvent>({
                                                                              .user = aFriend.name
                                                                      });
                    ImGui::CloseCurrentPopup();
                }

                if (ImGui::Button("Ban")) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::SameLine();

            // online status sign
            ImVec4 circleColor = aFriend.isOnline ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Button, circleColor);
            ImGui::Button("", ImVec2(10, 10));
            ImGui::PopStyleColor();

            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Invite")) {
            for (auto inviteIt = data.incomingRequests.begin(); inviteIt != data.incomingRequests.end();)
            {
                if (strcmp(inviteIt->c_str(), "") == 0) {
                    continue;
                }
                ImGui::PushID(inviteIt->c_str());

                ImGui::Selectable(inviteIt->c_str(), false);

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::Button("Send message")) {
                        evBus->PostEvent<Events::ShowMessagesWindowEvent>({
                                                                                  .user = *inviteIt
                                                                          });
                        ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                        ImGui::PopID();
                        continue;
                    }
                    if (ImGui::Button("Accept")) {
                        evBus->PostEvent<ConnectorEvents::AcceptFriendInviteEvent>({
                            .fromUser = *inviteIt
                        });
                        inviteIt = data.incomingRequests.erase(inviteIt);
                        ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                        ImGui::PopID();
                        continue;
                    }
                    if (ImGui::Button("Reject")) {
                        inviteIt = data.incomingRequests.erase(inviteIt);
                        ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                        ImGui::PopID();
                        continue;
                    }

                    if (ImGui::Button("Ban"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
                ++inviteIt;
                ImGui::PopID();
            }
        }

        if (ImGui::CollapsingHeader("Requesting invites")) {
            for (auto inviteIt = data.outgoingRequests.begin(); inviteIt != data.outgoingRequests.end();)
            {
                if (strcmp(inviteIt->c_str(), "") == 0) {
                    continue;
                }
                ImGui::PushID(inviteIt->c_str());

                ImGui::Selectable(inviteIt->c_str(), false);

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::Button("Send message")) {
                        evBus->PostEvent<Events::ShowMessagesWindowEvent>({
                                                                                  .user = *inviteIt
                                                                          });
                        ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                        ImGui::PopID();
                        continue;
                    }
                    ImGui::EndPopup();
                }
                ++inviteIt;
                ImGui::PopID();
            }
        }
        ImGui::End();
    }


} // namespace Windows
