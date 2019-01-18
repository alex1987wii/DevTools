生产烧录工具操作流程
1.修改配置文件(for_mfg_file.ini),指定rescue_image所在路径全名（若与工具同一目录，可直接指定为文件名)。注：ip字段请保持为10.10.0.12。
2.点击Browser按钮，选取要烧录的镜像（必须要和rescue_image匹配）。
3.选择是否勾选batch programing开启批量烧录模式，并点击download进行烧录。
4.屏幕提示“Waiting for mfg device"信息，将设备按特定操作步骤进入mfg模式并连接到PC，程序检测到MFG设备，自动开始烧录。
5.若开启了批量烧录模式，在屏幕提示”Waiting for mfg device“信息时，会出现一个Stop按钮，点击该按钮可取消当前的等待状态。若已经开始烧录中点击Stop按钮，则取消批量烧录模式，本次烧录完成后停止。

