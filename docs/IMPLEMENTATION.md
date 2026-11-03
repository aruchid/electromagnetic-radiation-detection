# 当前原型范围

## 输入

一维等间隔采样检波信号

## 实现语言

C11 库接口：`c/detector_filter.h`，实现：`c/detector_filter.c`；`c/demo.c` 导出合成 CSV。Python 作为对照参考和绘图工具保留。C 运行不依赖 NumPy 或 Python。

## 算法

中值去脉冲，再移动平均平滑；不实现射频硬件检波。

## 验收

运行 `make all test-c` 检查纯 C 行为；运行 Python unittest 验证 C/Python 同输入一致性，含随机信号、不同窗口与极端数值。Python 指标和图保存在 artifacts；C 演示输出 CSV 与 stderr 指标。

## 限制

窗口长度决定平滑与响应延迟，真实噪声需重新选择参数。

## 文档优先级

本文件与 README 描述已实现范围。PRD 是目标需求，未完成部分不能据此视为已实现。
