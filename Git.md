# Git 管理

仓库根目录是 `template/`，默认分支 `main`，远端 `origin` 为 `git@github.com:Capps4/AcmTemplates.git`。外层 acm_compete 的刷题文件、编辑器配置和截图不纳入该仓库。

本机为该仓库配置了独立的 SSH 密钥 `~/.ssh/id_ed25519_acm_templates`，通过仓库本地 `core.sshCommand` 使用，保留其他仓库的 SSH 配置。私钥保存在 `~/.ssh`，不纳入仓库。公钥需添加到 GitHub 的 SSH authentication keys 后才能推送。

跟踪实现、当前 Snippets 快照、Original 对照、测试、示例、工具、文档，以及 Baselines / VerificationHistory 中的历史资料。Reports、ReadyHeaders、Build/build、Python 缓存和二进制产物由 .gitignore 排除，继续保留在本机。

在 template 目录执行日常操作：

```bash
# 查看工作区与忽略情况
git status --short
git status --short --ignored

# 修改后暂存并检查
git diff
git add .
git diff --cached --stat
git diff --cached

# 提交并推送到当前上游分支
git commit -m "Update ListHelper"
git push

# 拉取远端更新；本仓库设置 pull.ff=only，仅允许快进更新
git pull

# 查看本地历史
git log --oneline --decorate
```

已有模块修改时同时维护实现、契约文档和相关测试，Original 和 Baselines 保留作为历史对照。Snippets 是导入时的来源快照；Final 包含后续修复，两者可能有明确记录的差异。工具的验证结果以源码指纹为准，Git commit 本身不代表验证通过。

首次推送使用 `git push -u origin main` 设置上游；之后在 main 上直接执行 `git push` / `git pull`。

后续需要隔离改动时可使用 `git switch -c <branch>` 创建本地分支，再用 `git push -u origin <branch>` 设置该分支的上游。日常推送使用普通 `git push`，无需强推。
