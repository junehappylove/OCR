使用示例demo前，请您进行如下操作：

1、libs/armeabi下，存在运行相应能力所需要的函数库（如libcur.so、libjtz.so等等）
2、libs下存在Java SDK jar包
3、检查assets下AccountInfo.txt文件中用户信息是否正确填写

4、检查本地能力时检查是否存在相应的资源库文件（不是函数库），有两种添加库文件的方式
 	4.1、添加资源库文件到sdcard，此时在初始化能力时，需要设置PARAM_KEY_DATA_PATH：为sdcard下的绝对路径
 	
 	4.2、添加资源库文件到libs/armeabi目录下，此时需要将资源库文件重命名为以lib开头， .so结尾的文件， 同时PARAM_KEY_FILE_FLAG设置为android_so 。

	(目前模板识别暂不支持4.2操作，因此示例代码采用4.1方式进行设置，设置地址为SD卡目录下HciCloud/HciCloudOcrExample/data)

确认以上问题无误后，用户便可以正常使用该示例程序。