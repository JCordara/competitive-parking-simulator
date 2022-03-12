#ifndef C_STD_H
#define C_STD_H

/* STL */
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <math.h>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <any>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <PhysX/PxPhysicsAPI.h>

#include "Random.h"

#include "Log.h"

using std::string;                           // Strings
using std::unordered_map, std::pair;         // Mapping
using std::shared_ptr,    std::make_shared;  // Shared pointers
using std::unique_ptr,    std::make_unique;  // Unique pointers
using std::enable_if_t,   std::is_base_of_v; // Template param type assertion
using std::dynamic_pointer_cast;             // Downcast smart pointers
using std::vector;


namespace conversion {
    /** Convert glm matrix to PhysX matrix
     @param m The glm matrix
    @return Converted PhysX matrix
    */
    inline physx::PxMat44 toPxMat44(const glm::mat4& m) {

        physx::PxMat44 px;

        px[0][0] = m[0][0];
        px[0][1] = m[0][1];
        px[0][2] = m[0][2];
        px[0][3] = m[0][3];

        px[1][0] = m[1][0];
        px[1][1] = m[1][1];
        px[1][2] = m[1][2];
        px[1][3] = m[1][3];

        px[2][0] = m[2][0];
        px[2][1] = m[2][1];
        px[2][2] = m[2][2];
        px[2][3] = m[2][3];

        px[3][0] = m[3][0];
        px[3][1] = m[3][1];
        px[3][2] = m[3][2];
        px[3][3] = m[3][3];

        return px;
    }

    /** Convert PhysX matrix to glm matrix
     @param px The PhysX matrix
    @return Converted glm matrix
    */
    inline glm::mat4 toMat4(const physx::PxMat44& px)
    {
        glm::mat4 m;

        m[0][0] = px[0][0];
        m[0][1] = px[0][1];
        m[0][2] = px[0][2];
        m[0][3] = px[0][3];

        m[1][0] = px[1][0];
        m[1][1] = px[1][1];
        m[1][2] = px[1][2];
        m[1][3] = px[1][3];

        m[2][0] = px[2][0];
        m[2][1] = px[2][1];
        m[2][2] = px[2][2];
        m[2][3] = px[2][3];

        m[3][0] = px[3][0];
        m[3][1] = px[3][1];
        m[3][2] = px[3][2];
        m[3][3] = px[3][3];

        return m;
    }

    /** Convert PhysX vector to glm vector
     @param pv The PhysX vector
    @return Converted glm vector
    */
    inline glm::vec3 toVec3(const physx::PxVec3& pv)
    {
        return glm::vec3(pv.x, pv.y, pv.z);
    }

    /** Convert PhysX vector to glm vector
     @param pv The PhysX vector
    @return Converted glm vector
    */
    inline glm::vec4 toVec4(const physx::PxVec4& pv)
    {
        return glm::vec4(pv.x, pv.y, pv.z, pv.w);
    }

    /** Convert glm vector to PhysX vector
     @param gv The glm vector
    @return Converted PhysX vector
    */
    inline physx::PxVec3 toPxVec3(const glm::vec3& gv)
    {
        return physx::PxVec3(gv.x, gv.y, gv.z);
    }

    /** Convert glm vector to PhysX vector
     @param gv The glm vector
    @return Converted PhysX vector
    */
    inline physx::PxVec4 toPxVec4(const glm::vec4& gv)
    {
        return physx::PxVec4(gv.x, gv.y, gv.z, gv.w);
    }

    /** Convert glm matrix to PhysX PxTransform
     @param m The glm matrix
    @return Converted PxTransform object
    */
    inline physx::PxTransform toPxTransform(const glm::mat4& m) {

        physx::PxMat44 px;
        
        px[0][0] = m[0][0];
        px[0][1] = m[0][1];
        px[0][2] = m[0][2];
        px[0][3] = m[0][3];

        px[1][0] = m[1][0];
        px[1][1] = m[1][1];
        px[1][2] = m[1][2];
        px[1][3] = m[1][3];

        px[2][0] = m[2][0];
        px[2][1] = m[2][1];
        px[2][2] = m[2][2];
        px[2][3] = m[2][3];

        px[3][0] = m[3][0];
        px[3][1] = m[3][1];
        px[3][2] = m[3][2];
        px[3][3] = m[3][3];

        return physx::PxTransform(px);
    }

    /** Convert PhysX PxTransform object to glm matrix
     @param t The PhysX PxTransform object
    @return Converted glm matrix
    */
    inline glm::mat4 toMat4(const physx::PxTransform& t) {

        physx::PxMat44 px(t);
        glm::mat4 m;

        m[0][0] = px[0][0];
        m[0][1] = px[0][1];
        m[0][2] = px[0][2];
        m[0][3] = px[0][3];

        m[1][0] = px[1][0];
        m[1][1] = px[1][1];
        m[1][2] = px[1][2];
        m[1][3] = px[1][3];

        m[2][0] = px[2][0];
        m[2][1] = px[2][1];
        m[2][2] = px[2][2];
        m[2][3] = px[2][3];

        m[3][0] = px[3][0];
        m[3][1] = px[3][1];
        m[3][2] = px[3][2];
        m[3][3] = px[3][3];

        return m;
    }

    /** Convert PhysX quaternion to glm matrix
     @param q The PhysX quaternion
    @return Converted glm matrix
    */
    inline glm::mat4 toMat4(const physx::PxQuat& q) {

        physx::PxMat44 px(q);
        glm::mat4 m;

        m[0][0] = px[0][0];
        m[0][1] = px[0][1];
        m[0][2] = px[0][2];
        m[0][3] = px[0][3];

        m[1][0] = px[1][0];
        m[1][1] = px[1][1];
        m[1][2] = px[1][2];
        m[1][3] = px[1][3];

        m[2][0] = px[2][0];
        m[2][1] = px[2][1];
        m[2][2] = px[2][2];
        m[2][3] = px[2][3];

        m[3][0] = px[3][0];
        m[3][1] = px[3][1];
        m[3][2] = px[3][2];
        m[3][3] = px[3][3];

        return m;
    }
};


template<typename T> inline
auto convert(const glm::mat4& m) -> T {}

template<typename T> inline
auto convert(const physx::PxMat44& px) -> T {}

template<typename T> inline
auto convert(const glm::vec3& gv) -> T {}

template<typename T> inline
auto convert(const glm::vec4& gv) -> T {}

template<typename T> inline
auto convert(const physx::PxVec3& pv) -> T {}

template<typename T> inline
auto convert(const physx::PxVec4& pv) -> T {}

template<typename T> inline
auto convert(const physx::PxQuat& q) -> T {}

template<typename T> inline
auto convert(const physx::PxTransform& t) -> T {}


template<> inline
auto convert<physx::PxMat44>(const glm::mat4& m) -> physx::PxMat44 {
    return conversion::toPxMat44(m);
}

template<> inline
auto convert<physx::PxTransform>(const glm::mat4& m) -> physx::PxTransform {
    return conversion::toPxTransform(m);
}

template<> inline
auto convert<glm::mat4>(const physx::PxMat44& px) -> glm::mat4 {
    return conversion::toMat4(px);
}

template<> inline
auto convert<glm::vec3>(const physx::PxVec3& pv) -> glm::vec3 {
    return conversion::toVec3(pv);
}

template<> inline
auto convert<glm::vec4>(const physx::PxVec4& pv) -> glm::vec4 {
    return conversion::toVec4(pv);
}

template<> inline
auto convert<physx::PxVec3>(const glm::vec3& gv) -> physx::PxVec3 {
    return conversion::toPxVec3(gv);
}

template<> inline
auto convert<physx::PxVec4>(const glm::vec4& gv) -> physx::PxVec4 {
    return conversion::toPxVec4(gv);
}

template<> inline
auto convert<glm::mat4>(const physx::PxQuat& q) -> glm::mat4 {
    return conversion::toMat4(q);
}

template<> inline
auto convert<glm::mat4>(const physx::PxTransform& t) -> glm::mat4 {
    return conversion::toMat4(t);
}




#endif // C_STD_H
