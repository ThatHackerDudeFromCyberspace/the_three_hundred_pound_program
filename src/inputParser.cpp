#include "inputParser.h"

#include <fstream>
#include "nlohmann/json.hpp"

std::map<std::string, InputInfo> parseInputList(const std::filesystem::path& inputPath) {
    std::map<std::string, InputInfo> inputList;

    std::ifstream file(inputPath);
    nlohmann::json data = nlohmann::json::parse(file);

    printf("[Fuzzer300] Loading input definition: %s\n", inputPath.c_str());
    printf("[Fuzzer300] Got input paths:\n");

    for (auto& inputSource : data["inputs"]) {
        Side side = Side::BOTH;
        if (!inputSource["side"].is_null()) {
            std::string inputSide = inputSource["side"];
            if (inputSide == "left") {
                side = Side::LEFT;
            } else if (inputSide == "right") {
                side = Side::RIGHT;
            }
        }
        
        std::string inputPath = inputSource["path"];
        std::string inputTypeString = inputSource["type"].get<std::string>();

        InputType inputType;
        if (inputTypeString == "boolean") {
            inputType = InputType::BOOLEAN;
        }
        else if (inputTypeString == "scalar_one_sided") {
            inputType = InputType::SCALAR_ONE_SIDED;
        }
        else if (inputTypeString == "scalar_two_sided") {
            inputType = InputType::SCALAR_TWO_SIDED;
        }
        else
        {
            continue;
        }

        inputList[inputPath] = {
            .inputType = inputType,
            .side = side
        };
    }

    printf("\n\n[Fuzzer300] Generated inputs:\n");
    for (auto& input : inputList) {
        std::string inputType;
        switch (input.second.inputType) {
            case InputType::SCALAR_ONE_SIDED:
                inputType = "SCALAR (0f 1f)";
                break;
            case InputType::SCALAR_TWO_SIDED:
                inputType = "SCALAR (-1f 1f)";
                break;
            case InputType::BOOLEAN:
                inputType = "BOOLEAN (0/1)";
                break;
        }

        std::string side;
        switch (input.second.side) {
            case Side::LEFT:
                side = "LEFT";
                break;
            case Side::RIGHT:
                side = "RIGHT";
                break;
                case Side::BOTH:
                side="BOTH";
                break;
        }

        printf("[Fuzzer300] (%s) %s - %s\n", side.c_str(), input.first.c_str(), inputType.c_str());
    }
    
    return inputList;
}
