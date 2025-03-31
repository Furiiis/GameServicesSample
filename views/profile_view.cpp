#include "imgui.h"

#include "profile_view.h"
#include "os/utils.h"
#include "imgui_internal.h"

namespace Windows {


    ProfileView::ProfileView(Event::EventBus* eventBus, UIModel::ProfileMainModel profile)
        : evBus(eventBus), data(std::move(profile)) {
        windowName = data.login + "'s profile";
    }


    void ProfileView::ShowWindow(bool& check) {
        ImGui::Begin(windowName.c_str(), &check);

        ImGui::Text("Login: ");

        ImGui::SameLine();
        ImGui::Text("%s", data.login.c_str());

        ImGui::Text("Auth ID: ");

        ImGui::SameLine();
        //ImGui::Text("%s", data.authId.c_str());
        if (ImGui::Selectable(data.authId.c_str(), false, ImGuiSelectableFlags_None)) {
            // Действие при выборе текста (если нужно)
        }

        // Копирование текста в буфер обмена
        if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0)) {
            ImGui::SetClipboardText(data.authId.c_str());
        }

        ImGui::Text("Last time online: ");

        ImGui::SameLine();
        ImGui::Text("%s", OS::Utils::GetTime(data.lastOnlineTimestamp).c_str());

        ImGui::End();
    }

} // namespace Windows
