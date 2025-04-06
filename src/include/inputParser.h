#pragma once

#include <filesystem>
#include <map>
#include <string>

enum class InputType {
    SCALAR_ONE_SIDED, // 0-1 float
    SCALAR_TWO_SIDED, // -1 to 1 float
    BOOLEAN // duh
};

enum class Side {
    LEFT = 1 << 0,
    RIGHT = 1 << 1,
    BOTH = LEFT | RIGHT
};

struct InputInfo {
    InputType inputType;
    Side side;
};

std::map<std::string, InputInfo> parseInputList(const std::filesystem::path& path);