## OpenNI 2 SDK 环境配置
> 仅供参考，目前将开发环境迁移到Astra SDK（因其提供骨骼特征API）

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

## Astra SDK 环境配置
硬件环境：Zora P1开发板+Astra相机

软件环境：开发板预装的基于Arm64的Ubuntu；OpenNI2 SDK

因为根据官方描述，Astra提供了骨骼信息提取的SDK，对于实验有用，故来试试这个开发环境的配置。

1. 在官网[下载使用于Ubuntu 14.04 or later的 Linux-aarch64 SDK (orbbec.com.cn)](https://api.orbbec.com.cn/uploads/files/AstraSDK-v2.1.1-24f74b8b15-20200426T012326Z-Linux-aarch64.tar.gz)
2. 解压后运行`install/install.sh`，不同于上面，这个脚本只是把环境变量打印出来，建议放在系统变量的配置文件中`.bash_profile` or `.bashrc`。 因为配置过了OpenNI SDK的开发环境，还是把这两个系统变量保存到`environment`文件中。编译前执行`source environment`即可。
3. 在`sample`目录下运行`cmake .`
4. 根据生成的Makefile，运行对应的target，即可进行编译。如`make InfraredColorReaderEvent ` 
5. 生成的程序在`samples/bin`目录下

> 此工程目前依赖Astra SDK环境，将其放在samples目录下

## Visual Studio 2019 + AstraSDK 开发环境配置
见官方指导文档https://developer.orbbec.com.cn/technical_library.html?id=22

注意，生成的exe文件依赖AstraSDK中bin目录下的dll文件

所以还要把，那些dll拷贝到我们自己工程的生成路径中如`x64/Debug`下



## 此项目环境

OpenCV 4.5

jsonxx

glew-2.1.0

glfw-3.3.2