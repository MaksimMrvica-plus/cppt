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
