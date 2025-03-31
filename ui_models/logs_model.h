#pragma once
#include <string>


#include "imgui.h"


namespace UIModel {


    struct LogMainModel {
        void Clear();
        void AddLog(const char* fmt, ...) IM_FMTARGS(2);
        void AddLog(const std::string& log);

        ImGuiTextBuffer     Buf;
        ImGuiTextFilter     Filter;
        ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLogEvent() calls.
    };


} // namespace UIModel


namespace Events {


    struct AddLogEvent {
        std::string log;
    };


} // namespace Events
