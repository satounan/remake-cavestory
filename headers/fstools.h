#include <filesystem>

namespace fs = std::filesystem;

inline fs::path replacePathComponentByName(const fs::path& originalPath, const fs::path& targetDirName, const fs::path& newPart) {
    // 构建新的路径
    fs::path newPath;
    bool foundTarget = false;
    bool replaced = false;

    for (const auto& entry : originalPath) {
        if (!foundTarget && entry == targetDirName) {
            // 找到了目标目录名，添加到新路径中
            newPath /= entry;
            foundTarget = true;
        } else if (foundTarget && !replaced) {
            // 一旦找到了目标目录，则替换紧随其后的第一个组件
            newPath /= newPart;
            replaced = true; // 确保只替换一次
        } else {
            // 目标目录之前的部分或已经替换后的部分保持不变
            newPath /= entry;
        }
    }

    if (foundTarget && !replaced) {
        throw std::runtime_error("No component to replace after the target directory.");
    }

    return newPath;
}


inline fs::path truncatePathAtDirectoryName(const fs::path& originalPath, const std::string& targetDirName) {
    // 构建新的路径
    fs::path truncatedPath;
    bool foundTarget = false;

    for (const auto& entry : originalPath) {
        if (!foundTarget && entry.string() == targetDirName) {
            // 找到了目标目录名，添加到新路径中
            truncatedPath /= entry;
            foundTarget = true;
        } else if (!foundTarget) {
            // 在找到目标目录之前的部分保持不变
            truncatedPath /= entry;
        } else {
            // 一旦找到了目标目录，就不再添加后续部分
            break;
        }
    }

    if (!foundTarget) {
        throw std::runtime_error("Target directory name not found in the path.");
    }

    return truncatedPath;
}


inline fs::path getAbsolutePath(const char* relativePath) {
        // 将 C-style 字符串转换为 filesystem::path 并获取其绝对路径
        fs::path absPath = fs::absolute(relativePath);
        return absPath;
}

inline std::string removeLeadingDotDot(const std::string& input) {
    // 检查字符串是否以 "../" 开头
    if (input.size() >= 3 && input.substr(0, 3) == "../") {
        // 返回去掉 "../" 后的字符串
        return input.substr(3);
    } else {
        // 如果不以 ".." 开头，则返回原字符串
        return input;
    }
}
