# MultiTouchPdfAnnotator

Qt/C++ 多点触摸 PDF 注释器 — 最小可运行原型

## 功能
- 打开并渲染 PDF（显示第一页）
- 透明 Overlay 支持在 PDF 上绘制
- 自由涂鸦、直线、矩形、椭圆、三角形工具
- 掌心检测作为橡皮擦（基于触点面积）
- 两指长按检测（进入图片编辑准备模式占位）
- 支持触摸事件（QTouchEvent）和鼠标回退
- GitHub Actions 自动化 Windows 编译

## 本地编译 (Windows 10/11 + MSVC)

### 前置要求
1. **Visual Studio 2022** (MSVC 工具链)
2. **Qt 6.x** (需要 Qt Pdf 模块)
3. **CMake 3.16+**

### 编译步骤
```bash
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release