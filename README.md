### 1. 准备源码

从百度飞桨 UIE [示例库](https://github.com/PaddlePaddle/FastDeploy/tree/develop/examples/text/uie/cpp)下载源码和 CMakeLists.txt 文件。


### 2.编译

编译环境依赖：
- cmake >= 3.12
- Visual Studio 16 2019

**注意：使用`x64 Native Tools Command Prompt for VS 2019`命令行工具编译项目。**

1.创建编译目录

```PowerShell
mkdir build
cd build
```
2.下载`FastDeploy`预编译库(PowerShell)
```PowerShell
Invoke-WebRequest -Uri "https://bj.bcebos.com/fastdeploy/release/cpp/fastdeploy-win-x64-1.0.7.zip" -OutFile "fastdeploy-win-x64-1.0.7.tgz"

tar -xvf fastdeploy-win-x64-1.0.7.tgz
```
3.配置一个CMake项目，并指定`FastDeploy`库的安装目录为当前工作目录下的`fastdeploy-win-x64-1.0.7`目录。为了在构建项目时能够找到并使用`FastDeploy`库：
```PowerShell
cmake .. -DFASTDEPLOY_INSTALL_DIR=%CD%\fastdeploy-win-x64-1.0.7
```
4.使用`msbuild`编译项目
```PowerShell
msbuild infer_demo.sln
```
5.将`FastDeploy`依赖库拷贝至可执行程序所在目录, 或者配置环境变量。
```PowerShell
cd fastdeploy-win-x64-1.0.7
fastdeploy_init.bat install "%cd%" "..\Debug"
```

### 3.下载模型进行推理(PowerShell)
下载模型：
```PowerShell
cd ..\Debug
Invoke-WebRequest -Uri https://bj.bcebos.com/fastdeploy/models/uie/uie-base.tgz -OutFile "uie-base.tgz"
tar -xvfz uie-base.tgz
```
使用 CPU 运行推理：
```PowerShell
.\infer_demo.exe uie-base 0
```
