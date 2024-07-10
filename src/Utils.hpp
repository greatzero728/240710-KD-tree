#ifndef UTILS_HPP
#define UTILS_HPP

#include "Logging.hpp"

#include <filesystem>
#include <string>
#include <vector>



namespace CS350 {
    /**
     * @brief
     *  Changes the working directory until the folder is found. By going up in the directory hierarchy
     */
    void ChangeWorkdir(char const* folder = "bin");

    /**
     * @brief
     *  Checks if a given path contains a folder in any of it's sub-folders, recursively.
     * @param path
     *  The starting path from which to search (e.g. "../out")
     * @param folderName
     *  The base folder name to find (e.g. "bin"). (case sensitive)
     * @param outPath
     *  If the folder is found, the path of the folder will be saved in this variable. Path is absolute (e.g. "C:/out/test/mydata/bin")
     * @return
     *  Whether the folder is found or not
     */
    bool FindSubFolderRecursive(std::string const& path, std::string const& folderName, std::filesystem::path& outPath);

    /**
     * @brief
     *  Tries to find an specific sub-folder, if not found, it will iterate once to the parent folder and search again
     * @param path
     *  The starting path from which to search
     * @param folderName
     *  The folder to find
     * @param maxParentCount
     *  The maximum amount of times to iterate to the parent
     * @param outPath
     *  If the folder is found, the path of the folder will be saved in this variable. Path is absolute
     * @return
     *  Whether the folder is found or not
     */
    bool FindSubFolderInParents(std::string const& path, std::string const& folderName, int maxParentCount, std::filesystem::path& outPath);    

    /**
     * @brief 
     *  Given a file path, opens all it's content in text format. Throws an exception if not found or not able to open.
     */
    std::string LoadFile(std::string const& path);
}

#endif // UTILS_HPP
