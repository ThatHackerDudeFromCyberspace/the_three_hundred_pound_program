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
    static void InitPipeHandler(const std::filesystem::path& fifoPath) {
        pipeHandler = new PipeHandler(fifoPath);
    };
    static PipeHandler* GetPipeHandler() {
        return pipeHandler;
    };

    Message PopLeftMessage();
    Message PopRightMessage();
    bool LeftMessageAvailable();
    bool RightMessageAvailable();
private:
    static PipeHandler* pipeHandler;

    PipeHandler(const std::filesystem::path& fifoPath);
    void readThreadWorker();

    std::thread readerThread;
    const std::filesystem::path fifoPath;

    std::queue<Message> leftMessages;
    std::queue<Message> rightMessages;
    std::shared_mutex leftMessagesMutex;
    std::shared_mutex rightMessagesMutex;
};
