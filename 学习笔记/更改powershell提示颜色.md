默认路径
    C:\Users\SGW\Documents\WindowsPowerShell\Microsoft.PowerShell_profile.ps1
配置powershell：
1. 检查配置文件是否存在
Test-Path $PROFILE
2. 创建配置文件
New-Item -Path $PROFILE -Type File -Force
3. 打开配置文件
notepad $PROFILE
4. 添加以下内容
function Prompt {
    # 设置提示符颜色
    $foregroundColor = [ConsoleColor]::Green  # 你可以选择其他颜色，如 Red, Blue, Yellow 等
    $backgroundColor = [ConsoleColor]::Black  # 背景颜色

    # 设置提示符文本
    $promptText = "PS " + (Get-Location).Path + "> "

    # 设置颜色并输出提示符
    Write-Host $promptText -ForegroundColor $foregroundColor -BackgroundColor $backgroundColor -NoNewline

    # 返回一个换行符
    return "`n"
}
5. 重新加载 PowerShell： 关闭并重新打开 PowerShell，或者运行以下命令来重新加载配置文件：

. $PROFILE

6. 查看配置文件路径：
$PROFILE
<<<<<<< HEAD
=======


powershell 运行脚本权限：
1. 查看策略
Get-ExecutionPolicy
2. 更改执行策略 
这将允许本地脚本运行，但要求从互联网下载的脚本必须经过签名：
Set-ExecutionPolicy RemoteSigned

如果你希望允许所有脚本运行，可以使用 Unrestricted：
Set-ExecutionPolicy Unrestricted
3. 没有管理员权限时可以用：
Set-ExecutionPolicy -Scope CurrentUser RemoteSigned
Set-ExecutionPolicy -Scope CurrentUser Unrestricted
>>>>>>> 0b3d94a12887fea97b6ea828b473dfc09b3d90df
