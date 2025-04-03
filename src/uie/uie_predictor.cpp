#include "uie_predictor.hpp"

UIEPredictor::UIEPredictor(const std::string& model_dir,
                          int device,
                          int backend_type,
                          float position_prob,
                          int max_length,
                          const std::vector<std::string>& schema,
                          int batch_size)
    : model_dir_(model_dir),
      device_(device),
      backend_type_(backend_type),
      position_prob_(position_prob),
      max_length_(max_length),
      schema_(schema),
      batch_size_(batch_size) {}

bool UIEPredictor::Initialize() {
    #ifdef WIN32
    const char sep = '\\';
    #else
    const char sep = '/';
    #endif

    std::string model_path = model_dir_ + sep + "inference.pdmodel";
    std::string param_path = model_dir_ + sep + "inference.pdiparams";
    std::string vocab_path = model_dir_ + sep + "vocab.txt";

    auto option = fastdeploy::RuntimeOption();
    
    // Set device
    if (device_ == 0) {
        option.UseCpu();
    } else {
        option.UseGpu();
    }

    // Set backend
    switch (backend_type_) {
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
            return false;
    }

    try {
        predictor_ = std::make_unique<fastdeploy::text::UIEModel>(
            model_path, param_path, vocab_path,
            position_prob_,
            max_length_,
            schema_,
            batch_size_,
            option);
        return true;
    } catch (...) {
        return false;
    }
}

std::string UIEPredictor::Predict(const std::string& text, int feature) {
    std::vector<std::unordered_map<std::string, std::vector<fastdeploy::text::UIEResult>>> results;
    
    if (feature == 2) {
        predictor_->SetSchema(fastdeploy::text::SchemaNode(schema_[0]));
    }

    predictor_->Predict({text}, &results);
    return ConvertResultsToJson(results).dump();
}

void UIEPredictor::SetSchema(const std::string& schema) {
    predictor_->SetSchema(fastdeploy::text::SchemaNode(schema));
}

nlohmann::json UIEPredictor::ConvertResultsToJson(
    const std::vector<std::unordered_map<std::string, std::vector<fastdeploy::text::UIEResult>>>& results) {
    nlohmann::json output;
    for (const auto& result_map : results) {
        for (const auto& pair : result_map) {
            const std::string& key = pair.first;
            const std::vector<fastdeploy::text::UIEResult>& ui_results = pair.second;
            nlohmann::json entities;
            for (const auto& ui_result : ui_results) {
                nlohmann::json entity;
                entity["text"] = ui_result.text_;
                entity["start"] = ui_result.start_;
                entity["end"] = ui_result.end_;
                entity["probability"] = ui_result.probability_;
                entities.push_back(entity);
            }
            output[key] = entities;
        }
    }
    return output;
}