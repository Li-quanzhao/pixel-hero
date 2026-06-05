# 像素勇者

基于 Qt 6 + C++17 的 2D 像素风冒险游戏。

## 技术栈

- **语言**: C++17
- **框架**: Qt 6.11.1
- **渲染**: QGraphicsView/QGraphicsScene
- **编译器**: MinGW 13.1.0

## 项目结构

```
pixel_hero/
├── src/                  # 源代码
│   ├── main.cpp          # 入口
│   ├── GameWindow.cpp    # 主窗口
│   ├── GameScene.cpp     # 游戏场景
│   ├── GameController.cpp# 游戏控制
│   ├── entities/         # 游戏实体
│   ├── map/              # 地图系统
│   ├── battle/           # 战斗系统
│   ├── ui/               # UI 组件
│   └── utils/            # 工具类
├── resources/            # 资源文件
├── pixel_hero.pro        # Qt 项目配置
└── README.md
```

## 编译运行

### Windows (MinGW)

```powershell
# 设置环境变量
$env:PATH = "E:\QT\Tools\mingw1310_64\bin;E:\QT\6.11.1\mingw_64\bin;" + $env:PATH

# 构建项目
cd pixel_hero
qmake.exe pixel_hero.pro -r
mingw32-make -j4

# 部署 Qt 依赖
windeployqt.exe bin/pixel_hero.exe

# 运行
bin/pixel_hero.exe
```

## 操作说明

| 按键 | 功能 |
|------|------|
| W / ↑ | 向上移动 |
| S / ↓ | 向下移动 |
| A / ← | 向左移动 |
| D / → | 向右移动 |
| Space | 攻击 |
| Esc | 暂停/继续 |

## 开发规范

详见 [技术规范文档.md](https://github.com/your-username/pixel-hero/blob/main/技术规范文档.md)