#pragma once

#include "RenderTarget.h"

namespace df3d {

class RenderTargetScreen : public RenderTarget
{
public:
    RenderTargetScreen(const Viewport &vp);

    void bind() override;
    void unbind() override;
};

}
