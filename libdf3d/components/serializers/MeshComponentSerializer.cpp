#include "df3d_pch.h"
#include "MeshComponentSerializer.h"

#include <components/MeshComponent.h>
#include <utils/JsonHelpers.h>

namespace df3d { namespace components { namespace serializers {

shared_ptr<NodeComponent> MeshComponentSerializer::fromJson(const Json::Value &root)
{
    return make_shared<MeshComponent>(root["path"].asCString());
}

Json::Value MeshComponentSerializer::toJson(shared_ptr<const NodeComponent> component)
{
    Json::Value result;

    auto comp = static_cast<const MeshComponent*>(component.get());
    result["path"] = "XZ";  // FIXME:

    return result;
}

} } }