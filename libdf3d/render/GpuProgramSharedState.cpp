#include "GpuProgramSharedState.h"

#include "GpuProgram.h"
#include "IRenderBackend.h"
#include "RenderManager.h"
#include <libdf3d/base/EngineController.h>
#include <libdf3d/base/TimeManager.h>
#include <libdf3d/3d/Camera.h>
#include <libdf3d/game/World.h>

namespace df3d {

void GpuProgramSharedState::resetFlags()
{
    m_worldViewProjDirty = m_worldViewDirty = m_worldView3x3Dirty
        = m_normalDirty = m_viewInverseDirty = m_worldInverseDirty = true;
}

const glm::mat4& GpuProgramSharedState::getWorldMatrix()
{
    return m_worldMatrix;
}

const glm::mat4& GpuProgramSharedState::getViewMatrix()
{
    return m_viewMatrix;
}

const glm::mat4& GpuProgramSharedState::getProjectionMatrix()
{
    return m_projMatrix;
}

const glm::mat4& GpuProgramSharedState::getWorldViewProjectionMatrix()
{
    if (m_worldViewProjDirty)
    {
        m_worldViewProj = m_projMatrix * getWorldViewMatrix();
        m_worldViewProjDirty = false;
    }

    return m_worldViewProj;
}

const glm::mat4& GpuProgramSharedState::getWorldViewMatrix()
{
    if (m_worldViewDirty)
    {
        m_worldView = m_viewMatrix * m_worldMatrix;
        m_worldViewDirty = false;
    }

    return m_worldView;
}

const glm::mat3& GpuProgramSharedState::getWorldView3x3Matrix()
{
    if (m_worldView3x3Dirty)
    {
        m_worldView3x3 = glm::mat3(getWorldViewMatrix());
        m_worldView3x3Dirty = false;
    }

    return m_worldView3x3;
}

const glm::mat3& GpuProgramSharedState::getNormalMatrix()
{
    if (m_normalDirty)
    {
        m_normalMatrix = glm::inverseTranspose(getWorldView3x3Matrix());
        m_normalDirty = false;
    }

    return m_normalMatrix;
}

const glm::mat4& GpuProgramSharedState::getViewMatrixInverse()
{
    if (m_viewInverseDirty)
    {
        m_viewMatrixInverse = glm::inverse(m_viewMatrix);
        m_viewInverseDirty = false;
    }

    return m_viewMatrixInverse;
}

const glm::mat4& GpuProgramSharedState::getWorldMatrixInverse()
{
    if (m_worldInverseDirty)
    {
        m_worldMatrixInverse = glm::inverse(m_worldMatrix);
        m_worldInverseDirty = false;
    }

    return m_worldMatrixInverse;
}

void GpuProgramSharedState::setWorldMatrix(const glm::mat4 &worldm)
{
    m_worldMatrix = worldm;
    resetFlags();
}

void GpuProgramSharedState::setViewMatrix(const glm::mat4 &viewm)
{
    m_viewMatrix = viewm;
    resetFlags();
}

void GpuProgramSharedState::setProjectionMatrix(const glm::mat4 &projm)
{
    m_projMatrix = projm;
    // FIXME:
    // Not all flags have to be set. Whatever.
    resetFlags();
}

void GpuProgramSharedState::setViewPort(const Viewport &viewport)
{
    m_pixelSize = glm::vec2(1.0f / (float)viewport.width(), 1.0f / (float)viewport.height());
}

void GpuProgramSharedState::setFog(float density, const glm::vec3 &color)
{
    m_fogDensity = density;
    m_fogColor = color;
}

void GpuProgramSharedState::setAmbientColor(const glm::vec3 &color)
{
    m_globalAmbient = glm::vec4(color, 1.0f);
}

void GpuProgramSharedState::setLight(const Light &light)
{
    // Update light params.
    m_currentLight.diffuseParam = light.getDiffuseColor();
    m_currentLight.specularParam = light.getSpecularColor();
    m_currentLight.k0Param = light.getConstantAttenuation();
    m_currentLight.k1Param = light.getLinearAttenuation();
    m_currentLight.k2Param = light.getQuadraticAttenuation();

    // Since we calculate lighting in the view space we should translate position and direction.
    if (light.getType() == Light::Type::DIRECTIONAL)
    {
        auto dir = light.getDirection();
        m_currentLight.positionParam = getViewMatrix() * glm::vec4(dir, 0.0f);
    }
    else if (light.getType() == Light::Type::OMNI)
    {
        DF3D_ASSERT(false, "unsupported"); // TODO:
        auto pos = light.getPosition();
        m_currentLight.positionParam = getViewMatrix() * glm::vec4(pos, 1.0f);
    }
}

void GpuProgramSharedState::clear()
{
    resetFlags();

    m_worldMatrix = glm::mat4(1.0f);
    m_viewMatrix = glm::mat4(1.0f);
    m_projMatrix = glm::mat4(1.0f);

    m_cameraPosition = svc().world().getCamera()->getPosition();

    m_engineElapsedTime = svc().timer().getElapsedTime();
}

void GpuProgramSharedState::updateSharedUniforms(const GpuProgram &program)
{
    const auto &sharedUniforms = program.getSharedUniforms();

    for (const auto &sharedUni : sharedUniforms)
    {
        const void *data = nullptr;

        switch (sharedUni.type)
        {
        case SharedUniformType::WORLD_VIEW_PROJECTION_MATRIX_UNIFORM:
            data = glm::value_ptr(getWorldViewProjectionMatrix());
            break;
        case SharedUniformType::WORLD_VIEW_MATRIX_UNIFORM:
            data = glm::value_ptr(getWorldViewMatrix());
            break;
        case SharedUniformType::WORLD_VIEW_3X3_MATRIX_UNIFORM:
            data = glm::value_ptr(getWorldView3x3Matrix());
            break;
        case SharedUniformType::VIEW_INVERSE_MATRIX_UNIFORM:
            data = glm::value_ptr(getViewMatrixInverse());
            break;
        case SharedUniformType::VIEW_MATRIX_UNIFORM:
            data = glm::value_ptr(getViewMatrix());
            break;
        case SharedUniformType::WORLD_INVERSE_MATRIX_UNIFORM:
            data = glm::value_ptr(getWorldMatrixInverse());
            break;
        case SharedUniformType::WORLD_MATRIX_UNIFORM:
            data = glm::value_ptr(getWorldMatrix());
            break;
        case SharedUniformType::PROJECTION_MATRIX_UNIFORM:
            data = glm::value_ptr(getProjectionMatrix());
            break;
        case SharedUniformType::NORMAL_MATRIX_UNIFORM:
            data = glm::value_ptr(getNormalMatrix());
            break;
        case SharedUniformType::CAMERA_POSITION_UNIFORM:
            data = glm::value_ptr(m_cameraPosition);
            break;
        case SharedUniformType::GLOBAL_AMBIENT_UNIFORM:
            data = glm::value_ptr(m_globalAmbient);
            break;
        case SharedUniformType::FOG_DENSITY_UNIFORM:
            data = &m_fogDensity;
            break;
        case SharedUniformType::FOG_COLOR_UNIFORM:
            data = glm::value_ptr(m_fogColor);
            break;
        case SharedUniformType::PIXEL_SIZE_UNIFORM:
            data = glm::value_ptr(m_pixelSize);
            break;
        case SharedUniformType::ELAPSED_TIME_UNIFORM:
            data = &m_engineElapsedTime;
            break;
        case SharedUniformType::SCENE_LIGHT_DIFFUSE_UNIFORM:
            data = glm::value_ptr(m_currentLight.diffuseParam);
            break;
        case SharedUniformType::SCENE_LIGHT_SPECULAR_UNIFORM:
            data = glm::value_ptr(m_currentLight.specularParam);
            break;
        case SharedUniformType::SCENE_LIGHT_POSITION_UNIFORM:
            data = glm::value_ptr(m_currentLight.positionParam);
            break;
        case SharedUniformType::SCENE_LIGHT_KC_UNIFORM:
            data = &m_currentLight.k0Param;
            break;
        case SharedUniformType::SCENE_LIGHT_KL_UNIFORM:
            data = &m_currentLight.k1Param;
            break;
        case SharedUniformType::SCENE_LIGHT_KQ_UNIFORM:
            data = &m_currentLight.k2Param;
            break;
        case SharedUniformType::COUNT:
        default:
            glog << "Can not set shared value to a not shared uniform" << logwarn;
            break;
        }

        if (data)
            svc().renderManager().getBackend().setUniformValue(sharedUni.descr, data);
    }
}

}