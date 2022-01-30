#include "Utils/FileUtils.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

namespace FileUtils {    
    std::vector<std::string> getFiles(
        std::string_view path) {
    std::vector<std::string> directories;
    if (!std::filesystem::is_directory(path)) return directories;
    std::error_code ec;
    auto directory_iterator = std::filesystem::directory_iterator(
        path, std::filesystem::directory_options::none, ec);
    if (ec) {
    }
    for (const auto& entry : directory_iterator) {
        if (entry.is_regular_file()) directories.push_back(entry.path().string());
    }
    return directories;
    }

    std::string RemoveExtension(std::string path) {
    if (path.find_last_of(".") != std::string::npos)
        return path.substr(0, path.find_last_of("."));
    return path;
    }

    std::string GetFileName(std::string path, bool removeExtension) {
    std::string result = "";
    if (path.find_last_of("/") != std::string::npos)
        result = path.substr(path.find_last_of("/") + 1);
    else
        result = path;
    if (removeExtension) result = RemoveExtension(result);
    return result;
    ;
    }

    bool isImageFile(std::string path) {
    if (path.ends_with(".png") || path.ends_with(".jpg")) {
        return true;
    }
    return false;
    }

    bool isOGGFile(std::string path) {
    if (path.ends_with(".gif")) {
        return true;
    }
    return false;
    }
}