#pragma once

#include <string>

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/types.hpp"

namespace FileUtils {
    bool isImageFile(std::string path);
    std::string RemoveExtension(std::string path);
    std::string GetFileName(std::string path, bool removeExtension);
    std::vector<std::string> getFiles(std::string_view path);
    bool isOGGFile(std::string path);
}
