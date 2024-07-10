#include "Utils.hpp"
#include <fstream>
#include <iterator>

namespace CS350 {

    void ChangeWorkdir(char const* folder)
    {
        int const   cMaxParentTraversal = 10;
        char const* cWorkdirFolder      = folder;

        // Change the workdir to the folder "bin" or similar
        std::filesystem::path initialPath = std::filesystem::current_path();
        std::filesystem::path workPath;
        if (CS350::FindSubFolderInParents(initialPath.string(), cWorkdirFolder, cMaxParentTraversal, workPath)) {
            std::filesystem::current_path(workPath);
        }
    }

    bool FindSubFolderRecursive(const std::string& path, const std::string& folderName, std::filesystem::path& outPath)
    {
        try {
            // For each subdirectory
            for (auto const& it : std::filesystem::recursive_directory_iterator(path)) {
                // Ignore files
                if (!std::filesystem::is_directory(path)) {
                    continue;
                }
                // Compare name
                auto filename = it.path().filename().string();
                if (filename == folderName) {
                    outPath = std::filesystem::absolute(it.path());
                    return true;
                }
            }
        } catch (std::exception const& /* ex */) {
            // Ignore file system errors, simply, folder not found
            return false;
        }
        return false;
    }

    bool FindSubFolderInParents(std::string const& path, std::string const& folderName, int maxParentCount, std::filesystem::path& outPath)
    {
        auto itPath = std::filesystem::absolute(path);
        while (maxParentCount > 0) {
            // Try to find an specific folder
            if (FindSubFolderRecursive(itPath.string(), folderName, outPath)) {
                return true;
            }

            // Move up one directory
            itPath = std::filesystem::path(itPath).parent_path();
            --maxParentCount;
        }
        return false;
    }

    std::string LoadFile(std::string const& path)
    {
        // File existence
        if (!std::filesystem::exists(path)) {
            throw std::runtime_error(fmt::format("Could not find file {}", path));
        }
        // Open
        std::ifstream fs(path);
        if (!fs.is_open()) {
            throw std::runtime_error(fmt::format("Could not open file {}", path));
        }
        // Read
        return std::string(std::istreambuf_iterator<char>(fs), {});
    }
}
