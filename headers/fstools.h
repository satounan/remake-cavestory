#include <filesystem>

namespace fs = std::filesystem;

fs::path replacePathComponentByName(const fs::path& originalPath, const fs::path& targetDirName, const fs::path& newPart) {
    // 构建新的路径
    fs::path newPath;
    bool replaced = false;

    for (const auto& entry : originalPath) {
        if (!replaced && entry == targetDirName) {
            // 替换目标目录名为新部分
            newPath /= newPart;
            replaced = true;
        } else {
            // 保留其他部分不变
            newPath /= entry;
        }
    }
    return newPath;
}