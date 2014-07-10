#pragma once

FWD_MODULE_CLASS(base, Controller)

namespace df3d { namespace resources {

// TODO:
// Work on Android

class FileDataSource;

class DF3D_DLL FileSystem : boost::noncopyable
{
    friend class base::Controller;

    FileSystem();
    ~FileSystem();

    mutable std::recursive_mutex m_lock;
    std::vector<std::string> m_searchPaths;

    mutable std::unordered_map<std::string, std::string> m_fullPathsCache;

    static std::string canonicalPath(const std::string &rawPath);
    static bool pathExists(const std::string &path);

public:
    shared_ptr<FileDataSource> openFile(const char *filePath);

    void addSearchPath(const char *path);

    std::string fullPath(const char *path) const;

    static std::string getFileDirectory(const std::string &filePath);
    static std::string pathConcatenate(const std::string &fp1, const std::string &fp2);
    static std::string getFileExtension(const std::string &rawPath);
    static std::string getFilename(const std::string &filePath);
    static std::string getFilenameWithoutExtension(const std::string &filePath);
};

} }