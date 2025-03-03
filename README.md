# cppt
webchat


测试用的变量统一以 "TEST_USED_" 开头
## 目录
优先级从高到低:
❌有错误（有错误，最近，最重要，最紧急，需要修复）
⭕
🎯当前目标（没有错误时，最近，最重要，最紧急）
🚧障碍进度（这个任务不做，会阻碍其他功能完成） 
⚠️近期优先做（完成当前目标后，选择其中）
📝计划中（只要想到的功能，列出来的点，就算）
🔜

✖️存疑(大体完成，或许会有问题的点，可能需要修复)
✈️后续可选优化点（把基本功能都实现后，后期考虑可以做优化的点）
✅已完成(已经完成)

1. Register
    ✅1.1 Register 失败   
    ✅1.2 Register 成功   
2. Login
    ✅2.1 Login 失败   
        📝2.1.1 账号不存在   
        📝2.1.2 密码错误
        📝2.1.3 账号被封禁
    ✅2.2 Login 成功后，自动请求一次用户资料
        ✅2.2.1 Login 登录成功后，再登录会检查当前状态
        ✅2.2.2 Register 登录成功后，不让进行注册
        ✅2.2.3 Logout 登录成功后，才可以登出
        🔜2.2.4 Login 登录成功后，可以更改自身在线状态
3. Get User Profile
    ✅3.1 失败，库中没有这个用户的资料，提示创建资料
    ✅3.2 成功，返回用户资料，加载到本地
7. Create User Profile
    🎯7.1 主动创建资料  创建资料和更新资料的主要区别是，创建全部，更新可以只选择部分
9. Update User Profile
5. Group
6. Friend
3. Chat
41. Send Message
42. Send Message Per
43. Send Message Grp
10. Logout
    ✅10.1 Logout 检查在登录状态，才可以登出
    ✅10.2 Logout 发送登出请求后，不需要再接收服务侧返回消息
        ⚠️10.2.1 服务端将用户从在线表中移除
    ✖️10.3 直接进行本地一些数据清理

12. Display User Account Info
11. Display Profile
13. Display Friend List
14. Change Your Status
    ✅14.1 改变的只是展示状态，并非真正的状态
    14.2 发送到服务器消息，服务器会将假状态更新，并广播给所有在线用户