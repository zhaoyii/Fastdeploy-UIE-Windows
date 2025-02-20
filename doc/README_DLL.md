### Windows操作系统中可执行文件（.exe）如何寻找动态链接库（.dll）?

Windows会按照以下顺序搜索DLL文件：

1. 当前目录：Windows首先检查运行`.exe`文件的当前目录。（注意：指`.exe`所在目录）
2. 系统目录：通常是`C:\Windows\System32`。
3. 16位系统目录：对于一些旧的16位应用程序，会检查`C:\Windows\System`。
4. Windows目录：通常是`C:\Windows`。
5. PATH环境变量：如果在前面的目录中没有找到DLL，Windows会检查`PATH`环境变量中列出的目录。

当系统找到DLL后，会将它加载到内存中，并执行.exe文件中调用的函数。如果在这些步骤中找不到DLL，程序将无法启动，并可能显示“找不到指定模块”的错误消息。

**将DLL文件放在.exe文件所在的目录是一种常见的做法，因为它可以确保程序能够在运行时找到所需的DLL，而不依赖于系统的其他部分或环境变量的配置。** 这是一种简单且有效的解决方案，可以减少因DLL文件位置问题而导致的程序错误。


### 对比 unix/linux/macos 系统寻找动态库的流程?

Unix、Linux和macOS系统在寻找动态链接库（在Unix和Linux中通常以.so结尾，在macOS中以.dylib结尾）的流程与Windows系统有所不同。在这些系统中，动态链接器会在以下路径中搜索所需的共享库：

1. 程序本身的路径：与Windows类似，动态链接器首先检查加载该程序的路径。
2. 共享库配置文件：在某些系统上，动态链接器可能会检查一些配置文件，如/etc/ld.so.conf（在Linux上）或/etc/ld.so.conf.d/*（在Linux上），这些文件列出了共享库的路径。
3. 默认路径：动态链接器还会在默认的路径中搜索共享库，这些路径通常包括/lib和/usr/lib（在Linux上），或者在macOS上的/usr/lib和/System/Library/Frameworks。
4. 环境变量：在Unix和Linux系统上，LD_LIBRARY_PATH环境变量可以用来指定动态链接器搜索共享库的额外路径。在macOS上，DYLD_LIBRARY_PATH环境变量有类似的作用。
5. 运行时链接器缓存：在Linux上，ldconfig命令会生成一个缓存文件/etc/ld.so.cache，其中包含了系统上所有共享库的路径。动态链接器会检查这个缓存文件来加速搜索过程。
6. 递归依赖：如果动态链接器在加载一个共享库时发现该库依赖于其他库，它将重复上述过程来加载这些依赖库。

在Unix、Linux和macOS系统上，如果动态链接器无法找到所需的共享库，程序将无法启动，并且通常会收到一个错误消息，指出无法找到共享库或者共享库的某个版本。

**与Windows系统相比，Unix-like系统通常更多地依赖于配置文件和环境变量来管理共享库的搜索路径。** 此外，Unix-like系统中的共享库通常不需要放置在可执行文件所在的目录中，除非特别指定。

### 如果二进制程序包含动态库的地址 path 呢？


如果二进制程序在编译时包含了动态库的绝对路径，那么在运行时，操作系统会直接使用这个路径来加载动态库，而不会按照通常的搜索顺序去查找。这种情况下，动态库必须位于程序指定的路径中，否则程序将无法启动，因为它无法找到所需的库。

在Linux和类Unix系统中，当使用ld链接器时，可以通过`-rpat`h或`-Wl,-rpath`选项在编译时指定运行时库的路径。这个路径会被硬编码到二进制文件中，动态链接器在加载程序时会使用这个路径来寻找动态库。

```bash
gcc -o myprogram myprogram.c -L/path/to/library -lmylib -Wl,-rpath=/path/to/library
```

在这个例子中，`-Wl,-rpath=/path/to/library`告诉链接器将`/path/to/library`作为运行时库路径添加到二进制文件中。

**在Windows系统中，如果使用DLL，通常会在编译时指定DLL的路径，但这并不是硬编码到EXE文件中，而是通过其他方式实现，比如在安装过程中将DLL放在EXE相同的目录下，或者通过修改系统的环境变量来确保DLL可以被找到。**

需要注意的是，硬编码路径可能会导致问题，比如当程序被移动到其他目录或者系统中没有指定的路径时，程序可能无法运行。因此，**通常建议不要硬编码路径，而是依赖于操作系统的标准搜索机制或者在运行时动态设置库路径。**

## macos 寻找并链接到动态库的流程？
从一个寻找动态库失败的例子看流程：
```bash
Library not loaded: /usr/local/lib/libsqlcipher.0.dylib
Referenced from: <9FEED709-F275-3512-8CF1-57418C8AA73A> /Applications/42memo.app/Contents/MacOS/42memo
Reason: tried: '/usr/local/lib/libsqlcipher.0.dylib' (no such file), '/System/Volumes/Preboot/Cryptexes/OS/usr/local/lib/libsqlcipher.0.dylib' (no such file), '/usr/local/lib/libsqlcipher.0.dylib' (no such file), '/usr/lib/libsqlcipher.0.dylib' (no such file, not in dyld cache)
```

寻找流程：
1. 写入二进制程序或者 dylib 的固定路径。
2. `/usr/local/lib`目录。
3. `/System/Volumes/Preboot/Cryptexes/OS/usr/local/lib`目录。
4. `/usr/lib`目录。