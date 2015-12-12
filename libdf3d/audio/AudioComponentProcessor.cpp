#include "df3d_pch.h"
#include "AudioComponentProcessor.h"

#include "AudioBuffer.h"
#include "impl/OpenALCommon.h"
#include <scene/impl/ComponentDataHolder.h>
#include <base/EngineController.h>
#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>
#include <utils/Utils.h>

namespace df3d {

static AudioComponentProcessor::State getAudioState(unsigned audioSourceId)
{
    assert(audioSourceId);

    ALint state;
    alGetSourcei(audioSourceId, AL_SOURCE_STATE, &state);

    switch (state)
    {
    case AL_PLAYING:
        return AudioComponentProcessor::State::PLAYING;
    case AL_PAUSED:
        return AudioComponentProcessor::State::PAUSED;
    case AL_STOPPED:
        return AudioComponentProcessor::State::STOPPED;
    default:
        return AudioComponentProcessor::State::INITIAL;
    }

    return  AudioComponentProcessor::State::INITIAL;
}

struct AudioComponentProcessor::Impl
{
    struct Data
    {
        unsigned audioSourceId = 0;
        float pitch = 1.0f;
        float gain = 1.0f;
        bool looped = false;
    };

    ComponentDataHolder<Data> data;
};

void AudioComponentProcessor::update(float systemDelta, float gameDelta)
{
    /*
    if (!m_audioSourceId)
        return;

    if (m_holder->transform())
    {
        auto pos = m_holder->transform()->getPosition();
        alSourcefv(m_audioSourceId, AL_POSITION, glm::value_ptr(pos));
    }

    // If it has stopped, then remove from the holder.
    if (getState() == State::STOPPED)
        m_holder->detachComponent(ComponentType::AUDIO);
    */
}

AudioComponentProcessor::AudioComponentProcessor()
    : m_pimpl(new Impl())
{

}

AudioComponentProcessor::~AudioComponentProcessor()
{

}

void AudioComponentProcessor::play(ComponentInstance comp)
{
    assert(comp.valid());

    const auto &compData = m_pimpl->data.getData(comp);

    if (getAudioState(compData.audioSourceId) != AudioComponentProcessor::State::PLAYING)
        alSourcePlay(compData.audioSourceId);
}

void AudioComponentProcessor::stop(ComponentInstance comp)
{
    assert(comp.valid());

    const auto &compData = m_pimpl->data.getData(comp);

    if (getAudioState(compData.audioSourceId) != AudioComponentProcessor::State::STOPPED)
        alSourceStop(compData.audioSourceId);
}

void AudioComponentProcessor::pause(ComponentInstance comp)
{
    assert(comp.valid());

    const auto &compData = m_pimpl->data.getData(comp);

    if (getAudioState(compData.audioSourceId) != AudioComponentProcessor::State::PAUSED)
        alSourcePause(compData.audioSourceId);
}

void AudioComponentProcessor::setPitch(ComponentInstance comp, float pitch)
{
    assert(comp.valid());

    auto &compData = m_pimpl->data.getData(comp);

    compData.pitch = pitch;
    alSourcef(compData.audioSourceId, AL_PITCH, pitch);
}

void AudioComponentProcessor::setGain(ComponentInstance comp, float gain)
{
    assert(comp.valid());

    auto &compData = m_pimpl->data.getData(comp);

    compData.gain = gain;
    alSourcef(compData.audioSourceId, AL_GAIN, gain);
}

void AudioComponentProcessor::setLooped(ComponentInstance comp, bool looped)
{
    assert(comp.valid());

    auto &compData = m_pimpl->data.getData(comp);
    compData.looped = looped;
    alSourcei(compData.audioSourceId, AL_LOOPING, looped);
}

float AudioComponentProcessor::getPitch(ComponentInstance comp) const
{
    assert(comp.valid());

    return m_pimpl->data.getData(comp).pitch;
}

float AudioComponentProcessor::getGain(ComponentInstance comp) const
{
    assert(comp.valid());

    return m_pimpl->data.getData(comp).gain;
}

bool AudioComponentProcessor::isLooped(ComponentInstance comp) const
{
    assert(comp.valid());

    return m_pimpl->data.getData(comp).looped;
}

void AudioComponentProcessor::add(Entity e, const std::string &audioFilePath)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has an audio component" << logwarn;
        return;
    }

    auto buffer = svc().resourceManager().getFactory().createAudioBuffer(audioFilePath);
    if (!buffer || !buffer->isInitialized())
    {
        glog << "Can not add audio component to an entity. Audio path:" << audioFilePath << logwarn;
        return;
    }

    Impl::Data data;

    alGenSources(1, &data.audioSourceId);
    alSourcef(data.audioSourceId, AL_PITCH, data.pitch);
    alSourcef(data.audioSourceId, AL_GAIN, data.gain);
    alSourcei(data.audioSourceId, AL_BUFFER, buffer->getALId());

    printOpenALError();
    if (!data.audioSourceId)
    {
        glog << "Failed to create audio source" << logwarn;
        return;
    }

    m_pimpl->data.add(e, data);
}

void AudioComponentProcessor::remove(Entity e)
{
    if (!m_pimpl->data.contains(e))
    {
        glog << "Failed to remove audio component from an entity. Component is not attached" << logwarn;
        return;
    }

    auto compInst = m_pimpl->data.lookup(e);
    const auto &data = m_pimpl->data.getData(compInst);
    alDeleteSources(1, &data.audioSourceId);

    m_pimpl->data.remove(e);
}

ComponentInstance AudioComponentProcessor::lookup(Entity e)
{
    return m_pimpl->data.lookup(e);
}

}
