# 生存割草模式 — TODO 待办事项

**项目**: 像素勇者 (Pixel Hero Adventure)
**日期**: 2026-06-10
**版本**: V2.0

---

## 缺陷状态

**全部 P0/P1/P2 已修复，无已知缺陷。**

### 已修复清单

| 编号 | 级别 | 问题 | 修复 |
|:--:|:--:|------|------|
| B1 | P0 | SurvivalPlayer Q_OBJECT缺失 | 无信号，升级由轮询处理 |
| B2 | P0 | startGame() 重复connect timer | 删除重复connect |
| B3 | P0 | 升级流程竞态 | pendingLevelUps延迟模式 |
| F1 | P1 | 读档不检查存档 | Menu::setItemDisabled |
| F2 | P1 | SurvivalStats kills不同步 | addKill() |
| E1 | P2 | 读档计时从0开始 | elapsedTime参数 |
| E2 | P2 | 升级后不恢复游戏 | onSkillSelected末尾resumeGame |
| — | — | 按键事件被吞噬 | eventFilter返回true |
| — | — | SelectUI不可见 | viewport.update()强制刷新 |
| — | — | 缺少DLL无法启动 | dev.bat自动windeployqt |

---

## 后续增强建议

| 优先级 | 内容 | 说明 |
|:--:|------|------|
| 低 | 敌人/角色/武器贴图 | 当前用纯色方块，可替换为像素精灵图 |
| 低 | 音效系统 | BGM + 攻击/技能/升级音效 |
| 低 | 更多武器 | 当前4把，可扩展（斧/弓/弩/拳套等） |
| 低 | 更多角色 | 当前3个，可扩展（刺客/圣骑/召唤师等） |
| 低 | 拾取物系统 | 击杀敌人掉落血瓶/金币/经验球 |
| 低 | 难度选择 | 简单/普通/困难 影响敌人stats |
| 低 | 单元测试 | 战斗/升级/存档核心逻辑覆盖 |

---

## 缺失配置

| 项 | 说明 | 操作指引 |
|------|------|---------|
| 音效 | 未实现，不在当前范围 | 后续可引入 QMediaPlayer |
| 单元测试 | 未编写 | 手动验证游戏流程 |
| CI/CD | 未配置 | 无需求 |

---

## 一键构建

```bash
.\scripts\dev.bat    # 编译+部署DLL+运行
/dev                 # Trae 项目命令
```

---

**文档状态**: ✅ V2.0 完成
