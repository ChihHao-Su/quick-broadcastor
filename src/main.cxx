#include "msg.hxx"
#include "networkFunctionality.hxx"
#include "notifyWin.hxx"
#include "sendWin.hxx"
#include "trayFunctionality.hxx"

int appmain(int argc, char **argv) {
    Fl::scheme("gtk+");
    Fl::visual(FL_DOUBLE | FL_INDEX);

    try {
        NotifyWindow notifyWindow;
        MessageHandler messageHandler{
            [&notifyWindow](BroadcastMessage const &message) {
                auto sender = message.name.empty() ? message.host : message.name;
                if (!message.name.empty() && !message.host.empty()) {
                    sender += " @ ";
                    sender += message.host;
                }
                notifyWindow.showMessage(std::move(sender), message.content);
            }
        };
        NetworkFunctionality networkFunctionality{
            [&messageHandler](std::string_view content) {
                messageHandler.process(content);
            }
        };
        SendWindow sendWindow{
            [&networkFunctionality, &messageHandler](std::string_view name, std::string_view message) {
                auto broadcastMessage = makeBroadcastMessage(name, message);
                messageHandler.remember(broadcastMessage.id);
                networkFunctionality.broadcast(serializeMessage(broadcastMessage));
            },
            [&notifyWindow](std::string_view name, std::string_view message) {
                notifyWindow.showMessage(std::string(name), std::string(message));
            }
        };
        TrayFunctionality trayFunctionality{
            [&sendWindow] {
                sendWindow.show();
                sendWindow.take_focus();
            },
            [] {
                Fl::program_should_quit(1);
            }
        };

        sendWindow.show(argc, argv);
        while (!Fl::program_should_quit()) {
            Fl::wait(1e20);
        }
    } catch (std::exception const &error) {
        fl_alert("QuickBroadcastor failed to start:\n%s", error.what());
        return 1;
    }

    return 0;
}
