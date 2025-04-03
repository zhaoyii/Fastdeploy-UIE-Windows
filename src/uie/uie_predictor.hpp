#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fastdeploy/text.h>
#include "json.hpp"

class UIEPredictor {
public:
    UIEPredictor(const std::string& model_dir, 
                 int device, 
                 int backend_type,
                 float position_prob,
                 int max_length,
                 const std::vector<std::string>& schema,
                 int batch_size);
    
    bool Initialize();
    std::string Predict(const std::string& text, int feature);
    void SetSchema(const std::string& schema);

private:
    std::string model_dir_;
    int device_;
    int backend_type_;
    float position_prob_;
    int max_length_;
    std::vector<std::string> schema_;
    int batch_size_;
    
    std::unique_ptr<fastdeploy::text::UIEModel> predictor_;
    nlohmann::json ConvertResultsToJson(const std::vector<std::unordered_map<std::string, std::vector<fastdeploy::text::UIEResult>>>& results);
};