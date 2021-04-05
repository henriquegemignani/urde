#pragma once

#include <optional>

#include "Runtime/GCNTypes.hpp"
#include "Runtime/Graphics/Shaders/CTexturedQuadFilter.hpp"

#include <boo/graphicsdev/IGraphicsDataFactory.hpp>

#include <zeus/CColor.hpp>
#include <zeus/CMatrix4f.hpp>

namespace urde {

class CWorldShadowShader {
  static boo::ObjToken<boo::IShaderPipeline> s_Pipeline;
  static boo::ObjToken<boo::IShaderPipeline> s_ZPipeline;
  boo::ObjToken<boo::ITextureR> m_tex;
  std::optional<CTexturedQuadFilter> m_prevQuad;
  u32 m_w, m_h;

  struct Uniform {
    zeus::CMatrix4f m_matrix;
    zeus::CColor m_color;
  };
  boo::ObjToken<boo::IGraphicsBufferD> m_vbo;
  boo::ObjToken<boo::IGraphicsBufferD> m_uniBuf;
  boo::ObjToken<boo::IShaderDataBinding> m_dataBind;
  boo::ObjToken<boo::IShaderDataBinding> m_zDataBind;
  Uniform m_uniform;

public:
  static void Initialize();
  static void Shutdown();
  explicit CWorldShadowShader(u32 w, u32 h);
  void bindRenderTarget();
  void drawBase(float extent);
  void lightenShadow();
  void blendPreviousShadow();
  void resolveTexture();

  u32 GetWidth() const { return m_w; }
  u32 GetHeight() const { return m_h; }

  const boo::ObjToken<boo::ITextureR>& GetTexture() const { return m_tex; }
};

} // namespace urde
