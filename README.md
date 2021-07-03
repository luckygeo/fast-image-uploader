# fast-image-uploader
a fast image uploader

快速上传图片至SM.MS图床，用C语言编写，完全开源 ，底层使用libcurl上传，支持macOS和Linux，后期考虑增加支持其它图床

## How

### Compile

将main.c内容约17行`AUTH`内容， ` Authorization: Basic apikey` 换成自己的SM图床key

```shell
cc main.c -o uploader  -lcurl
```

如果提示像`fatal error: curl/curl.h: No such file or directory`  ， 则需要安装`libcurl4-openssl-dev`，debian系如下命令直接安装

```shell
sudo apt-get install libcurl4-openssl-dev
```

macOS需要安装`Command Line Tools`,安装后会有命令行clang编译器，无须安装庞大的`xcode`

### Usage

#### Use in CLI

```shell
./uploader filepath
```

例如上传 位置在`/user/a/a.png`的图片 ，则执行 `./uploader /user/a/a.png`

#### work with typroa

在Typora图像设置中，上传服务选择`Custom Command `,自定义命令指向编译好的二进制文件uploader全路径即可

## Why

#### 初衷

之前在Windows下用Typroa，它使用Node.js，使用第三方插件很方便，在macOS下未使用Node.js，自己电脑不想安装Node.js，安装完一大堆依赖，所以用C语言写了，macOS下 ***LLVM version 9.0.0***编译完只有14k大小，完全满足需求，后期考虑增加其他图床支持，增加大文件上传，分块上传等

