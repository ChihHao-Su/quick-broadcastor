#pragma once

#include "stdafx.h"

class NotifyWindow final : public Fl_Window
{
public:
    NotifyWindow();

    void showMessage(std::string sender, std::string content);

private:
    static void closeTimeout(void *userdata);
    static void appearanceTick(void *userdata);

    void beginAppearance(int targetXIn, int targetYIn);
    void stopAppearance();

    Fl_Box senderBox;
    Fl_Box contentBox;
    std::string senderText;
    std::string contentText;
    std::chrono::steady_clock::time_point appearanceStartTime;
    int appearanceStartX = 0;
    int appearanceTargetX = 0;
    int appearanceTargetY = 0;
    bool appearanceActive = false;
};
