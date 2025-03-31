#pragma once
#include <memory>

#include "event_bus/event_bus.h"


namespace Windows {


    struct IWindow {
        virtual ~IWindow() {}
        virtual void ShowWindow(bool& check) = 0;
    };


    struct IndependentWindow {
        std::unique_ptr<IWindow> window;
        bool isWinActive = false;
    };


} // namespace Windows
