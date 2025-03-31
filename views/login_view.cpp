#include "imgui.h"


#include "login_view.h"




namespace Windows {

    LoginView::LoginView(Event::EventBus* eventBus, UIModel::LoginMainModel login)
        : evBus(eventBus), data(std::move(login)) {
    }


    void LoginView::ShowWindow(bool& check) {
        ImGui::Begin("Login", &check);

        const char* items[] = { "Developer connect"};
        static int item_current = 0;
        ImGui::Combo("Choose authorization provider", &item_current, items, IM_ARRAYSIZE(items));

        static char inputBuffer[256] = "";  // Буфер для ввода текста (статический)

        ImGui::InputText("Login", inputBuffer, 256);
        // todo change button's id
        if (ImGui::Button("Logins")) {
            evBus->PostEvent<ConnectorEvents::LoginEvent>({
                .login = {
                        .provider = UIModel::AuthorizationProvider::DEVELOPER,
                        .userName = inputBuffer
                        }
            });
        }

        ImGui::End();
    }

} // namespace Windows
