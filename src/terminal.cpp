#include "terminal.h"
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <thread>
#include <mutex>

static std::string terminalBuffer;
static std::mutex terminalMutex;
static bool terminalInitialized = false;

void initializeTerminal() {
    if (!terminalInitialized) {
        terminalInitialized = true;
        std::thread([]() {
            std::array<char, 128> buffer;
            std::shared_ptr<FILE> pipe(popen("/bin/sh", "r"), pclose);
            if (!pipe) throw std::runtime_error("popen() failed!");
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                std::lock_guard<std::mutex> lock(terminalMutex);
                terminalBuffer += buffer.data();
            }
        }).detach();
    }
}

void runCommand(const std::string& command, std::string& output) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    std::lock_guard<std::mutex> lock(terminalMutex);
    output += result;
}

void processTerminalInput(std::string& terminalOutput) {
    std::lock_guard<std::mutex> lock(terminalMutex);
    terminalOutput += terminalBuffer;
    terminalBuffer.clear();
}
