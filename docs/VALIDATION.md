# 验证记录

`python -m unittest discover -s tests -v`

```text
test_invalid_data (test_prototype.PrototypeTests.test_invalid_data) ... ok
test_reference_recovery (test_prototype.PrototypeTests.test_reference_recovery) ... ok
test_reject_invalid_process_input (test_prototype.PrototypeTests.test_reject_invalid_process_input) ... ok

----------------------------------------------------------------------
Ran 3 tests in 0.020s

OK
```

`python src/prototype.py --output artifacts` 生成实际结果图及 metrics.json。指标来自固定种子合成样例，不代表真实场景精度。


## 数值稳定性回归（2026-11-02）

修复前，`process(np.full(15, np.finfo(float).max), window=11)` 输出包含无穷值，虽然输入有限且常量信号的理论平均仍等于输入值。修复将中值滤波后的信号归一化后做移动平均，并约束归一化结果的舍入越界，再恢复原尺度。

新增测试覆盖窗口 1、3、11、17、101，正负 float64 极值、零和 1e-308 常量；同时比较随机信号在 1e-300 与 1e300 缩放后的结果。非法窗口类型统一抛出 ValueError，继续支持 NumPy 整数。

本机 Python 3.14.9 运行 6 项测试全部通过；合成样例 raw_rmse=0.20540606716562915，filtered_rmse=0.029931755037582348。未新增硬件实测或真实检测数据结论。GitHub Actions 配置对 Python 3.11、3.13 执行测试和合成样例导出，实际 CI 状态以 Actions 为准。
