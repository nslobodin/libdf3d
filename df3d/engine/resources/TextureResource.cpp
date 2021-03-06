#include "TextureResource.h"

#include "loaders/TextureLoader_stbi.h"
#include "loaders/TextureLoader_webp.h"
#include "loaders/TextureLoader_ktx.h"
#include "ResourceFileSystem.h"
#include <df3d/engine/EngineController.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFileSystem.h>
#include <df3d/lib/JsonUtils.h>
#include <df3d/lib/os/PlatformFile.h>

namespace df3d {

namespace {

df3d::TextureResourceData* LoadTextureDataFromFile(const char *path, Allocator &allocator, bool forceRgba)
{
    auto &fs = svc().resourceManager().getFS();

    auto textureSource = fs.open(path);
    if (!textureSource)
        return nullptr;

    df3d::TextureResourceData *result = nullptr;
    if (FileSystemHelpers::compareExtension(path, ".webp"))
        result = TextureLoader_webp(*textureSource, allocator, forceRgba);
    else if (FileSystemHelpers::compareExtension(path, ".ktx"))
        result = TextureLoader_ktx(*textureSource, allocator);
    else
        result = TextureLoader_stbi(*textureSource, allocator, forceRgba);

    fs.close(textureSource);

    return result;
}

uint32_t GetTextureFlags(const Json::Value &root)
{
    uint32_t retRes = 0;

    if (root.isMember("filtering"))
    {
        auto valueStr = Id(root["filtering"].asCString());
        if (valueStr == Id("NEAREST"))
            retRes |= TEXTURE_FILTERING_NEAREST;
        else if (valueStr == Id("BILINEAR"))
            retRes |= TEXTURE_FILTERING_BILINEAR;
        else if (valueStr == Id("TRILINEAR"))
            retRes |= TEXTURE_FILTERING_TRILINEAR;
        else if (valueStr == Id("ANISOTROPIC"))
            retRes |= TEXTURE_FILTERING_ANISOTROPIC;
        else
            DFLOG_WARN("Unknown filtering mode %s", root["filtering"].asCString());
    }
    else
    {
        retRes |= TEXTURE_FILTERING_ANISOTROPIC;
    }

    if (root.isMember("wrap_mode"))
    {
        auto valueStr = Id(root["wrap_mode"].asCString());
        if (valueStr == Id("WRAP"))
            retRes |= TEXTURE_WRAP_MODE_REPEAT;
        else if (valueStr == Id("CLAMP"))
            retRes |= TEXTURE_WRAP_MODE_CLAMP;
        else
            DFLOG_WARN("Unknown wrap_mode mode %s", root["wrap_mode"].asCString());
    }
    else
    {
        retRes |= TEXTURE_WRAP_MODE_REPEAT;
    }

    return retRes;
}

}

bool TextureHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return false;

    if (!root.isMember("path"))
        return false;

    m_flags = GetTextureFlags(root);

    std::string path = root["path"].asString();
    if (df3d::svc().resourceManager().getIsLowEndDevice())
    {
        auto extension = path.substr(path.find_last_of('.'));
        auto lqPath = path.substr(0, path.find_last_of('.'));
        lqPath += "_lq" + extension;

        if (PlatformFileExists(lqPath.c_str()))
            path = lqPath;
    }

    bool forceRGBA = svc().renderManager().getBackendID() == RenderBackendID::METAL;

    m_resourceData = LoadTextureDataFromFile(path.c_str(), allocator, forceRGBA);

    return m_resourceData != nullptr;
}

void TextureHolder::decodeCleanup(Allocator &allocator)
{
    MAKE_DELETE(allocator, m_resourceData);
    m_resourceData = nullptr;
}

bool TextureHolder::createResource(Allocator &allocator)
{
    TextureHandle handle = svc().renderManager().getBackend().createTexture(*m_resourceData, m_flags);
    if (!handle.isValid())
        return false;

    m_resource = MAKE_NEW(allocator, TextureResource)();
    m_resource->handle = handle;
    m_resource->width = m_resourceData->mipLevels[0].width;
    m_resource->height = m_resourceData->mipLevels[0].height;
    return true;
}

void TextureHolder::destroyResource(Allocator &allocator)
{
    DF3D_ASSERT(m_resource->handle.isValid());
    svc().renderManager().getBackend().destroyTexture(m_resource->handle);
    MAKE_DELETE(allocator, m_resource);
    m_resource = nullptr;
}

TextureResourceData* LoadTexture_Workaround(ResourceDataSource &dataSource, Allocator &alloc)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return nullptr;

    if (!root.isMember("path"))
        return nullptr;

    return LoadTextureDataFromFile(root["path"].asCString(), alloc, true);
}

}
