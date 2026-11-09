#  Git基础 

## 1. 安装与配置 Git
查看是否已安装：终端输入 git
安装指令：sudo apt install git
配置账户信息：
```bash
git config --global user.name "Your Name"
git config --global user.email "email@example.com"
```

## 2. 创建版本库并添加文件
```bash
mkdir xxx        # 创建空目录
cd xxx           # 进入目录
pwd              # 显示当前路径
git init         # 将目录变为 Git 仓库
git add xxx      # 添加文件到暂存区
git commit -m "提交说明"   # 提交到本地仓库
```

## 3. 管理文件
```bash
git status               # 查看仓库状态（显示被修改的文件）
git diff xxx             # 查看文件具体改动内容
git checkout -- xxx      # 撤销工作区修改
git reset HEAD xxx       # 撤销暂存区修改
git rm xxx               # 删除文件
git commit               # 提交删除操作
```

## 4. 远程仓库
基本步骤：关联仓库 -> 暂存 -> 提交 -> 推送。
重要提醒：在项目根目录创建 .gitignore 文件，写入忽略规则（如：build/、install/、log/、*.mp4、*.png、*.o），防止上传编译产物。
```bash
# 关联远程仓库
git remote add origin xxx远程仓库地址

# 首次推送
git push -u origin xxx分支名

# 删除远程库
git remote -v            # 查看远程库信息
git remote rm xxx        # 删除远程库（xxx为库名）
```

## 5. 分支管理
```bash
git checkout -b xxx      # 创建并切换到新分支
git checkout xxx         # 仅切换分支
git branch xxx           # 仅创建分支
git branch               # 查看所有分支
git merge xxx            # 将 xxx 合并到当前分支
git branch -d xxx        # 删除 xxx 分支
```

## 6. 标签管理
```bash
git tag v1.0             # 打标签
git tag                  # 查看所有标签
git tag -d v0.1          # 删除标签
git push origin --tags   # 推送所有标签到远程
```
