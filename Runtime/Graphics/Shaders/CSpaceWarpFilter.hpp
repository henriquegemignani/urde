#pragma once

#include <array>

#include "Runtime/GCNTypes.hpp"

#include <boo/graphicsdev/IGraphicsDataFactory.hpp>

#include <zeus/CMatrix4f.hpp>
#include <zeus/CVector3f.hpp>

namespace urde {

class CSpaceWarpFilter {
  static boo::ObjToken<boo::IShaderPipeline> s_Pipeline;
  struct Uniform {
    zeus::CMatrix4f m_matrix;
    zeus::CMatrix4f m_indXf;
    zeus::CVector3f m_strength;
  };
  std::array<std::array<std::array<u8, 4>, 8>, 4> m_shiftTexture{};
  boo::ObjToken<boo::ITexture> m_warpTex;
  boo::ObjToken<boo::IGraphicsBufferS> m_vbo;
  boo::ObjToken<boo::IGraphicsBufferD> m_uniBuf;
  boo::ObjToken<boo::IShaderDataBinding> m_dataBind;
  Uniform m_uniform;
  float m_strength = 1.f;

  void GenerateWarpRampTex(boo::IGraphicsDataFactory::Context& ctx);

public:
  static void Initialize();
  static void Shutdown();
  CSpaceWarpFilter();
  void setStrength(float strength) { m_strength = strength; }
  void draw(const zeus::CVector3f& pt);
};

} // namespace urde
