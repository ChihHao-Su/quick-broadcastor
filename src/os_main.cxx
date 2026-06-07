#include <cstdio>
int appmain(int argc, char **argv);

#ifdef WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // https://gist.github.com/mojocorp/d1a0b400abd790f44a53
    // Redirect stdin, stdout, stderr to the console.
    gsl::owner<FILE *> conin{ nullptr };
    gsl::owner<FILE *> conout{ nullptr };

    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen_s(&conin, "CONIN$", "r", stdin);
        freopen_s(&conout, "CONOUT$", "w", stdout);
        freopen_s(&conout, "CONOUT$", "w", stderr);
    }

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    
    int ret = appmain(__argc, __argv);

    if (conin)
        fclose(conin);
    if (conout)
        fclose(conout);
    return ret;
}
#else
int main(int argc, char **argv) {
    return appmain(argc, argv);
}
#endif