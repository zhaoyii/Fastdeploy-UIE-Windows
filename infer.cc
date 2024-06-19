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
#include "json.hpp"
#include <windows.h>

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
  // if (argc != 3 && argc != 4) {
  //   std::cout << "Usage: infer_demo /path/to/model device [backend], "
  //                "e.g ./infer_demo uie-base 0 [0]"
  //             << std::endl;
  //   std::cout << "The data type of device is int, 0: run with cpu; 1: run "
  //                "with gpu."
  //             << std::endl;
  //   std::cout << "The data type of backend is int, 0: use paddle backend; 1: "
  //                "use onnxruntime backend; 2: use openvino backend. Default 0."
  //             << std::endl;
  //   return -1;
  // }

  // 模型路径
  std::string model_dir(argv[1]);
  std::string model_path = model_dir + sep + "inference.pdmodel";
  std::string param_path = model_dir + sep + "inference.pdiparams";
  std::string vocab_path = model_dir + sep + "vocab.txt";
  using fastdeploy::text::SchemaNode;
  using fastdeploy::text::UIEResult;

  // 位置参数2 cpu or gpu
  auto option = fastdeploy::RuntimeOption();
  if (std::atoi(argv[2]) == 0)
  {
    option.UseCpu();
  }
  else
  {
    option.UseGpu();
  }
  // 位置参数3 后端类型
  auto backend_type = 0;
  backend_type = std::atoi(argv[3]);
  switch (backend_type)
  {
  case 0:
    option.UsePaddleInferBackend();
    break;
  case 1:
    option.UseOrtBackend();
    break;
  case 2:
    option.UseOpenVINOBackend();
    break;
  default:
    break;
  }

  // 可能值
  auto position_prob = std::stof(argv[4]);
  // 最长字符
  auto max_length = std::atoi(argv[5]);
  //
  // std::vector<std::string> schema = {"日期", "时间", "地点", "地名", "国家", "人物", "人名", "姓名", "机构", "组织", "公司"};
  std::string argv_6 = arg_to_utf8(argv[6]);
  std::vector<std::string> schema = split(argv_6, ',');
  // std::cout << argv[6] << std::endl;

  // 批量处理大小
  auto batch_size = std::atoi(argv[7]);

  // 模型初始化
  auto predictor = fastdeploy::text::UIEModel(
      model_path, param_path, vocab_path,
      position_prob,
      max_length,
      schema,
      batch_size,
      option);
  // std::cout << "After init predictor" << std::endl;

  // 功能 1实体提取 2情感分析
  auto feature = std::atoi(argv[8]);
  // 要预测的文本
  std::string argv_9 = arg_to_utf8(argv[9]);
  auto text = argv_9;
  // std::cout << text << std::endl;

  std::vector<std::unordered_map<std::string, std::vector<UIEResult>>> results;

  if (feature == 1)
  {
    // Named Entity Recognition
    // predictor.Predict({text}, &results);
  }
  else if (feature == 2)
  {
    // Sequence classification
    predictor.SetSchema(SchemaNode(argv[6]));
  }

  predictor.Predict({text}, &results);

  // 转换结果为 JSON 字符串
  json output;
  for (const auto &result_map : results)
  {
    for (const auto &pair : result_map)
    {
      const std::string &key = pair.first;
      const std::vector<UIEResult> &ui_results = pair.second;
      json entities;
      for (const auto &ui_result : ui_results)
      {
        json entity;
        entity["text"] = ui_result.text_;
        entity["start"] = ui_result.start_;
        entity["end"] = ui_result.end_;
        entity["probability"] = ui_result.probability_;
        entities.push_back(entity);
      }
      output[key] = entities;
    }
  }

  // 设置输出中文 utf-8
  std::locale::global(std::locale("zh_CN.UTF-8"));
  std::wcout.imbue(std::locale());

  // 输出 JSON 字符串
  std::cout << output.dump() << std::endl;
  results.clear();

  return 0;
}
