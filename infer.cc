
#pragma execution_character_set("utf-8")

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

using namespace paddlenlp;

#ifdef WIN32
const char sep = '\\';
#else
const char sep = '/';
#endif

int main(int argc, char *argv[])
{

  if (argc != 3 && argc != 4)
  {
    std::cout << "Usage: infer_demo /path/to/model device [backend], "
                 "e.g ./infer_demo uie-base 0 [0]"
              << std::endl;
    std::cout << "The data type of device is int, 0: run with cpu; 1: run "
                 "with gpu."
              << std::endl;
    std::cout << "The data type of backend is int, 0: use paddle backend; 1: "
                 "use onnxruntime backend; 2: use openvino backend. Default 0."
              << std::endl;
    return -1;
  }
  auto option = fastdeploy::RuntimeOption();
  if (std::atoi(argv[2]) == 0)
  {
    option.UseCpu();
  }
  else
  {
    option.UseGpu();
  }
  auto backend_type = 0;
  if (argc == 4)
  {
    backend_type = std::atoi(argv[3]);
  }
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
  std::string model_dir(argv[1]);
  std::string model_path = model_dir + sep + "inference.pdmodel";
  std::string param_path = model_dir + sep + "inference.pdiparams";
  std::string vocab_path = model_dir + sep + "vocab.txt";

  std::cout << "model_path: " + model_path << std::endl;
  std::cout << "param_path: " + param_path << std::endl;
  std::cout << "vocab_path: " + vocab_path << std::endl;
  std::wcout << "测试输出中文 " << std::endl;

  using fastdeploy::text::SchemaNode;
  using fastdeploy::text::UIEResult;

  try
  {
    auto predictor = fastdeploy::text::UIEModel(
        model_path, param_path, vocab_path, 0.5, 128,
        {"时间", "选手", "赛事名称"}, /* batch_size = */ 1, option);
    std::cout << "After init predictor" << std::endl;
    std::vector<std::unordered_map<std::string, std::vector<UIEResult>>> results;
    // Named Entity Recognition
    predictor.Predict({"2月8日上午北京冬奥会自由式滑雪女子大跳台决赛中中国选手谷"
                       "爱凌以188.25分获得金牌！"},
                      &results);

    // 设置输出中文 utf-8
    std::locale::global(std::locale("zh_CN.UTF-8"));
    std::wcout.imbue(std::locale());
    std::cout << results << std::endl;
    results.clear();
  }
  catch (const std::exception &e)
  {
    // 捕获并记录异常信息到控制台
    std::cerr << "Error occurred: " << e.what() << std::endl;
  }
  catch (...)
  {
    // 捕获其他未知异常
    std::cerr << "Unknown error occurred." << std::endl;
  }

  return 0;
}
