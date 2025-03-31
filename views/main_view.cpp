#include "main_view.h"


namespace Windows {

    MainView::MainView(Event::EventBus* eventBus)
        : evBus(eventBus), loginWindow(eventBus, {}),
          friendsWindow(eventBus, {}), logsWindow(eventBus, {}),
          messagesWindow(eventBus, {"Furis", {}}),
          profileWindow(eventBus, {.login = "Furis"}) {
        Event::Listener showProfileWindowEventListener;
        showProfileWindowEventListener.Listen<Events::ShowProfileWindowEvent>(
            [this](const Events::ShowProfileWindowEvent& event) {
                this->ShowProfileWindowEvent(event);
            });
        evBus->AddListener(std::move(showProfileWindowEventListener));

        Event::Listener ShowMessagesWindowEventListener;
        ShowMessagesWindowEventListener.Listen<Events::ShowMessagesWindowEvent>(
            [this](const Events::ShowMessagesWindowEvent& event) {
                this->ShowMessagesWindowEvent(event);
            });
        evBus->AddListener(std::move(ShowMessagesWindowEventListener));

        Event::Listener successLoginEventListener;
        successLoginEventListener.Listen<Events::SuccessLoginEvent>(
                [this](const Events::SuccessLoginEvent& event) {
                    this->SuccessLoginEvent(event);
                });
        evBus->AddListener(std::move(successLoginEventListener));
    }

    void MainView::ShowProfileWindowEvent(const Events::ShowProfileWindowEvent& event) {
        independentWindows.push_back({
            .window = std::make_unique<ProfileView>(ProfileView{this->evBus, event.profile}),
            .isWinActive = true
        });
    }


    void MainView::ShowMessagesWindowEvent(const Events::ShowMessagesWindowEvent& event) {
        messagesWindow.SetActiveUser(event.user);
        messagesCheck = true;
    }


    void MainView::SuccessLoginEvent(const Events::SuccessLoginEvent &event) {
        isUserLogged = true;
        friendsWindow.SetUp();
    }


    void MainView::ShowWindow() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Services")) {
                if (isUserLogged) {
                    if (ImGui::Checkbox("My profile", &profileCheck)) {
                        // wait for ShowMainUserProfileWindowEvent
                        profileCheck = false;
                        evBus->PostEvent<ConnectorEvents::ShowMainUserProfileEvent>({});
                    }
                    ImGui::Checkbox("Friends", &friendsCheck);
                    ImGui::Checkbox("Messages", &messagesCheck);
                } else {
                    ImGui::Checkbox("Login", &loginCheck);
                }

                ImGui::Checkbox("Logs", &logsCheck);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (loginCheck && !isUserLogged) {
            loginWindow.ShowWindow(loginCheck);
        }

        if (friendsCheck && isUserLogged) {
            friendsWindow.ShowWindow(friendsCheck);
        }

        if (logsCheck) {
            logsWindow.ShowWindow(logsCheck);
        }

        if (messagesCheck && isUserLogged) {
            messagesWindow.ShowWindow(messagesCheck);
        }

        if (profileCheck && isUserLogged) {
            profileWindow.ShowWindow(profileCheck);
        }

        if (isUserLogged) {
            for (IndependentWindow &window: independentWindows) {
                if (window.isWinActive) {
                    window.window->ShowWindow(window.isWinActive);
                } else {
                    //todo delete element
                }
            }
        }
    }

} // namespace Windows
