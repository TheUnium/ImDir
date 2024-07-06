#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <string>
#include <vector>

std::vector<std::string> getDirectoryEntries(const std::string& path);
bool isDirectory(const std::string& path);

#endif
