# 产品需求文档 (PRD)

## 1. 项目背景
FastDeploy-UIE-Windows 是一个基于百度飞桨 FastDeploy 的命令行工具，旨在在 Windows 平台上运行 UIE（Universal Information Extraction）模型，用于抽取和识别结构化信息。该工具主要服务于需要高效处理自然语言数据的开发者，提供便捷的命令行操作和结构化输出，便于后续数据处理和分析。

目标：
- 提供一个高效、易用的工具，支持在 Windows 平台上运行 UIE 模型。
- 输出结构化的识别结果，便于集成到其他系统中。
- 为开发者提供清晰的接口和扩展能力。

---

## 2. 典型用户用例

### 用例 1：文本信息抽取
**描述**：用户希望从一段文本中提取特定的实体（如人名、日期、地点等）。
- **输入**：包含自然语言文本的文件（如 `input.txt`）。
- **输出**：JSON 格式的结构化数据，包含提取的实体及其类别。
- **示例**：
  - 输入文件内容：
    ```
    张三于2025年4月3日在北京参加了会议。
    ```
  - 输出结果：
    ```json
    [
      {"entity": "张三", "type": "Person", "start": 0, "end": 2},
      {"entity": "2025年4月3日", "type": "Date", "start": 3, "end": 12},
      {"entity": "北京", "type": "Location", "start": 13, "end": 15}
    ]
    ```

### 用例 2：批量处理文件
**描述**：用户希望批量处理多个文本文件，并将结果保存到指定目录。
- **输入**：包含多个文本文件的目录路径。
- **输出**：每个文件对应的 JSON 结果文件。
- **示例**：
  - 输入目录：`./data/`
  - 输出目录：`./results/`
  - 输出文件内容（如 `file1.json`）：
    ```json
    [
      {"entity": "张三", "type": "Person", "start": 0, "end": 2},
      {"entity": "2025年4月3日", "type": "Date", "start": 3, "end": 12}
    ]
    ```

---

## 3. 技术选型架构

### 技术选型
- **深度学习框架**：百度飞桨（PaddlePaddle）
- **模型部署工具**：FastDeploy
- **编程语言**：C++（核心逻辑）、Python（扩展功能）
- **构建工具**：CMake
- **运行环境**：Windows 10 或更高版本

### 系统架构
1. **输入模块**：负责读取用户提供的文本文件或目录。
2. **模型推理模块**：加载 UIE 模型并执行推理，提取结构化信息。
3. **输出模块**：将推理结果保存为 JSON 格式，支持单文件和批量处理。
4. **扩展模块**（可选）：提供 Python 接口，便于用户自定义功能。

---

## 4. 核心流程

### 流程 1：单文件处理
1. 用户通过命令行指定模型路径和输入文件路径。
2. 系统加载 UIE 模型。
3. 系统读取输入文件内容。
4. 系统执行模型推理，提取结构化信息。
5. 系统将结果保存为 JSON 文件。

### 流程 2：批量文件处理
1. 用户通过命令行指定模型路径和输入目录路径。
2. 系统加载 UIE 模型。
3. 系统遍历输入目录中的所有文件。
4. 对每个文件执行模型推理，提取结构化信息。
5. 系统将每个文件的结果保存为对应的 JSON 文件。

---

## 5. 关键技术实现

### 示例 1：加载模型
```cpp
// filepath: src/model_loader.cpp
#include "fastdeploy/runtime.h"

fastdeploy::Runtime LoadModel(const std::string& model_path) {
    fastdeploy::RuntimeOption option;
    option.model_path = model_path;
    option.device = fastdeploy::Device::CPU;
    fastdeploy::Runtime runtime(option);
    if (!runtime.Initialized()) {
        throw std::runtime_error("Failed to initialize the model.");
    }
    return runtime;
}
```

---

## 6. 新增功能建议

### 示例 2：支持 GPU 加速
```cpp
// filepath: src/model_loader.cpp
#include "fastdeploy/runtime.h"

fastdeploy::Runtime LoadModel(const std::string& model_path, bool use_gpu) {
    fastdeploy::RuntimeOption option;
    option.model_path = model_path;
    option.device = use_gpu ? fastdeploy::Device::GPU : fastdeploy::Device::CPU;
    fastdeploy::Runtime runtime(option);
    if (!runtime.Initialized()) {
        throw std::runtime_error("Failed to initialize the model.");
    }
    return runtime;
}
```
此功能允许用户选择是否使用 GPU 加速，从而提高模型推理的效率。