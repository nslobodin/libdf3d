#include "df3d_pch.h"
#include "ResourceFactory.h"

#include "ResourceManager.h"
#include <render/Texture2D.h>
#include <render/GpuProgram.h>

namespace df3d { namespace resources {

const char * const SIMPLE_LIGHTING_PROGRAM_EMBED_PATH = "__embed_simple_lighting_program";
const char * const RTT_QUAD_PROGRAM_EMBED_PATH = "__embed_quad_render_program";
const char * const COLORED_PROGRAM_EMBED_PATH = "__embed_colored_program";
const char * const AMBIENT_PASS_PROGRAM_EMBED_PATH = "__embed_ambient_pass_program";

// TODO_REFACTO

ResourceFactory::ResourceFactory(ResourceManager *holder)
    : m_holder(holder)
{

}

ResourceFactory::~ResourceFactory()
{

}

shared_ptr<audio::AudioBuffer> ResourceFactory::createAudioBuffer(const std::string &audioPath)
{
    return nullptr;
    //return static_pointer_cast<audio::AudioBuffer>(loadResourceFromFileSystem(audioPath, ResourceLoadingMode::IMMEDIATE));
}

shared_ptr<render::GpuProgram> ResourceFactory::createGpuProgram(const std::string &vertexShader, const std::string &fragmentShader)
{
    return nullptr;
    /*
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    // FIXME:
    // Use full path.
    std::string gpuProgramId = vertexShader + ";" + fragmentShader;
    if (auto alreadyLoaded = findResource(gpuProgramId))
        return static_pointer_cast<render::GpuProgram>(alreadyLoaded);

    auto program = make_shared<render::GpuProgram>();
    auto vShader = render::Shader::createFromFile(vertexShader);
    auto fShader = render::Shader::createFromFile(fragmentShader);

    if (!vShader || !fShader)
    {
        base::glog << "Can not create gpu program. Either vertex or fragment shader is invalid" << base::logwarn;
        return nullptr;
    }

    program->attachShader(vShader);
    program->attachShader(fShader);

    program->setGUID(gpuProgramId);
    program->setInitialized();
    appendResource(program);

    return program;
    */
}

shared_ptr<render::GpuProgram> ResourceFactory::createGpuProgram(const std::string &guid, const std::string &vertexData, const std::string &fragmentData)
{
    // FIXME: maybe check in cache?

    render::GpuProgramManualLoader loader(guid, vertexData, fragmentData);

    return static_pointer_cast<render::GpuProgram>(m_holder->loadManual(&loader));
}

shared_ptr<render::GpuProgram> ResourceFactory::createSimpleLightingGpuProgram()
{
    return static_pointer_cast<render::GpuProgram>(m_holder->findResource(SIMPLE_LIGHTING_PROGRAM_EMBED_PATH));
}

shared_ptr<render::GpuProgram> ResourceFactory::createColoredGpuProgram()
{
    return static_pointer_cast<render::GpuProgram>(m_holder->findResource(COLORED_PROGRAM_EMBED_PATH));
}

shared_ptr<render::GpuProgram> ResourceFactory::createRttQuadProgram()
{
    return static_pointer_cast<render::GpuProgram>(m_holder->findResource(RTT_QUAD_PROGRAM_EMBED_PATH));
}

shared_ptr<render::GpuProgram> ResourceFactory::createAmbientPassProgram()
{
    return static_pointer_cast<render::GpuProgram>(m_holder->findResource(AMBIENT_PASS_PROGRAM_EMBED_PATH));
}

shared_ptr<render::MeshData> ResourceFactory::createMeshData(const std::string &meshDataPath, ResourceLoadingMode lm)
{
    return nullptr;
    //return static_pointer_cast<render::MeshData>(loadResourceFromFileSystem(meshDataPath, lm));
}

shared_ptr<render::Texture2D> ResourceFactory::createTexture(const std::string &imagePath, ResourceLoadingMode lm)
{
    return createTexture(imagePath, render::TextureCreationParams(), lm);
}

shared_ptr<render::Texture2D> ResourceFactory::createTexture(const std::string &imagePath, render::TextureCreationParams params, ResourceLoadingMode lm)
{
    return nullptr;
}

shared_ptr<render::Texture2D> ResourceFactory::createTexture(unique_ptr<render::PixelBuffer> pixelBuffer, render::TextureCreationParams params)
{
    render::Texture2DManualLoader loader(std::move(pixelBuffer), params);

    return static_pointer_cast<render::Texture2D>(m_holder->loadManual(&loader));
}
/*
shared_ptr<render::Texture2D> ResourceFactory::createTexture(const std::string &imagePath, ResourceLoadingMode lm)
{
    return nullptr;
    //return static_pointer_cast<render::Texture2D>(loadResourceFromFileSystem(imagePath, lm));
}

shared_ptr<render::Texture2D> ResourceFactory::createEmptyTexture(const std::string &id)
{
    return nullptr;
    /*
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    if (!id.empty())
    {
        if (auto alreadyLoaded = findResource(id))
            return static_pointer_cast<render::Texture2D>(alreadyLoaded);
    }

    auto texture = make_shared<render::Texture2D>();
    texture->setInitialized();
    if (!id.empty())
        texture->setGUID(id);
    appendResource(texture);

    return texture;
    *
}
*/
shared_ptr<render::TextureCube> ResourceFactory::createCubeTexture(const std::string &positiveXImage, const std::string &negativeXImage,
                                                                   const std::string &positiveYImage, const std::string &negativeYImage,
                                                                   const std::string &positiveZImage, const std::string &negativeZImage,
                                                                   ResourceLoadingMode lm)
{
    return nullptr;
    /*
    auto positiveX = createTexture(positiveXImage, lm);
    auto negativeX = createTexture(negativeXImage, lm);
    auto positiveY = createTexture(positiveYImage, lm);
    auto negativeY = createTexture(negativeYImage, lm);
    auto positiveZ = createTexture(positiveZImage, lm);
    auto negativeZ = createTexture(negativeZImage, lm);

    auto textureCube = make_shared<render::TextureCube>(positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ);
    textureCube->setInitialized();
    textureCube->setGUID(positiveX->getGUID() + ";" + negativeX->getGUID() + ";" +
                         positiveY->getGUID() + ";" + negativeY->getGUID() + ";" +
                         positiveZ->getGUID() + ";" + negativeZ->getGUID() + ";");

    appendResource(textureCube);

    return textureCube;
    */
}

shared_ptr<render::MaterialLib> ResourceFactory::createMaterialLib(const std::string &mtlLibPath)
{
    return nullptr;
    //return static_pointer_cast<render::MaterialLib>(loadResourceFromFileSystem(mtlLibPath, ResourceLoadingMode::IMMEDIATE));
}


} }
