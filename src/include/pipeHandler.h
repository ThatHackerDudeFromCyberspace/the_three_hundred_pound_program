#pragma once

#include <cstdint>
#include <shared_mutex>
#include <queue>
#include <thread>
#include <string>
#include <filesystem>

struct Message {
    uint32_t pathLen;
    std::string path;
    uint32_t paramLen;
    std::string param;
};

class PipeHandler {
public:
    static PipeHandler* GetPipeHandler() {
        if (pipeHandler == NULL) {
            pipeHandler = new PipeHandler();
        }
        return pipeHandler;
    };

    Message PopLeftMessage();
    Message PopRightMessage();
    bool LeftMessageAvailable();
    bool RightMessageAvailable();
private:
    static PipeHandler* pipeHandler;

    PipeHandler();
    void readThreadWorker();

    std::thread readerThread;
    std::filesystem::path fifoPath = "/tmp";

    std::queue<Message> leftMessages;
    std::queue<Message> rightMessages;
    std::shared_mutex leftMessagesMutex;
    std::shared_mutex rightMessagesMutex;
};