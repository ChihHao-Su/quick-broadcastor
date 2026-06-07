#include "notifyWin.hxx"

#ifdef _WIN32
#include <FL/x.H>
#include <windows.h>
#endif

namespace {
constexpr int windowWidth = 360;
constexpr int windowHeight = 132;
constexpr int margin = 18;
constexpr int edgeGap = 18;
constexpr int hiddenGap = 8;
constexpr double appearanceDuration = 0.24;
constexpr double appearanceFrameInterval = 1.0 / 60.0;

void keepOnTop(Fl_Window &window) {
#ifdef _WIN32
    if (window.shown()) {
        SetWindowPos(fl_xid(&window), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
#else
    window.set_non_modal();
#endif
}
}

NotifyWindow::NotifyWindow()
    : Fl_Window{ windowWidth, windowHeight },
      senderBox{ margin, 14, windowWidth - margin * 2, 26 },
      contentBox{ margin, 48, windowWidth - margin * 2, 66 } {
    clear_border();
    color(fl_rgb_color(31, 35, 45));

    senderBox.box(FL_NO_BOX);
    senderBox.labelfont(FL_HELVETICA_BOLD);
    senderBox.labelsize(15);
    senderBox.labelcolor(fl_rgb_color(245, 247, 250));
    senderBox.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);

    contentBox.box(FL_NO_BOX);
    contentBox.labelsize(14);
    contentBox.labelcolor(fl_rgb_color(214, 219, 230));
    contentBox.align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_WRAP);

    end();
}

void NotifyWindow::showMessage(std::string sender, std::string content) {
    senderText = std::move(sender);
    contentText = std::move(content);

    senderBox.copy_label(senderText.c_str());
    contentBox.copy_label(contentText.c_str());

    const auto screen = Fl::screen_num(Fl::event_x_root(), Fl::event_y_root());
    int screenX = 0;
    int screenY = 0;
    int screenWidth = 0;
    int screenHeight = 0;
    Fl::screen_work_area(screenX, screenY, screenWidth, screenHeight, screen);

    beginAppearance(
        screenX + screenWidth - windowWidth - edgeGap,
        screenY + screenHeight - windowHeight - edgeGap);

    Fl::remove_timeout(closeTimeout, this);
    Fl::add_timeout(8.0, closeTimeout, this);
}

void NotifyWindow::closeTimeout(void *userdata) {
    auto &window = UNWRAP(static_cast<NotifyWindow *>(userdata));
    window.stopAppearance();
    window.hide();
}

void NotifyWindow::appearanceTick(void *userdata) {
    auto &window = UNWRAP(static_cast<NotifyWindow *>(userdata));
    if (!window.appearanceActive) {
        return;
    }

    const auto elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - window.appearanceStartTime).count();
    const auto progress = std::clamp(elapsed / appearanceDuration, 0.0, 1.0);
    const auto easedProgress = 1.0 - (1.0 - progress) * (1.0 - progress);
    const auto nextX = window.appearanceStartX
        + static_cast<int>((window.appearanceTargetX - window.appearanceStartX) * easedProgress);

    window.position(nextX, window.appearanceTargetY);
    keepOnTop(window);

    if (progress >= 1.0) {
        window.stopAppearance();
        window.position(window.appearanceTargetX, window.appearanceTargetY);
        return;
    }

    Fl::repeat_timeout(appearanceFrameInterval, appearanceTick, userdata);
}

void NotifyWindow::beginAppearance(int targetXIn, int targetYIn) {
    stopAppearance();

    appearanceTargetX = targetXIn;
    appearanceTargetY = targetYIn;
    appearanceStartX = visible() ? x() : targetXIn + windowWidth + hiddenGap;
    appearanceStartTime = std::chrono::steady_clock::now();
    appearanceActive = true;

    position(appearanceStartX, appearanceTargetY);
    show();
    keepOnTop(*this);
    Fl::add_timeout(0.0, appearanceTick, this);
}

void NotifyWindow::stopAppearance() {
    if (!appearanceActive) {
        return;
    }
    appearanceActive = false;
    Fl::remove_timeout(appearanceTick, this);
}
