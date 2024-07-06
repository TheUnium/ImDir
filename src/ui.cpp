#include "imgui.h"
#include "file_operations.h"
#include "terminal.h"
#include "IconsLucide.h"
#include <cstdlib>
#include <filesystem>
#include <imgui_impl_opengl3.h>
#include <stack>

static float sidebarWidth = 200.0f;
static float terminalHeight = 150.0f;

std::stack<std::string> backHistory;
std::stack<std::string> forwardHistory;

void navigateBack(std::string& currentPath) {
    if (!backHistory.empty()) {
        forwardHistory.push(currentPath);
        currentPath = backHistory.top();
        backHistory.pop();
    }
}

void navigateForward(std::string& currentPath) {
    if (!forwardHistory.empty()) {
        backHistory.push(currentPath);
        currentPath = forwardHistory.top();
        forwardHistory.pop();
    }
}

void navigateUp(std::string& currentPath) {
    std::filesystem::path parentPath = std::filesystem::path(currentPath).parent_path();
    if (!parentPath.empty()) {
        backHistory.push(currentPath);
        currentPath = parentPath.string();
        while (!forwardHistory.empty()) forwardHistory.pop();
    }
}

void setupImGuiFonts() {
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;
    config.GlyphMinAdvanceX = 13.0f;

    io.Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/hack/Hack-Bold.ttf", 16.0f);
    static const ImWchar icons_ranges[] = { ICON_MIN_LC, ICON_MAX_LC, 0 };
    io.Fonts->AddFontFromFileTTF("/home/unium/Projects/personal/ImFileManager/icons/lucide.ttf", 16.0f, &config, icons_ranges);
    ImGui_ImplOpenGL3_CreateFontsTexture();
}

void renderBreadcrumbs(const std::string& currentPath) {
    std::vector<std::string> directories;
    std::string pathCopy = currentPath;

    while (!pathCopy.empty()) {
        size_t slashPos = pathCopy.find('/');
        if (slashPos == std::string::npos) {
            directories.push_back(pathCopy);
            break;
        } else {
            directories.push_back(pathCopy.substr(0, slashPos));
            pathCopy = pathCopy.substr(slashPos + 1);
        }
    }

    std::string accumulatedPath;
    for (const auto& directory : directories) {
        accumulatedPath += directory + " > ";
    }

    ImGui::Text("%s", accumulatedPath.c_str());
}

void renderSidebar(const std::string& currentPath, std::string& selectedPath) {
    ImGui::BeginChild("Sidebar", ImVec2(sidebarWidth, 0), true);
    auto entries = getDirectoryEntries(currentPath);
    for (const auto& entry : entries) {
        std::string filename = std::filesystem::path(entry).filename().string();
        if (filename[0] == '.') {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        }
        if (isDirectory(entry)) {
            if (ImGui::Selectable((std::string(ICON_LC_FOLDER " ") + filename).c_str())) {
                selectedPath = entry;
            }
        } else {
            if (ImGui::Selectable((std::string(ICON_LC_FILE " ") + filename).c_str())) {
                selectedPath = entry;
            }
        }
        if (filename[0] == '.') {
            ImGui::PopStyleColor();
        }
    }
    ImGui::EndChild();
}

void renderMainContent(std::string& currentPath, std::string& selectedPath) {
    ImGui::BeginChild("Main Content", ImVec2(0, -terminalHeight), true);
    auto entries = getDirectoryEntries(currentPath);
    for (const auto& entry : entries) {
        std::string filename = std::filesystem::path(entry).filename().string();
        if (filename[0] == '.') {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        }
        if (isDirectory(entry)) {
            if (ImGui::Selectable((std::string(ICON_LC_FOLDER " ") + filename).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                if (ImGui::IsMouseDoubleClicked(0)) {
                    currentPath = entry;
                }
            }
        } else {
            if (ImGui::Selectable((std::string(ICON_LC_FILE " ") + filename).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                if (ImGui::IsMouseDoubleClicked(0)) {
                    std::string command = "xdg-open " + entry;
                    std::system(command.c_str());
                }
            }
        }
        if (filename[0] == '.') {
            ImGui::PopStyleColor();
        }
    }
    ImGui::EndChild();
}

void renderTerminal(std::string& terminalOutput) {
    ImGui::BeginChild("Terminal", ImVec2(0, terminalHeight), true);

    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::TextUnformatted(terminalOutput.c_str());
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();

    ImGui::Separator();
    ImGui::Text("$ ");
    ImGui::SameLine();
    static char command[256];
    if (ImGui::InputText("##Command", command, IM_ARRAYSIZE(command), ImGuiInputTextFlags_EnterReturnsTrue)) {
        terminalOutput += "$ " + std::string(command) + "\n";
        runCommand(command, terminalOutput);
        command[0] = '\0';
        ImGui::SetKeyboardFocusHere(-1);
    }

    ImGui::EndChild();
}

void renderNavigationBar(std::string& currentPath) {
    if (ImGui::Button((std::string(ICON_LC_ARROW_UP " ")).c_str())) {
        navigateUp(currentPath);
    }
    ImGui::SameLine();
    if (ImGui::Button((std::string(ICON_LC_ARROW_LEFT " ")).c_str())) {
        navigateBack(currentPath);
    }
    ImGui::SameLine();
    if (ImGui::Button((std::string(ICON_LC_ARROW_RIGHT " ")).c_str())) {
        navigateForward(currentPath);
    }
}

void renderUI(std::string& currentPath, std::string& selectedPath, std::string& terminalOutput) {
    ImGui::Begin("ImFileManager", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::BeginChild("NavigationBar", ImVec2(0, 38), true);
    renderNavigationBar(currentPath);
    ImGui::SameLine();
    renderBreadcrumbs(currentPath);
    ImGui::EndChild();

    ImGui::BeginChild("LeftPane", ImVec2(sidebarWidth, 0), true);
    auto entries = getDirectoryEntries(currentPath);
    for (const auto& entry : entries) {
        if (isDirectory(entry)) {
            if (ImGui::Selectable((std::string(ICON_LC_FOLDER " ") + entry).c_str())) {
                selectedPath = entry;
            }
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::BeginChild("RightPane", ImVec2(0, 0), true);
    renderMainContent(currentPath, selectedPath);

    ImGui::Separator();
    renderTerminal(terminalOutput);

    ImGui::EndChild();
    ImGui::End();
}
