#include <QtWidgets/QApplication>
#include "GameWindow.h"

#ifdef Q_OS_WIN
#include <windows.h>

// MinGW 16.1.0 兼容: libQt6EntryPoint.a 不兼容，手动提供 WinMain
extern "C" int main(int argc, char *argv[]);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // 用标准 main() 作为入口，QT_NEEDS_QMAIN 已在 Makefile.Release 中移除
    return main(__argc, __argv);
}
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    GameWindow window;
    window.show();
    
    return app.exec();
}