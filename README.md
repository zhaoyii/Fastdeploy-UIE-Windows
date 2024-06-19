### 1. 准备源码
项目内 C++ 源码`infer.cc`和编译配置文件` CMakeLists.txt`拷贝自百度飞桨 [UIE 示例库](https://github.com/PaddlePaddle/FastDeploy/tree/develop/examples/text/uie/cpp)，修复了如下问题：
1. 增加源代码文件预编译指令`utf-8`，解决运行模型，因为中文字符串编码问题导致死循环。
2. 解决输出中文乱码问题。


### 2.编译

编译环境依赖：
- Visual Studio 16 2019 （[下载链接](https://learn.microsoft.com/zh-cn/visualstudio/releases/2019/redistribution#--download), [安装建议](https://learn.microsoft.com/zh-cn/windows/dev-environment/rust/setup)）
- cmake >= 3.12 （Visual Studio 自带，不用安装）

**注意：使用`x64 Native Tools Command Prompt for VS 2019`命令行工具编译项目。**

1.创建编译目录

```PowerShell
# 使用 PowerShell
mkdir build
cd build
```

2.下载`FastDeploy`预编译库(PowerShell)

```PowerShell
# 使用 PowerShell
Invoke-WebRequest -Uri "https://bj.bcebos.com/fastdeploy/release/cpp/fastdeploy-win-x64-1.0.7.zip" -OutFile "fastdeploy-win-x64-1.0.7.tgz"

tar -xvf fastdeploy-win-x64-1.0.7.tgz
```

3.配置一个CMake项目，并指定`FastDeploy`库的安装目录为当前工作目录下的`fastdeploy-win-x64-1.0.7`目录。为了在构建项目时能够找到并使用`FastDeploy`库：

```PowerShell
# 使用 x64 Native Tools Command Prompt for VS 2019 命令行窗口编译配置
cmake .. -G "Visual Studio 16 2019" -A x64 -DFASTDEPLOY_INSTALL_DIR=%CD%\fastdeploy-win-x64-1.0.7
```

4.使用`msbuild`编译项目

```PowerShell
# 使用 x64 Native Tools Command Prompt for VS 2019 命令行窗口编译
MSBuild infer_demo.sln /m /p:Configuration=Release /p:Platform=x64
```

5.将`FastDeploy`动态依赖库拷贝至可执行程序所在目录。（或者配置环境变量。）建议将动态依赖库拷贝到`infer_demo.exe`所在目录，[原理👋](./doc/README_DLL.md)。

```PowerShell
cd fastdeploy-win-x64-1.0.7
.\fastdeploy_init.bat install "%cd%" "..\Release"
```

### 3.下载模型进行推理(PowerShell)

下载模型：

```PowerShell
# 使用 PowerShell
cd ..\Release
Invoke-WebRequest -Uri https://bj.bcebos.com/fastdeploy/models/uie/uie-base.tgz -OutFile "uie-base.tgz"
tar -xvf uie-base.tgz
```

使用 CPU 运行推理：

```PowerShell
.\infer_demo.exe uie-base 0 0 0.03 512 地点,地名,地方,人名,姓名,名字,时间,感情,情绪,心情 1 1 2024年5月6日，周五下午，李华在北京的天安门广场上，与他的好朋友张明一起享受阳光。他们都感到非常开心。突然，一个名叫'纽约'的小狗跑了过来，李华笑着说，'这只狗的名字真有趣！'张明回答说，'是的，我也觉得很有趣。'然后，他们决定一起去吃一家叫做'巴黎'的法国餐厅。
```
如果文本中含有空格，可使用单引号包裹文本：
```PowerShell
.\infer_demo.exe uie-base 0 0 0.03 512 地点,地名,地方,人名,姓名,名字,时间,感情,情绪,心情 1 1 '2024年5月6日，周五下午，李华在北京的天安门广场上，与他的好朋友张明一起享受阳光。他们都感到非常开心。突然，一个名叫'纽约'的小狗跑了过来，李华笑着说，'这只狗的名字真有趣！'张明回答说，'是的，我也觉得很有趣。'然后，他们决定一起去吃一家叫做'巴黎'的      法国餐厅。'
```

**注意：使用百度飞桨 paddle 运行时。windows 使用 onnx 运行时会死循环。**

## 参考
- https://github.com/PaddlePaddle/FastDeploy/blob/develop/docs/cn/faq/use_sdk_on_windows.md
- https://github.com/PaddlePaddle/FastDeploy/blob/develop/docs/cn/faq/use_sdk_on_windows_build.md#VisualStudio2019Sln3
- https://github.com/PaddlePaddle/FastDeploy/blob/develop/docs/cn/build_and_install/download_prebuilt_libraries.md
- https://github.com/PaddlePaddle/FastDeploy/issues/2290
- https://github.com/PaddlePaddle/FastDeploy/blob/17ff330daa9b87a5e443899579fdccfd92782ffe/docs/cn/faq/build_on_win_with_gui.md#%E7%89%B9%E5%88%AB%E6%8F%90%E7%A4%BA
- https://github.com/PaddlePaddle/FastDeploy/blob/develop/examples/text/uie/cpp/README_CN.md