#include "inputParser.h"
#include "pipeHandler.h"
#include <cstdio>
#include <fstream>
#include <string>

#include "nlohmann/json.hpp"
#include "openvr_driver.h"


int main() {
    printf("Reading json...\n");

    std::filesystem::path inputPath = getenv("HOME");
    inputPath /= ".steam/steam/steamapps/common/SteamVR/drivers/three_hundred_fuzzer/resources/input/controller_profile.json";

    std::map<std::string, InputInfo> inputList = parseInputList(inputPath);

    printf("\n\nParsed inputs:\n");
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

        printf("(%s) %s - %s\n", side.c_str(), input.first.c_str(), inputType.c_str());
    }
    printf("\n\n");

    printf("300TEST running...\n");
    while (true) {
        while (PipeHandler::GetPipeHandler()->LeftMessageAvailable()) {
            PipeHandler::GetPipeHandler()->PopLeftMessage();
        }
        while (PipeHandler::GetPipeHandler()->RightMessageAvailable()) {
            PipeHandler::GetPipeHandler()->PopRightMessage();
        }
    }
    return 0;
}