#pragma once

#include "stdafx.h"

class TrayFunctionality final
{
public:
    using Action = std::function<void()>;

    TrayFunctionality(Action onOpenIn, Action onExitIn);
    ~TrayFunctionality();

    TrayFunctionality(TrayFunctionality const &) = delete;
    TrayFunctionality &operator=(TrayFunctionality const &) = delete;
    TrayFunctionality(TrayFunctionality &&) = delete;
    TrayFunctionality &operator=(TrayFunctionality &&) = delete;

private:
    static void pollTray(void *userdata);
    static void openClicked(struct tray_menu *item);
    static void exitClicked(struct tray_menu *item);

    std::string iconPath;
    Action openAction;
    Action exitAction;
};
