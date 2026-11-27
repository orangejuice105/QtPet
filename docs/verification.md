# QtPet 里程碑验收记录

本文档按开发里程碑记录各版本在真实环境下的构建、实机验收结果与后续待补项。

---

## v0.0.1 — 透明窗口原型

### 测试环境
- **操作系统**：Microsoft Windows 11 家庭中文版（Build 22631）
- **Qt Kit**：Desktop Qt 6.12.0 MSVC2022 64bit
- **构建方式**：CMake（Qt Creator）
- **当前验证素材**：内置资源 `:/pet/placeholder.gif`（`resources/pets/Luoxiaohei1.gif`，`QPixmap` 静态首帧绘制）

### 已实现范围
- 无边框逐像素透明窗口（`Qt::FramelessWindowHint` + `Qt::WA_TranslucentBackground`）
- 固定逻辑画布尺寸 `180 × 180`，在中间 `120 × 120` 区域绘制内置占位图
- 右键菜单（父对象为 `PetWindow`），支持可勾选的“置顶”切换与“退出”
- 运行时切换 `Qt::WindowStaysOnTopHint` 前记录 `isVisible()` 与 `pos()`，切换后恢复显示与原逻辑位置

### 实机验收清单
- [x] 无边框透明窗口正常显示，无系统边框与背景底色
- [x] 右键点击角色主体可正常弹出菜单
- [x] 透明外沿区域点击可穿透至下层窗口（原生分层窗口透明命中生效，暂无需额外 `setMask`）
- [x] 切换“置顶”开关生效，切换后窗口保持可见且停留在原位置
- [x] 点击“退出”可正常退出应用并结束进程
- [ ] 待补充：高 DPI（150% 缩放）专项复测记录与实机截图（截图可放至 `docs/images/` 目录并在本节引用）

### 结论与备忘
- 当前项目后续规划以 GIF 素材为主，首版已确认原生透明点击穿透正常；动画播放（`QMovie` / `FramePlayer`）、每帧透明清底与鼠标交互（单击、拖动）按计划进入 `v0.0.2`。

---

## v0.0.2 — 交互与帧播放

### 测试环境
- **操作系统**：Microsoft Windows 11 家庭中文版（Build 22631）
- **Qt Kit**：Desktop Qt 6.12.0 MSVC2022 64bit
- **构建方式**：CMake（Qt Creator / Clean Ninja+MSVC 验证构建）
- **内置素材**：
  - `:/pet/idle.gif`（`resources/pets/idle.gif`，`240 × 240`，10 帧）
  - `:/pet/walk.gif`（`resources/pets/walk.gif`，`480 × 480`，12 帧）
  - `:/pet/sleep.gif`（`resources/pets/sleep.gif`，`190 × 190`，34 帧；原素材为伪 `.gif` 后缀 WebP，已转为标准透明 `GIF89a`）

### 已实现范围
- 抽出独立动画模块 `src/animation/FramePlayer`（继承 `QObject`，不依赖窗口）：
  - 使用 `QImageReader` 逐帧解码预缓存为 `QPixmap` 与帧间隔列表，并对异常过小帧间隔做下限兜底
  - 单次链式 `QTimer` 调度（`setSingleShot(true)`），切换片段先停旧定时器再重置第 0 帧，单帧片段停表省电
  - 状态先于 `frameChanged()` 信号更新，避免同线程同步槽读到旧状态或越界索引
- `PetWindow` 绘制升级：
  - 每帧先以 `QPainter::CompositionMode_Source` 清成全透明，再切回 `CompositionMode_SourceOver` 绘制当前帧（防残影）
  - 引入统一逻辑画布上限 `120 × 120`（`Qt::KeepAspectRatio` 等比缩放）与底部居中脚底锚点 `(90, 156)`
- 鼠标手势与单击问候：
  - 左键按下记录全局坐标与窗口偏移，移动超 `QApplication::startDragDistance()` 后进入拖动（且不因拖回原点附近触发死区卡顿），松手单次结算
  - 单击在窗口顶部留白区叠加半透明圆角问候气泡，单次 `QTimer` 2 秒自动收起
- 右键菜单支持手动切换“待机 / 行走 / 睡觉”，并保留“置顶”与“退出”

### 实机验收清单
- [x] 单击角色稳定弹出问候气泡，2 秒后自动消失，不触发窗口位移
- [x] 拖动角色流畅跟手，不误触发单击问候；拖出轮廓外松手后仍可继续响应后续交互
- [x] 连续切换“待机 / 行走 / 睡觉”三种动画片段，角色显示完整、脚底位置稳定、帧率不翻倍
- [x] 置顶切换（保持可见与原位置）与退出结束进程均正常回归
- [x] 从干净构建目录配置并编译 MSVC x64 目标通过

### 结论与备忘
- `v0.0.2` 交互与帧播放里程碑验收通过；四态行为控制器（`PetController`）、多屏边界约束（`ScreenBounds`）、当前屏自动漫步、系统托盘与用户偏好持久化（`QSettings`）按计划进入 `v0.1`。
