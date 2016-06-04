#include <libdf3d/io/FileSystemHelpers.h>

#include "FileDataSourceAndroid.h"

namespace df3d {

bool FileSystemHelpers::isPathAbsolute(const std::string &path)
{
    if (path.empty())
        return false;

    return path[0] == '/';
}

bool FileSystemHelpers::pathExists(const std::string &path)
{
    if (path.empty())
        return false;

    bool retRes = false;

    if (isPathAbsolute(path))
    {
        auto file = fopen(path.c_str(), "r");
        if (file)
        {
            retRes = true;
            fclose(file);
        }
    }
    else
    {
        auto aassetManager = platform_impl::FileDataSourceAndroid::getAAssetManager();
        auto file = AAssetManager_open(aassetManager, path.c_str(), AASSET_MODE_UNKNOWN);
        if (file)
        {
            retRes = true;
            AAsset_close(file);
        }
    }

    return retRes;
}

shared_ptr<FileDataSource> FileSystemHelpers::openFile(const std::string &path)
{
    if (isPathAbsolute(path))
    {
        // TODO:
        // Return desktop version.
        assert(false);
        return nullptr;
    }
    else
    {
        auto result = make_shared<platform_impl::FileDataSourceAndroid>(path.c_str());
        if (!result->valid())
            return nullptr;
        return result;
    }
}

}
