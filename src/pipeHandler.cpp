#include "pipeHandler.h"
#include <cstdlib>
#include <istream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>      // std::fstream

PipeHandler* PipeHandler::pipeHandler = NULL;

PipeHandler::PipeHandler() {
    if (getenv("XDG_RUNTIME_DIR") != NULL) {
        fifoPath = getenv("XDG_RUNTIME_DIR");
    };

    if (std::filesystem::exists(fifoPath / "300_fifo")) {
        remove((fifoPath / "300_fifo").c_str());
    }

    printf("[Fuzzer300] - Creating fifo file: [%s]\n", (fifoPath / "300_fifo").c_str());
    if (mkfifo((fifoPath / "300_fifo").c_str(), 0666) != 0) {
        printf("[Fuzzer300] - Failed to create fifo at path [%s]\n", fifoPath.c_str());
        throw std::runtime_error("Could not create fifo");
    }
    readerThread = std::thread(&PipeHandler::readThreadWorker, this);
}

void PipeHandler::readThreadWorker() {
    printf("[Fuzzer300] PipeHandler thread worker started!\n");
    while (true) {
        printf("[Fuzzer300] Attempting to open fifo file\n");
        std::ifstream fs(fifoPath / "300_fifo", std::iostream::in | std::iostream::binary);
        if (!fs.good() || !fs.is_open()) {
            printf("[Fuzzer300] Fifo file wasn't good\n");
            fs.close();
            continue;
        }
        printf("[Fuzzer300] Openned the fifo\n");
        uint8_t hand=0; // 0 = left, 1 = right
        Message message;
        bool abort = false;

        // Read the handedness
        printf("[Fuzzer300] Reading handedness\n");
        fs.read((char*) &hand, sizeof(hand)); // 1 byte so this is ok
        if (fs.eof()) { fs.close(); continue; } // Unexpectedly ended

        // Read the pathlen
        message.pathLen = 0;
        printf("[Fuzzer300] Reading pathlen\n");
        for (int i=0; i < sizeof(message.pathLen); i++) {
            message.pathLen |= fs.get() << (i * 8);
            if (fs.eof()) { abort = true; break; } // Unexpectedly ended
        }
        if (abort) { fs.close(); continue; }

        // Read the path
        printf("[Fuzzer300] Reading path\n");
        for (uint32_t i=0; i < message.pathLen; i++) {
            message.path += fs.get();
            if (fs.eof()) { abort = true; break; } // Unexpectedly ended
        }
        if (abort) { fs.close(); continue; }

        // Read the paramlen
        message.paramLen = 0;
        printf("[Fuzzer300] Reading paramlen\n");
        for (int i=0; i < sizeof(message.paramLen); i++) {
            message.paramLen |= fs.get() << (i * 8);
            if (fs.eof()) { abort = true; break; } // Unexpectedly ended
        }
        if (abort) { fs.close(); continue; }

        // Read the param
        printf("[Fuzzer300] Reading param\n");
        for (uint32_t i=0; i < message.paramLen; i++) {
            message.param += fs.get();
            if (fs.eof()) { abort = true; break; } // Unexpectedly ended
        }

        printf("[Fuzzer300] IPC FIFO Message received: |%i| [%s]:[%s] - (%i):(%i)\n", hand, message.path.c_str(), message.param.c_str(), message.pathLen, message.paramLen);

        if (hand == 0) {
            leftMessagesMutex.lock();
            leftMessages.push(message);
            leftMessagesMutex.unlock();
        } else {
            rightMessagesMutex.lock();
            rightMessages.push(message);
            rightMessagesMutex.unlock();
        }

        fs.close();
    }
}

Message PipeHandler::PopLeftMessage() {
    std::shared_lock lock(leftMessagesMutex);
    Message message = leftMessages.front();
    leftMessages.pop();
    return message;
}

Message PipeHandler::PopRightMessage() {
    std::shared_lock lock(rightMessagesMutex);
    Message message = rightMessages.front();
    rightMessages.pop();
    return message;
}

bool PipeHandler::LeftMessageAvailable() {
    std::shared_lock lock(leftMessagesMutex);
    return !leftMessages.empty();
}

bool PipeHandler::RightMessageAvailable() {
    std::shared_lock lock(rightMessagesMutex);
    return !rightMessages.empty();
}