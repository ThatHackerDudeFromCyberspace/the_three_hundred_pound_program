#pragma once

#include <cstddef>
#include <cstdint>

class GlobalState {
    public:
        static GlobalState* GetGlobalState() {
            if (globalState == NULL) {
                globalState = new GlobalState();
            }
            return globalState;
        };

        bool standby=false; // Whether the whole system is entering standby mode
        uint64_t frameNumber;

    private:
        static GlobalState* globalState;
        GlobalState() {};
};