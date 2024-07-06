#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>

void initializeTerminal();
void runCommand(const std::string& command, std::string& output);
void processTerminalInput(std::string& terminalOutput);

#endif
