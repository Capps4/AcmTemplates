# 严格编译与原注释功能切换

统一入口见 [主 README](../README.md)。

```bash
python3 template/Run.py compile --all
python3 template/Run.py check --all
python3 template/Run.py integration
```

compile 只表示严格编译成功；check 会执行严格版、优化版和 sanitizer 的正确性测试。
集成测试包含九个保留的 Original 注释功能切换，生成源码只存在 Build 中，默认模板不被修改。
Options.py 保留 DSU 按大小合并、AC 模式 ID、重心树、ExSam ID、Sam cnt、SCC 去重、RingTree DSU、LinearBasis 排名切换和 FFT 取整验证。
Point、PersistentTree、Geo2/Geo3 暂缓。
原 Results.json、Options.json 和 OriginalComments.json 是上一次扫描的历史证据；当前结果使用 Reports。
