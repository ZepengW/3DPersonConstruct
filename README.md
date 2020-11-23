## 环境配置

硬件环境：Zora P1开发板+Astra相机

软件环境：开发板预装的基于Arm64的Ubuntu；OpenNI2 SDK

### 设备功能验证

​    linux环境下，Astra直接连接开发板usb即可

​	**在官网下载OpenNI2 SDK**  `OpenNI-Linux-Arm64-2.3.0.65` [Android 系统固件 · 3D视觉开发者社区 · 奥比中光 (orbbec.com.cn)](https://developer.orbbec.com.cn/download.html?id=64)。 链接路径若有变动请自行在官网查找。

​	解压到开发板目录，**运行./install.sh**(需要用chmod修改文件权限，作为一个不合格的linux开发，通常是简单粗暴的 `chmod +777 install.sh`

> 注：该脚本一共做了两件事，为摄像头设备增加UDEV规则，把开发环境写到 `OpenNIDevEnvironment`文件中

​	安装README文件中提到的依赖项	

```
- GCC 4.x
- Python 2.6+/3.x
- LibUSB 1.0.x
- LibUDEV
- JDK 6.0
- FreeGLUT3
- Doxygen
- GraphViz
```

​	运行`OpenNI-Linux-Arm64-2.3.0.65/Tools/Arm64-Release`目录下的NiViewer可验证

### 开发环境配置

​	加载之前生成的系统变量 `source OpenNIDevEnvironment`

​	`Samples`目录下面有一些Sample Programs. 比如打开`SimpleRead`

​	修改Makefile文件，因为makefile里面`INC_DIRS`定义的时候用的是`../../Include`但是实际文件路径中是小写的`include`. Linux 系统对大小写敏感，所以改成小写的include，或者直接把文件夹名改成大写的。因为我看其他程序都是用的小写。而且引用的`../Common`目录，在示例工程中没有该目录。但是没有影响后面的编译，所以暂时没管

​	执行`make`，成功编译。虽然在根目录的`README`中说ARM平台编译要定义一些系统变量，但是这边没报错，暂时没管。

​	执行生成文件`SimpleRead/Bin/Arm64-Release/SimpleRead`

​	没看源代码，所以不知道这个程序是什么意思，但是摄像头前面没人的时候输出是0，有人输出一堆数字，暂且认为是正确的吧。

​	下面就可以按照Sample中的工程架构，构建自己的项目了。

