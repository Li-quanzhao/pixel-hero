QT       += core gui widgets

TARGET = pixel_hero
TEMPLATE = app

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    src/main.cpp \
    src/GameWindow.cpp \
    src/GameScene.cpp \
    src/GameController.cpp \
    src/entities/Player.cpp \
    src/entities/Enemy.cpp \
    src/entities/NPC.cpp \
    src/entities/Item.cpp \
    src/entities/Armor.cpp \
    src/map/MapManager.cpp \
    src/map/Tile.cpp \
    src/map/CollisionLayer.cpp \
    src/battle/BattleSystem.cpp \
    src/battle/Skill.cpp \
    src/battle/Weapon.cpp \
    src/ui/HUD.cpp \
    src/ui/Menu.cpp \
    src/ui/Inventory.cpp \
    src/utils/AnimationManager.cpp \
    src/utils/ResourceManager.cpp \
    src/utils/GameData.cpp \
    src/utils/SaveManager.cpp

HEADERS += \
    src/GameWindow.h \
    src/GameScene.h \
    src/GameController.h \
    src/entities/Player.h \
    src/entities/Enemy.h \
    src/entities/NPC.h \
    src/entities/Item.h \
    src/entities/Armor.h \
    src/map/MapManager.h \
    src/map/Tile.h \
    src/map/CollisionLayer.h \
    src/battle/BattleSystem.h \
    src/battle/Skill.h \
    src/battle/Weapon.h \
    src/ui/HUD.h \
    src/ui/Menu.h \
    src/ui/Inventory.h \
    src/utils/AnimationManager.h \
    src/utils/ResourceManager.h \
    src/utils/GameData.h \
    src/utils/SaveManager.h

RESOURCES += \
    resources.qrc

INCLUDEPATH += src \
               src/entities \
               src/map \
               src/battle \
               src/ui \
               src/utils

DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui