烧录工具在执行烧录时，会产生一些事件，并以特定格式实时记录到主程序目录下的event.txt文件中，上层应用可通过实时解析该文件来获取烧录工具当前的状态。
每个event由多个字段组成（至少两个字段），且独占一行，字段之间由空格分开。
第一个字段是日期/时间，第二个字段是事件类型，具体如下表所示：

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
| 0x300f     | EV_LOW_POWER       | 电池电量过低 |
| 0x3010     | EV_USERDATA_ERROR_SKIP   | 在userdata更新失败时，用户选择了不进行强制更新 |
| 0x3011     | EV_USERDATA_ERROR_MANDATORY | 在userdata更新失败时，用户选择了强制更新 |