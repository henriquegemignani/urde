#ifndef _DNAMP1_GRAPPLEPOINT_HPP_
#define _DNAMP1_GRAPPLEPOINT_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec::DNAMP1
{
struct GrapplePoint : IScriptObject
{
    DECL_YAML
    String<-1> name;
    Value<atVec3f> location;
    Value<atVec3f> orientation;
    Value<bool> active;
    GrappleParameters grappleParameters;
};
}

#endif
