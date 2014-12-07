#pragma once

#include <chrono>
#include <memory>

// Common includes.
#define GLM_FORCE_RADIANS
//#define GLM_MESSAGES
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <json/json.h>

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::weak_ptr;
using std::dynamic_pointer_cast;
using std::static_pointer_cast;

namespace df3d {

DF3D_DLL std::string glmVecToString(const glm::vec3 &v);

inline void SafeNormalize(glm::vec3 &v)
{
    if (glm::fastLength(v) > 0.00001f)
        v = glm::normalize(v);
}

inline float IntervalBetween(const TimePoint &t1, const TimePoint &t2)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t2).count() / 1000.f;
}

// seconds
inline float IntervalBetweenNowAnd(const TimePoint &timepoint)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timepoint).count() / 1000.0f;
}

}
