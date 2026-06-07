#include "trayFunctionality.hxx"
#include "EatiEssentials/codecvt.hxx"

// Include here directly instead of in stdafx.h, due to the dependency of
// WIN32 API.
#include <tray.h>

#if TRAY_APPINDICATOR
#define TRAY_ICON "indicator-messages"
#elif TRAY_APPKIT
#define TRAY_ICON "icon/QuickBroadcastor.png"
#elif TRAY_WINAPI
#define TRAY_ICON "assets/icon/QuickBroadcastor.ico"
#endif

namespace {
constexpr double trayPollInterval = 0.1;
}

TrayFunctionality::TrayFunctionality(Action onOpenIn, Action onExitIn)
    : openAction{ std::move(onOpenIn) },
      exitAction{ std::move(onExitIn) } {

    using namespace Essentials::String;
    static auto text1 = utf8ToSysApiEncoding("打開發送面板");
    static auto text2 = utf8ToSysApiEncoding("退出");


    static tray_menu menu[] = {
        { const_cast<char *>(text1.c_str()), 0, 0, openClicked, nullptr },
        { const_cast<char *>("-"), 0, 0, nullptr, nullptr },
        { const_cast<char *>(text2.c_str()), 0, 0, exitClicked, nullptr },
        { nullptr, 0, 0, nullptr, nullptr }
    };
    static tray trayState {
        const_cast<char *>(TRAY_ICON),
        menu
    };
    for (auto *item = menu; item->text; ++item) {
        item->context = this;
    }

    if (tray_init(&trayState) < 0) {
        throw std::runtime_error("failed to initialize tray");
    }

    Fl::add_timeout(trayPollInterval, pollTray, this);
}

TrayFunctionality::~TrayFunctionality() {
    Fl::remove_timeout(pollTray, this);
    tray_exit();
}

void TrayFunctionality::pollTray(void *userdata) {
    tray_loop(0);
    Fl::repeat_timeout(trayPollInterval, pollTray, userdata);
}

void TrayFunctionality::openClicked(tray_menu *item) {
    auto &menuItem = UNWRAP(item);
    auto &functionality = UNWRAP(static_cast<TrayFunctionality *>(menuItem.context));
    if (functionality.openAction) {
        functionality.openAction();
    }
}

void TrayFunctionality::exitClicked(tray_menu *item) {
    auto &menuItem = UNWRAP(item);
    auto &functionality = UNWRAP(static_cast<TrayFunctionality *>(menuItem.context));
    if (functionality.exitAction) {
        functionality.exitAction();
    }
}
