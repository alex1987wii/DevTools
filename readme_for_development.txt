烧录工具操作流程
1.修改配置文件
2.选择烧录方式进行烧录

说明：
1. linux烧录必须在target上有系统运行时才可使用。
2. mfg烧录在设定完成后点击download按钮，会出现waiting for MFG device.. 的提示，然后按照特定操作步骤使得设备进入指定模式，并使用USB连接到PC，此时程序检测到mfg设备的插入事件并自动进行烧录。
3. linux烧录使用的配置文件为for_user_file.ini，spl和mfg烧录使用的配置文件是for_mfg_file.ini，其格式为标准ini格式，若想详细了解ini语法格式，可直接在网络上查找相关信息，这里还对ini的字段进行了一些限制：
文件必须要有Options这个section,而且该section下必须要有ip字段,image字段和rescue_image为可选字段，对于for_mfg_file.ini而言，rescue_image为必选字段，image字段以浏览文件的地址为准。
