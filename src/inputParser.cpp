#include "inputParser.h"

#include <fstream>
#include "nlohmann/json.hpp"

std::map<std::string, InputInfo> parseInputList(const std::filesystem::path& inputPath) {
    std::map<std::string, InputInfo> inputList;

    std::ifstream file(inputPath);
    nlohmann::json data = nlohmann::json::parse(file);

    printf("Fuzzer300 Loading input profile: %s\n", data["jsonid"].get<std::string>().c_str());
    printf("Fuzzer300 Got input paths:\n");
    
    for (auto& inputSource : data["input_source"].items()) {
        Side side = Side::BOTH;
        if (!inputSource.value()["side"].is_null()) {
            std::string inputSide = inputSource.value()["side"];
            if (inputSide == "left") {
                side = Side::LEFT;
            } else if (inputSide == "right") {
                side = Side::RIGHT;
            }
        }
        
        std::string inputPath = inputSource.key();
        std::string inputType = inputSource.value()["type"].get<std::string>();

        
        if (inputType == "joystick") {
            // Register /x and /y
            inputList[inputPath + "/x"] = {
                .inputType = InputType::SCALAR_TWO_SIDED,
                .side = side
            };

            inputList[inputPath + "/y"] = {
                .inputType = InputType::SCALAR_TWO_SIDED,
                .side = side
            };
        }


        if ((!inputSource.value()["click"].is_null()) && inputSource.value()["click"].get<bool>()) {
            inputList[inputPath + "/click"] = {
                .inputType = InputType::BOOLEAN,
                .side = side
            };
        }
        if ((!inputSource.value()["value"].is_null()) && inputSource.value()["value"].get<bool>()) {
            inputList[inputPath + "/value"] = {
                .inputType = InputType::SCALAR_ONE_SIDED,
                .side = side
            }; // Docs say this should be ONE_SIDED btw - "value - Optional. Represents that the input can sense a scalar value ranging from 0-1. Valid types are trigger."
        }
        if ((!inputSource.value()["touch"].is_null()) && inputSource.value()["touch"].get<bool>()) {
            inputList[inputPath + "/touch"] = {
                .inputType = InputType::BOOLEAN,
                .side = side
            };
        }
        if ((!inputSource.value()["force"].is_null()) && inputSource.value()["force"].get<bool>()) {
            inputList[inputPath + "/force"] = {
                .inputType = InputType::SCALAR_ONE_SIDED,
                .side = side
            }; // Strictly speaking docs don't state this should be ONE_SIDED but like come on
        }
    }

    return inputList;
}