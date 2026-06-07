#pragma once

#include "stdafx.h"

class SendWindow final : public Fl_Window
{
public:
    using SendCallback = std::function<void(std::string_view name, std::string_view message)>;

    SendWindow(SendCallback onSendIn, SendCallback onPreviewIn);

private:
    static void sendClicked(Fl_Widget *widget, void *userdata);
    static void testNotifyClicked(Fl_Widget *widget, void *userdata);
    static void closeClicked(Fl_Widget *widget, void *userdata);
    void handleSend();
    void handleTestNotify();

    Fl_Flex contentLayout;
    Fl_Box title;
    Fl_Box subtitle;
    Fl_Box nameLabel;
    Fl_Input nameInput;
    Fl_Box messageLabel;
    Fl_Multiline_Input messageInput;
    Fl_Flex actionLayout;
    Fl_Button hideButton;
    Fl_Button testNotifyButton;
    Fl_Box actionSpacer;
    Fl_Button sendButton;
    SendCallback sendCallback;
    SendCallback previewCallback;
};
