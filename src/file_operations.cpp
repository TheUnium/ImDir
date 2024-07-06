#include "file_operations.h"
#include <filesystem>
#include <algorithm>

std::vector<std::string> getDirectoryEntries(const std::string& path) {
    std::vector<std::string> entries;
    std::vector<std::string> hiddenEntries;

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string filename = entry.path().filename().string();
        if (filename == "." || filename == "..") {
            entries.push_back(entry.path().string());
        } else if (filename[0] == '.') {
            hiddenEntries.push_back(entry.path().string());
        } else {
            entries.push_back(entry.path().string());
        }
    }

    std::sort(entries.begin(), entries.end(), [](const std::string& a, const std::string& b) {
        return std::filesystem::is_directory(a) && !std::filesystem::is_directory(b);
    });

    entries.insert(entries.end(), hiddenEntries.begin(), hiddenEntries.end());

    return entries;
}

bool isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}
