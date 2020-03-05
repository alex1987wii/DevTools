## 本次修改点
1. 软件名修改为Unication_Device_Upgrade.exe.
2. 去掉了*for_user_file.ini*文件中的ip字段(默认ip为10.10.0.12).
3. 添加event.txt文件，并实时向其中记录特定的event

| Event Code | MACRO              | Description |
| :----      | :----              | :---- |
| 0x3001     | EV_START           | 初始状态，程序启动尚未点击upgrade按钮|
| 0x3002     | EV_UPGRADE         | 用户点击upgrade按钮 |
| 0x3003     | EV_NETWORK_ERROR   | 网络错误事件 |
| 0x3004     | EV_VERIFY_MSN      | 验证MSN的事件，有附加信息msn序号 |
| 0x3005     | EV_MSN_DB_LOST     | 检测到msn.db文件打开失败后抛出的事件|
| 0x3006     | EV_MSN_NOT_MATCH   | 对比设备与msn.db中的MSN序号，不匹配后抛出|
| 0x3007     | EV_MSN_MATCH       | 对比设备与msn.db中的MSN序号，匹配后抛出|
| 0x3008     | EV_INIT_SUCCESS    | 初始化（烧录前的准备）成功后抛出 |
| 0x3009     | EV_INIT_ERROR      | 初始化（烧录前的准备）失败后抛出|
| 0x300a     | EV_BURNPARTITION   | 强制烧录分区前抛出 |
| 0x300b     | EV_BURNIMAGE       | 更新系统前抛出 |
| 0x300c     | EV_UPGRADE_FAILED  | 更新userdata分区失败后抛出(后续在点击对话框后强制烧录) |
| 0x300d     | EV_ERROR           | 烧录或更新出错|
| 0x300e     | EV_COMPLETE        | 烧录或更新（成功）完成|  
4. 文件清理工作，遵循谁创建谁管理谁清除的原则，以免出现混乱。目前烧录工具会在tmp目录下生成临时解密镜像文件以及从target端下载的msn序号文件，会在适当的时机删除。
