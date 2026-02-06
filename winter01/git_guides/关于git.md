代码版本管理工具，可以实现代码版本更新，历史代码回滚，代码分支创建

## 常用命令
- git config：初始设置
```bash
git config --global user.name "Phragmund" # 全局配置用户名
git config --global user.email "phragmund@163.com" # 全局配置邮箱
git config --list # 查看所有配置，验证是否生效 
git config user.name "Phragmund_Repo_1"# 单个仓库单独配置（进入仓库目录执行，覆盖全局配置） 
```

- git init：初始化仓库
```bash
cd 项目文件夹 # 先进入项目目录 
git init # 初始化仓库
```

- git status：查看工作区状态，用于确认文件修改/新增/未跟踪
```bash
git status # 详细状态（中文显示）
git status -s # 简洁状态（快速看文件变化）
```

- git add：将工作区的修改 / 新增文件，添加到暂存区
```bash
git add 文件名.py # 添加单个文件
git add 文件夹/ # 添加整个文件夹
git add . # 添加当前目录所有修改/新增文件（推荐日常用，高效）
git add -u # 仅添加已跟踪文件的修改（不包含新创建的未跟踪文件）
```

- git commit：将暂存区的文件提交为本地版本
```bash
git commit -m "备注信息：如【功能】新增登录接口、【修复】解决列表渲染卡顿" # 直接提交+备注
git commit # 无-m会打开默认编辑器写备注
git commit --amend # 追加提交（修改最后一次提交的备注，或补充暂存区文件到最后一次提交，未推远程时用）
```

- git log：查看本地提交的版本历史记录
```bash
git log # 详细日志（按q退出查看）
git log --oneline # 简洁日志（每行一个版本，显示短版本号+备注） 
git log --graph # 图形化日志（带分支线，多分支时看版本走向） 
git log -n 5 # 查看最近5次提交
```

- git diff：查看文件未暂存的修改细节，对比工作区和暂存区的差异，确认修改内容
```bash
git diff # 查看所有未暂存文件的差异 
git diff 文件名.py # 查看单个文件的差异 
git diff --cached # 查看已暂存但未提交的文件差异
```
## 分支管理
- git branch：查看/创建/删除分支
```bash
git branch # 查看本地所有分支（*标注当前所在分支） 
git branch -a # 查看本地+远程所有分支 
git branch 分支名 # 创建新分支（基于当前分支创建，不会自动切换） 
git branch -d 分支名 # 删除本地已合并的分支（未合并会提示） 
git branch -D 分支名 # 强制删除本地分支（未合并也能删） 
git branch -m 旧名 新名 # 重命名当前分支
```

- git checkout/git switch：切换分支
```bash
# 切换已有分支（两种方式都可以） 
git checkout 分支名 
git switch 分支名 
# 创建并直接切换到新分支（最常用） 
git checkout -b 新分支名 
git switch -c 新分支名 
# 切换到上一个分支（快速在两个分支间切换） 
git checkout - 
git switch -
```

- git merge：合并分支，将指定分支的代码合并到当前所在分支
```bash
# 示例：将feature/登录功能合并到main主分支 
git switch main # 先切换到主分支 
git pull # 先拉取远程主分支最新代码（避免冲突） 
git merge feature/登录功能 # 合并开发分支到主分支
```

- git stash：暂存工作区未提交的修改（临时切换分支时保存当前未完成代码）
```bash
git stash # 暂存当前工作区所有未提交修改（会清空工作区的修改状态） git stash save "备注：未完成的登录接口开发" # 带备注的暂存（多暂存时方便区分） git stash list # 查看所有暂存的记录 git stash apply # 恢复最近一次的暂存（暂存记录不会删除） git stash pop # 恢复最近一次的暂存并删除该暂存记录（推荐，恢复后清理） git stash drop # 删除最近一次的暂存记录 git stash clear # 清空所有暂存记录
```
## 远程仓库协作
- git remote：关联/查看/删除远程仓库
```bash
# 关联远程仓库（origin是别名，可自定义） 
git remote add origin 远程仓库地址（HTTPS/SSH） 
git remote -v # 查看远程仓库关联信息（验证是否正确） 
git remote remove origin # 删除已关联的远程仓库别名 
git remote rename 旧别名 新别名 # 重命名远程仓库别名
```

- git clone：从远程仓库克隆整个项目到本地
```bash
git clone 远程仓库地址 # 克隆到本地，文件夹名和远程项目名一致 
git clone 远程仓库地址 自定义文件夹名 # 克隆到本地并指定文件夹名
```

- git pull：拉取远程仓库的最新代码，合并到本地当前分支
```bash
git pull origin 分支名 # 拉取远程origin仓库的指定分支到本地当前分支 
git pull # 简写（当前分支已关联远程分支时，直接拉取）
```

- git push：将本地分支的最新提交推送到远程仓库的指定分支
```bash
# 首次推送当前分支到远程（-u：关联本地分支和远程分支，后续可直接git push） 
git push -u origin 分支名 
git push # 简写（已关联远程分支时，直接推送） 
git push origin -d 分支名 # 删除远程仓库的指定分支
```

## 版本回滚与撤销
- 未暂存，仅工作区修改，还没执行git add
```bash
# 撤销单个文件的工作区修改（恢复到最近一次提交/暂存的状态） 
git checkout -- 文件名.py # 撤销所有工作区的未暂存修改
git restore . # Git2.23+可替代git checkout -- .
```

- 已暂存，执行了git add，还没执行git commit
```bash
git reset HEAD 文件名.py # 取消单个文件的暂存（回到工作区） 
git reset HEAD . # 取消所有文件的暂存（推荐） 
# 之后再执行撤销工作区修改的指令 
git restore .
```

- 已提交本地，执行了git commit，还没推送远程
```bash
# 第一步：查看历史版本，获取要回滚的「短版本号」（git log --oneline） # 示例：版本号是a1b2c3，回滚到该版本 
git reset --mixed a1b2c3 # 【默认/推荐】回滚版本，修改保留在工作区（可重新提交） 
git reset --soft a1b2c3 # 回滚版本，修改保留在暂存区（直接git commit即可重新提交） 
git reset --hard a1b2c3 # 回滚版本，彻底丢弃所有修改
```

- 已推送到远程，执行了git push
```bash
# 第一步：获取要回滚的目标版本号（git log --oneline） 
git revert 版本号 # 创建新提交，撤销该版本的所有修改 
git push # 将回滚的新提交推送到远程，团队拉取后即可同步
```

## 实际场景应用
- 单人开发+推远程
```bash
1. cd 项目目录 && git init（首次） 
2. git config --global 配置用户名/邮箱（首次） 
3. 写代码... 
4. git status -s 查看文件变化 
5. git add . 暂存所有修改 
6. git commit -m "清晰的备注" 提交本地版本 
7. git remote add origin 远程地址（首次推） 
8. git push -u origin main（首次推，后续直接git push） # 后续开发重复3-6，然后git push即可
```

- 多人协作
```bash
1. git clone 远程地址（首次获取项目） 
2. git switch -c feature/新功能 （创建并切换开发分支） 
3. git pull origin main（拉取主分支最新代码，避免落后） 
4. 写代码... 
5. git add . && git commit -m "备注"（本地提交） 
6. git pull origin main（再次拉取，解决可能的冲突） 
7. git push -u origin feature/新功能（推开发分支到远程） # 开发完成后，合并到主分支（仓库平台提PR/MR，或本地合并后推） 
8. git switch main && git pull origin main 
9. git merge feature/新功能 && git push origin main
```

## 使用SourceTree完成git工作流
[参考资料](https://zhengqing.blog.csdn.net/article/details/124456228)
