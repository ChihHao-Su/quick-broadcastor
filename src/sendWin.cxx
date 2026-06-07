#include "sendWin.hxx"

namespace {
constexpr int windowWidth = 480;
constexpr int windowHeight = 360;

std::string trimCopy(std::string_view value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos) {
        return {};
    }

    const auto last = value.find_last_not_of(" \t\r\n");
    return std::string(value.substr(first, last - first + 1));
}
}

SendWindow::SendWindow(SendCallback onSendIn, SendCallback onPreviewIn)
    : Fl_Window{ windowWidth, windowHeight, "QuickBroadcast" },
      contentLayout{ 0, 0, windowWidth, windowHeight, Fl_Flex::VERTICAL },
      title{ 0, 0, 0, 0, "QuickBroadcast" },
      subtitle{ 0, 0, 0, 0, "Send a short LAN broadcast to all listening devices." },
      nameLabel{ 0, 0, 0, 0, "Name" },
      nameInput{ 0, 0, 0, 0 },
      messageLabel{ 0, 0, 0, 0, "Message" },
      messageInput{ 0, 0, 0, 0 },
      actionLayout{ 0, 0, 0, 0, Fl_Flex::HORIZONTAL },
      hideButton{ 0, 0, 0, 0, "Hide" },
      testNotifyButton{ 0, 0, 0, 0, "Preview" },
      actionSpacer{ 0, 0, 0, 0 },
      sendButton{ 0, 0, 0, 0, "Broadcast" },
      sendCallback{ std::move(onSendIn) },
      previewCallback{ std::move(onPreviewIn) } {
    color(fl_rgb_color(246, 248, 252));
    contentLayout.margin(28, 22, 28, 18);
    contentLayout.gap(8);

    title.box(FL_NO_BOX);
    title.labelfont(FL_HELVETICA_BOLD);
    title.labelsize(24);
    title.labelcolor(fl_rgb_color(29, 35, 48));
    title.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);

    subtitle.box(FL_NO_BOX);
    subtitle.labelsize(13);
    subtitle.labelcolor(fl_rgb_color(102, 112, 133));
    subtitle.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);

    nameLabel.box(FL_NO_BOX);
    nameLabel.labelfont(FL_HELVETICA_BOLD);
    nameLabel.labelsize(13);
    nameLabel.labelcolor(fl_rgb_color(52, 64, 84));
    nameLabel.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);

    nameInput.textsize(14);
    nameInput.value("QuickBroadcast");

    messageLabel.box(FL_NO_BOX);
    messageLabel.labelfont(FL_HELVETICA_BOLD);
    messageLabel.labelsize(13);
    messageLabel.labelcolor(fl_rgb_color(52, 64, 84));
    messageLabel.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);

    messageInput.textsize(14);
    messageInput.wrap(1);

    actionLayout.gap(12);
    actionSpacer.box(FL_NO_BOX);

    sendButton.color(fl_rgb_color(37, 99, 235));
    sendButton.labelcolor(FL_WHITE);
    sendButton.labelfont(FL_HELVETICA_BOLD);
    sendButton.callback(sendClicked, this);

    testNotifyButton.callback(testNotifyClicked, this);
    hideButton.callback(closeClicked, this);

    actionLayout.fixed(hideButton, 104);
    actionLayout.fixed(testNotifyButton, 104);
    actionLayout.fixed(sendButton, 128);
    actionLayout.end();

    contentLayout.fixed(title, 34);
    contentLayout.fixed(subtitle, 24);
    contentLayout.fixed(nameLabel, 22);
    contentLayout.fixed(nameInput, 34);
    contentLayout.fixed(messageLabel, 22);
    contentLayout.fixed(actionLayout, 36);
    contentLayout.end();

    callback(closeClicked, this);
    resizable(contentLayout);
    size_range(360, 300);
    end();
}

void SendWindow::sendClicked(Fl_Widget *, void *userdata) {
    UNWRAP(static_cast<SendWindow *>(userdata)).handleSend();
}

void SendWindow::testNotifyClicked(Fl_Widget *, void *userdata) {
    UNWRAP(static_cast<SendWindow *>(userdata)).handleTestNotify();
}

void SendWindow::closeClicked(Fl_Widget *, void *userdata) {
    UNWRAP(static_cast<SendWindow *>(userdata)).hide();
}

void SendWindow::handleSend() {
    auto name = trimCopy(nameInput.value());
    auto message = trimCopy(messageInput.value());
    if (name.empty()) {
        nameInput.take_focus();
        return;
    }
    if (message.empty()) {
        messageInput.take_focus();
        return;
    }

    if (sendCallback) {
        sendCallback(name, message);
    }
    messageInput.value("");
    messageInput.take_focus();
}

void SendWindow::handleTestNotify() {
    auto name = trimCopy(nameInput.value());
    if (name.empty()) {
        name = "QuickBroadcast";
    }

    if (previewCallback) {
        previewCallback(name, "This is a preview notification.");
    }
}
