# 生存割草模式 — ACCEPTANCE 验收文档

**项目**: 像素勇者 (Pixel Hero Adventure)
**文档版本**: ACCEPTANCE V2.0
**日期**: 2026-06-10
**阶段**: 阶段5+6 — 自动化执行 + 评估
**前置文档**: [TASK_生存割草模式.md](./TASK_生存割草模式.md)

---

## 一、逐任务验收

### Task 1: JSON 数据文件 — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| skills.json 存在 | 5技能×5等级 | fireball/lightning/frost_nova/attack_boost/speed_boost 各5级 | ✅ |
| survival_waves.json 存在 | 20波+6敌人 | 20波，5/10/13/16/20标记精英/Boss | ✅ |
| weapons.json 存在 | 4武器 | 短剑/长剑/法杖/匕首 | ✅ |
| JSON 格式合法 | 有效 JSON | 有效 | ✅ |
| 6种敌人 baseStats | 完整 | slime/goblin/skeleton/bat/goblin_elite/dragon | ✅ |

### Task 2: GameData 扩展 — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| loadSkillData | 加载5技能 | ✅ | ✅ |
| getSkillById("fireball") | 返回5级数据 | ✅ | ✅ |
| loadWaveData | 加载20波 | ✅ | ✅ |
| loadWeaponData | 加载4武器 | ✅ `GameData.cpp:103-148` | ✅ |
| getWeaponById("dagger") | 返回匕首数据 | ✅ speedMod=1.5 | ✅ |
| JSON异常回退硬编码 | initDefaultSkills/Waves/Weapons | ✅ | ✅ |

### Task 3: 删除旧代码 + 改造 Player — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| 14个旧文件删除 | GameScene/Item/Armor/NPC/BattleSystem/Weapon/MapManager/Tile/CollisionLayer/HUD/Inventory | 全部已删除 | ✅ |
| Player 无 Weapon/Armor/Inventory 成员 | 基础属性+移动+战斗+经验 | ✅ | ✅ |
| Player 新 WeaponData m_weapon | applyWeapon()/attackSpeed() | ✅ `Player.h:38-40` | ✅ |
| Player 攻击受武器加成 | totalAtk = m_attack + weapon.attackBonus | ✅ | ✅ |
| 攻速受 speedMod 影响 | cooldown /= weapon.speedMod | ✅ | ✅ |
| 编译通过 | 无缺失引用 | ✅ | ✅ |
| .pro 无旧路径 | battle/map 不在列表中 | ✅ | ✅ |

### Task 4: Enemy 扩展 — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| EnemyType 6种枚举 | GOBLIN/SLIME/SKELETON/BAT/GOBLIN_ELITE/DRAGON | ✅ `Enemy.h` | ✅ |
| 编译通过 | 无错误 | ✅ | ✅ |

### Task 5: EnemyFactory — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| createEnemy("slime") hp=20 | 20 | ✅ | ✅ |
| createEnemy("bat") spd=3.0 | 3.0 | ✅ | ✅ |
| createEnemy("dragon") hp=500 | 500 | ✅ | ✅ |

### Task 6: WaveManager — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| 开局生成史莱姆×8 | 间隔3秒 | ✅ Wave1配置正确 | ✅ |
| 第5波精英 | waveChanged(5)+精英 | ✅ isEliteWave=true | ✅ |
| 第10波Boss | bossSpawned | ✅ isBossWave=true | ✅ |
| addKill累计 | m_totalKills++ | ✅ (已修复) | ✅ |

### Task 7: SurvivalPlayer — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| 构造 HP=100/atk=10/def=3/spd=3 | 100/10/3/3 | ✅ | ✅ |
| availableCharacters() 3角色 | 战士/弓箭手/法师 | ✅ | ✅ |
| applyCharacter 应用属性+颜色 | HP/ATK/DEF/SPD | ✅ | ✅ |
| addSkill("attack_boost",3) attack()==28 | 28 | ✅ | ✅ |
| 武器加成 attack() | 基础+被动+武器 | ✅ | ✅ |
| 升级检查 pendingLevelUps | 仅计数延迟处理 | ✅ | ✅ |

### Task 8: SurvivalHUD — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| HP血条+数值 | HP 80/100 | ✅ | ✅ |
| 等级+经验条 | Lv.5 XP 45/80 | ✅ | ✅ |
| 计时 mm:ss / 波次 Wave X / 击杀数 | 全部显示 | ✅ | ✅ |

### Task 9: UpgradeUI — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| 3张升级卡片 | 随机3技能 | ✅ | ✅ |
| 按数字键选择 | 1/2/3 键 | ✅ | ✅ |
| 满级显示"已满" | 描述处理 | ✅ | ✅ |

### Task 10: SurvivalScene — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| startGame(characterId, weaponId) | 传参启动 | ✅ | ✅ |
| 应用角色配置 | applyCharacter | ✅ | ✅ |
| 应用武器配置 | applyWeapon | ✅ | ✅ |
| HP=0→endGame | 死亡结算 | ✅ | ✅ |
| 编译零错误 | mingw32-make | ✅ | ✅ |

### Task 11: SurvivalStats — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| 中途存档 | saveCurrentGame | ✅ | ✅ |
| 读档恢复 | loadSavedGame→startFromSave | ✅ | ✅ |
| 破纪录显示 | isNewRecord | ✅ | ✅ |
| kills实时同步 | addKill | ✅ | ✅ |

### Task 12: GameWindow + Menu — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| 主菜单3选项 | 新游戏/读档/退出 | ✅ | ✅ |
| 角色选择界面 | CharacterSelectUI 3卡片 | ✅ | ✅ |
| 武器选择界面 | WeaponSelectUI 4卡片 | ✅ | ✅ |
| 完整流程: 菜单→角色→武器→游戏 | 独立UI各自键盘处理 | ✅ | ✅ |
| eventFilter 返回true阻止传播 | 防止事件被QGraphicsView吞噬 | ✅ | ✅ |
| viewport强制刷新 | 每次hide/show后update | ✅ | ✅ |
| 读档无存档时禁用 | setItemDisabled | ✅ | ✅ |
| ESC暂停→保存退出 | showPauseMenu | ✅ | ✅ |

### Task 13: 编译配置 — ✅ 通过

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| qmake 成功 | Makefile生成 | ✅ | ✅ |
| mingw32-make 零错误 | exit 0 | ✅ | ✅ |
| dev.bat 一键编译+部署+运行 | 5步自动 | ✅ | ✅ |
| windeployqt 自动部署DLL | Qt6/MingGW DLL | ✅ | ✅ |
| /dev Trae项目命令 | scripts\dev.bat | ✅ | ✅ |

---

## 二、新增功能验收 (V2.0)

### 武器系统

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| WeaponData 数据结构 | id/name/attackBonus/speedMod/rangeBonus/skillBonus/color/desc | ✅ `Weapon.h` | ✅ |
| weapons.json 4武器 | 短剑/长剑/法杖/匕首 | ✅ | ✅ |
| GameData::loadWeaponData | JSON加载+硬编码回退 | ✅ | ✅ |
| Player::applyWeapon | 应用武器属性 | ✅ | ✅ |
| 攻击伤害含武器加成 | totalAtk = m_attack + weapon.attackBonus | ✅ | ✅ |
| 攻速受speedMod影响 | cooldown /= speedMod | ✅ | ✅ |

### 角色选择 (CharacterSelectUI)

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| 3角色: 战士/弓箭手/法师 | 各不同HP/ATK/DEF/SPD | ✅ | ✅ |
| 属性条形图 | HP红/ATK橙/DEF蓝/SPD绿 | ✅ | ✅ |
| 角色颜色预览 | 方块颜色显示 | ✅ | ✅ |
| 卡片居中排列 | 3卡片水平居中 | ✅ | ✅ |
| A/D ← → 切换 Enter确认 | 键盘操作 | ✅ | ✅ |

### 武器选择 (WeaponSelectUI)

| 项 | 预期 | 实际 | 状态 |
|----|------|------|:--:|
| 4武器: 短剑/长剑/法杖/匕首 | 各不同攻速/范围/技能加成 | ✅ | ✅ |
| 属性行: 攻击/攻速/范围/技能 | 数值+颜色标识 | ✅ | ✅ |
| 攻速正值绿色 负值红色 | 颜色区分 | ✅ | ✅ |
| 武器颜色预览 | 方块显示 | ✅ | ✅ |
| A/D ← → 切换 Enter确认 | 键盘操作 | ✅ | ✅ |

---

## 三、CONSENSUS 验收标准对照

| # | 标准 | 结果 |
|:--:|------|:--:|
| AC1 | 启动→主菜单3选项 | ✅ |
| AC2 | HP=0触发结算 | ✅ |
| AC3 | 杀敌→经验→升级→三选一 | ✅ |
| AC4 | 5技能3主动+2被动 | ✅ |
| AC5 | 6种敌人渐进生成 | ✅ |
| AC6 | 每5波精英/每10波Boss | ✅ |
| AC7 | 结算：时间/波次/击杀 | ✅ |
| AC8 | 编译零错误 | ✅ |
| AC9 | 无故事模式代码 | ✅ |
| AC10 | 最高纪录存盘 | ✅ |
| AC11 | 中途存档+读档 | ✅ |
| AC12 | 全中文界面 | ✅ |
| AC13 | 角色选择→武器选择→游戏 | ✅ 新增 |

**通过率**: 13/13 ✅

---

## 四、已修复缺陷

| # | 问题 | 修复 |
|:--:|------|------|
| B2 | startGame() 重复 connect timer | 已删除重复connect |
| B3 | 升级流程竞态 | pendingLevelUps延迟模式 |
| F1 | 读档不检查存档 | setItemDisabled |
| F2 | SurvivalStats kills不同步 | addKill |
| E1 | 读档计时从0开始 | elapsedTime参数传递 |
| — | SelectUI不可见 | viewport.update()强制刷新 |
| — | 按键事件被QGraphicsView吞噬 | eventFilter返回true |
| — | 无法启动(缺少DLL) | dev.bat自动windeployqt |

---

**文档状态**: ✅ V2.0 完成
