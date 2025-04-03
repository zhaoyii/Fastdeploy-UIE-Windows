// Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <iostream>
#include <sstream>
#include <vector>
#include <locale>
#include <codecvt>

#include "fastdeploy/text.h"
#include <nlohmann/json.hpp>
#include <windows.h>
#include "uie/uie_predictor.hpp"

using namespace paddlenlp;
using json = nlohmann::json;

#ifdef WIN32
const char sep = '\\';
#else
const char sep = '/';
#endif

std::vector<std::string> split(const std::string &s, char delimiter)
{
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter))
  {
    tokens.push_back(token);
  }
  return tokens;
}

/**
 * 命令行参数转宽字符串 wstring
 */
std::wstring arg_to_wstring(char *arg)
{
  int wide_size = MultiByteToWideChar(CP_ACP, 0, arg, -1, NULL, 0);
  wchar_t *wide_str = new wchar_t[wide_size];
  MultiByteToWideChar(CP_ACP, 0, arg, -1, wide_str, wide_size);
  std::wstring wtext(wide_str);
  delete[] wide_str;

  return wtext;
}

/**
 * 宽字符串 wstring 转成 str
 */
std::string wide_to_utf8(const std::wstring &wstr)
{
  if (wstr.empty())
    return std::string();
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
  std::string strTo(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
  return strTo;
}

/**
 * 命令行参数字符串转成 utf-8 格式
 *
 * windows 平台下，命令行输入参数字符串，直接输入到模型进行推理，
 * 会导致推理任务死循环。所以需要转换输入的字符编码。
 */
std::string arg_to_utf8(char *arg)
{
  std::wstring wtext = arg_to_wstring(arg);
  std::string text = wide_to_utf8(wtext);
  return text;
}

int main(int argc, char *argv[])
{
  if (argc != 10)
  {
    std::cout << "Usage: " << argv[0] << " <model_dir> <device> <backend> <position_prob> "
              << "<max_length> <schema> <batch_size> <feature> <text>" << std::endl;
    std::cout << "Parameters:" << std::endl;
    std::cout << "  model_dir: Path to model directory" << std::endl;
    std::cout << "  device: 0 for CPU, 1 for GPU" << std::endl;
    std::cout << "  backend: 0 for Paddle, 1 for ONNX, 2 for OpenVINO" << std::endl;
    std::cout << "  position_prob: Float value for position probability" << std::endl;
    std::cout << "  max_length: Integer value for maximum length" << std::endl;
    std::cout << "  schema: Comma-separated schema values" << std::endl;
    std::cout << "  batch_size: Integer value for batch size" << std::endl;
    std::cout << "  feature: Feature type (integer)" << std::endl;
    std::cout << "  text: Input text for prediction" << std::endl;
    return -1;
  }

  try
  {
    // Model paths
    std::string model_dir(argv[1]);

    // Parse command line arguments
    int device = std::atoi(argv[2]);
    int backend = std::atoi(argv[3]);
    float position_prob = std::stof(argv[4]);
    int max_length = std::atoi(argv[5]);
    std::string schema_str = arg_to_utf8(argv[6]);
    std::vector<std::string> schema = split(schema_str, ',');
    int batch_size = std::atoi(argv[7]);
    int feature = std::atoi(argv[8]);
    std::string text = arg_to_utf8(argv[9]);

    // Validate parameters
    if (device != 0 && device != 1)
    {
      throw std::invalid_argument("Device must be 0 (CPU) or 1 (GPU)");
    }
    if (backend < 0 || backend > 2)
    {
      throw std::invalid_argument("Backend must be 0 (Paddle), 1 (ONNX), or 2 (OpenVINO)");
    }
    if (position_prob < 0.0f || position_prob > 1.0f)
    {
      throw std::invalid_argument("Position probability must be between 0 and 1");
    }
    if (max_length <= 0)
    {
      throw std::invalid_argument("Max length must be positive");
    }
    if (batch_size <= 0)
    {
      throw std::invalid_argument("Batch size must be positive");
    }
    if (schema.empty())
    {
      throw std::invalid_argument("Schema cannot be empty");
    }

    // Create and initialize predictor
    UIEPredictor predictor(model_dir, device, backend, position_prob,
                           max_length, schema, batch_size);

    if (!predictor.Initialize())
    {
      throw std::runtime_error("Failed to initialize predictor");
    }

    // Predict and output results
    std::string result = predictor.Predict(text, feature);
    // 设置输出中文 utf-8
    std::locale::global(std::locale("zh_CN.UTF-8"));
    std::wcout.imbue(std::locale());
    std::cout << result << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}