#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

#include <assimp/scene.h>

struct BoneChannel
{
    std::string nodeName;

    std::vector<aiVectorKey> positionKeys;
    std::vector<aiQuatKey> rotationKeys;
    std::vector<aiVectorKey> scaleKeys;
};

class AnimationClip
{
public:
    AnimationClip() = default;

    explicit AnimationClip(const aiAnimation* animation)
    {
        Load(animation);
    }

    void Load(const aiAnimation* animation)
    {
        name = animation->mName.C_Str();

        duration = animation->mDuration;

        ticksPerSecond =
            animation->mTicksPerSecond > 0.0
            ? animation->mTicksPerSecond
            : 25.0;

        channels.clear();

        for (unsigned int i = 0;
             i < animation->mNumChannels;
             ++i)
        {
            const aiNodeAnim* source =
                animation->mChannels[i];

            BoneChannel channel;

            channel.nodeName =
                source->mNodeName.C_Str();

            if (source->mNumPositionKeys > 0)
            {
                channel.positionKeys.assign(
                    source->mPositionKeys,
                    source->mPositionKeys + source->mNumPositionKeys
                );
            }

            if (source->mNumRotationKeys > 0)
            {
                channel.rotationKeys.assign(
                    source->mRotationKeys,
                    source->mRotationKeys + source->mNumRotationKeys
                );
            }

            if (source->mNumScalingKeys > 0)
            {
                channel.scaleKeys.assign(
                    source->mScalingKeys,
                    source->mScalingKeys + source->mNumScalingKeys
                );
            }

            channels[channel.nodeName] =
                std::move(channel);
        }
    }

    const BoneChannel* FindChannel(
        const std::string& nodeName
    ) const
    {
        auto it = channels.find(nodeName);

        if (it == channels.end())
            return nullptr;

        return &it->second;
    }

    double GetDurationSeconds() const
    {
        return ticksPerSecond > 0.0
            ? duration / ticksPerSecond
            : 0.0;
    }

public:
    std::string name;

    double duration = 0.0;
    double ticksPerSecond = 25.0;

    std::unordered_map<std::string, BoneChannel> channels;
};
