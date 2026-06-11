QT       += core gui widgets

TARGET = pixel_hero
TEMPLATE = app

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

RESOURCES += resources.qrc

# MinGW 16.1.0 compatibility: suppress QT_NEEDS_QMAIN / Qt6EntryPoint
win32:!win32-msvc* {
    DEFINES -= QT_NEEDS_QMAIN
    QMAKE_LIBS_QT_ENTRY =
}

SOURCES += \
    src/main.cpp \
    src/GameWindow.cpp \
    src/entities/Player.cpp \
    src/entities/Enemy.cpp \
    src/utils/GameData.cpp \
    src/utils/ResourceManager.cpp \
    src/utils/AnimationManager.cpp \
    src/ui/SelectableListBase.cpp \
    src/ui/Menu.cpp \
    src/ui/CharacterSelectUI.cpp \
    src/ui/WeaponSelectUI.cpp \
    src/ui/SaveLoadUI.cpp \
    src/survival/SurvivalScene.cpp \
    src/survival/WaveManager.cpp \
    src/survival/UpgradeUI.cpp \
    src/survival/SurvivalHUD.cpp \
    src/survival/SurvivalPlayer.cpp \
    src/survival/SurvivalStats.cpp \
    src/survival/EnemyFactory.cpp \
    src/survival/EffectManager.cpp \
    src/survival/EnemyManager.cpp \
    src/survival/PickupItem.cpp \
    src/survival/PickupManager.cpp \
    src/survival/skills/SkillExecutor.cpp

HEADERS += \
    src/GameWindow.h \
    src/entities/Player.h \
    src/entities/Enemy.h \
    src/entities/Weapon.h \
    src/utils/GameData.h \
    src/utils/ResourceManager.h \
    src/utils/AnimationManager.h \
    src/ui/SelectableListBase.h \
    src/ui/Menu.h \
    src/ui/CharacterSelectUI.h \
    src/ui/WeaponSelectUI.h \
    src/ui/SaveLoadUI.h \
    src/survival/SurvivalScene.h \
    src/survival/WaveManager.h \
    src/survival/UpgradeUI.h \
    src/survival/SurvivalHUD.h \
    src/survival/SurvivalPlayer.h \
    src/survival/SurvivalStats.h \
    src/survival/SurvivalSaveData.h \
    src/survival/EnemyFactory.h \
    src/survival/EffectManager.h \
    src/survival/EnemyManager.h \
    src/survival/PickupItem.h \
    src/survival/PickupManager.h \
    src/survival/skills/SkillExecutor.h \
    src/config/GameConfig.h

RESOURCES += \
    resources.qrc

INCLUDEPATH += src \
               src/entities \
               src/ui \
               src/utils \
               src/survival \
               src/survival/skills \
               src/config

DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui
