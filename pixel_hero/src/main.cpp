#include <QtWidgets/QApplication>
#include <ctime>
#include <cstdlib>
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
    // 初始化随机数种子（敌人巡逻AI依赖）
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    QApplication app(argc, argv);
    app.setApplicationName("PixelHero");
    app.setOrganizationName("PixelHero");
    GameWindow window;
    window.show();
    
    return app.exec();
}