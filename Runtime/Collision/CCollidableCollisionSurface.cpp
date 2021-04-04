#include "Runtime/Collision/CCollidableCollisionSurface.hpp"

namespace urde {
constexpr CCollisionPrimitive::Type sTypeCCollidableCollisionSurface(CCollidableCollisionSurface::SetStaticTableIndex,
                                          "CCollidableCollisionSurface");

const CCollisionPrimitive::Type& CCollidableCollisionSurface::GetType() { return sTypeCCollidableCollisionSurface; }

void CCollidableCollisionSurface::SetStaticTableIndex(u32 index) { sTableIndex = index; }
} // namespace urde
