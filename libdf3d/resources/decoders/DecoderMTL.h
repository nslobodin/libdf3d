#pragma once

#include "../ResourceDecoder.h"

FWD_MODULE_CLASS(render, Material)
FWD_MODULE_CLASS(render, Technique)
FWD_MODULE_CLASS(render, RenderPass)
FWD_MODULE_CLASS(render, GpuProgram)
FWD_MODULE_CLASS(render, Texture)

namespace df3d { namespace resources {

struct MaterialLibNode;

class DecoderMTL : public ResourceDecoder
{
    shared_ptr<render::Material> parseMaterialNode(const MaterialLibNode &node);
    shared_ptr<render::Technique> parseTechniqueNode(const MaterialLibNode &node);
    shared_ptr<render::RenderPass> parsePassNode(const MaterialLibNode &node);
    shared_ptr<render::GpuProgram> parseShaderNode(const MaterialLibNode &node);
    void parseShaderParamsNode(MaterialLibNode &node, shared_ptr<render::RenderPass> pass);
    shared_ptr<render::Texture> parseSamplerNode(const MaterialLibNode &node);

    // For logging.
    std::string m_libPath;

public:
    DecoderMTL();
    ~DecoderMTL();

    shared_ptr<Resource> createResource() override;
    bool decodeResource(shared_ptr<FileDataSource> file, shared_ptr<Resource> resource) override;
};

} }