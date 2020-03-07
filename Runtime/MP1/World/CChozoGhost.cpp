#include "Runtime/MP1/World/CChozoGhost.hpp"

#include "Runtime/GameGlobalObjects.hpp"
#include "Runtime/CStateManager.hpp"
#include "Runtime/CSimplePool.hpp"
#include "Runtime/CRandom16.hpp"
#include "Runtime/Character/CPASAnimParmData.hpp"
#include "Runtime/Weapon/CGameProjectile.hpp"
#include "Runtime/World/CPlayer.hpp"
#include "Runtime/World/CScriptCoverPoint.hpp"
#include "Runtime/World/CScriptWaypoint.hpp"
#include "Runtime/World/CTeamAiMgr.hpp"

#include "TCastTo.hpp" // Generated file, do not modify include path
namespace urde::MP1 {
CChozoGhost::CBehaveChance::CBehaveChance(CInputStream& in)
: x0_propertyCount(in.readUint32Big())
, x4_lurk(in.readFloatBig())
, x8_(in.readFloatBig())
, xc_attack(in.readFloatBig())
, x10_move(in.readFloatBig())
, x14_lurkTime(in.readFloatBig())
, x18_chargeAttack(x0_propertyCount <= 5 ? 0.5f : in.readFloatBig() * .01f)
, x1c_numBolts(x0_propertyCount <= 6 ? 2 : in.readUint32Big()) {
  float f2 = 1.f / (x10_move + xc_attack + x4_lurk + x8_);
  x4_lurk *= f2;
  x8_ *= f2;
  xc_attack *= f2;
  x10_move *= f2;
}

EBehaveType CChozoGhost::CBehaveChance::GetBehave(EBehaveType type, CStateManager& mgr) const {
  float dVar5 = x4_lurk;
  float dVar4 = x8_;
  float dVar3 = xc_attack;
  if (type == EBehaveType::Lurk) {
    float dVar2 = dVar5 / 3.f;
    dVar5 = 0.f;
    dVar4 += dVar2;
    dVar3 += dVar2;
  } else if (type == EBehaveType::One) {
    float dVar2 = dVar4 / 3.f;
    dVar4 = 0.f;
    dVar5 += dVar2;
    dVar3 += dVar2;
  } else if (type == EBehaveType::Attack) {
    float dVar2 = dVar3 / 3.f;
    dVar3 = 0.f;
    dVar5 += dVar2;
    dVar4 += dVar2;
  } else if (type == EBehaveType::Move) {
    float dVar2 = x10_move / 3.f;
    dVar5 += dVar2;
    dVar4 += dVar2;
    dVar3 += dVar2;
  }

  float rnd = mgr.GetActiveRandom()->Float();
  if (dVar5 > rnd)
    return EBehaveType::Lurk;
  else if (dVar4 > (rnd - dVar5))
    return EBehaveType::One;
  else if (dVar3 > (rnd - dVar5) - dVar4)
    return EBehaveType::Attack;
  return EBehaveType::Move;
}

CChozoGhost::CChozoGhost(TUniqueId uid, std::string_view name, const CEntityInfo& info, const zeus::CTransform& xf,
                         CModelData&& mData, const CActorParameters& actParms, const CPatternedInfo& pInfo,
                         float hearingRadius, float fadeOutDelay, float attackDelay, float freezeTime, CAssetId wpsc1,
                         const CDamageInfo& dInfo1, CAssetId wpsc2, const CDamageInfo& dInfo2,
                         const CBehaveChance& chance1, const CBehaveChance& chance2, const CBehaveChance& chance3,
                         u16 soundImpact, float f5, u16 sId2, u16 sId3, u32 w1, float f6, u32 w2, float hurlRecoverTime,
                         CAssetId projectileVisorEffect, s16 soundProjectileVisor, float f8, float f9, u32 nearChance,
                         u32 midChance)
: CPatterned(ECharacter::ChozoGhost, uid, name, EFlavorType::Zero, info, xf, std::move(mData), pInfo,
             EMovementType::Flyer, EColliderType::Zero, EBodyType::BiPedal, actParms, EKnockBackVariant::Medium)
, x568_hearingRadius(hearingRadius)
, x56c_fadeOutDelay(fadeOutDelay)
, x570_attackDelay(attackDelay)
, x574_freezeTime(freezeTime)
, x578_(wpsc1, dInfo1)
, x5a0_(wpsc2, dInfo2)
, x5c8_(chance1)
, x5e8_(chance2)
, x608_(chance3)
, x628_soundImpact(soundImpact)
, x62c_(f5)
, x630_(sId2)
, x632_(sId3)
, x634_(f6)
, x638_hurlRecoverTime(hurlRecoverTime)
, x63c_(w2)
, x650_sound_ProjectileVisor(soundProjectileVisor)
, x654_(f8)
, x658_(f9)
, x65c_nearChance(nearChance)
, x660_midChance(midChance)
, x664_24_onGround(w1)
, x664_25_flinch(!w1)
, x664_26_(false)
, x664_27_(false)
, x664_28_(false)
, x664_29_(false)
, x664_30_(false)
, x664_31_(false)
, x665_24_(true)
, x665_25_(false)
, x665_26_(false)
, x665_27_playerInLeashRange(false)
, x665_28_inRange(false)
, x665_29_aggressive(false)
, x680_behaveType(x664_24_onGround ? EBehaveType::Attack : EBehaveType::Four)
, x68c_boneTracking(*GetModelData()->GetAnimationData(), "Head_1"sv, zeus::degToRad(80.f), zeus::degToRad(180.f),
                    EBoneTrackingFlags::None) {
  x578_.Token().Lock();
  x5a0_.Token().Lock();
  x668_ = GetModelData()->GetScale().z() *
          GetAnimationDistance(CPASAnimParmData(13, CPASAnimParm::FromEnum(3), CPASAnimParm::FromEnum(0)));
  x66c_ = GetModelData()->GetScale().z() *
          GetAnimationDistance(CPASAnimParmData(15, CPASAnimParm::FromEnum(1), CPASAnimParm::FromReal32(90.f)));
  x670_ = GetModelData()->GetScale().z() *
          GetAnimationDistance(CPASAnimParmData(7, CPASAnimParm::FromEnum(1), CPASAnimParm::FromEnum(2)));

  if (projectileVisorEffect.IsValid())
    x640_projectileVisor = g_SimplePool->GetObj({SBIG('PART'), projectileVisorEffect});
  x460_knockBackController.SetEnableBurn(false);
  x460_knockBackController.SetEnableLaggedBurnDeath(false);
  x460_knockBackController.SetEnableShock(false);
  x460_knockBackController.SetEnableFreeze(false);
  CreateShadow(false);
  MakeThermalColdAndHot();
}
void CChozoGhost::AcceptScriptMsg(EScriptObjectMessage msg, TUniqueId uid, CStateManager& mgr) {
  CPatterned::AcceptScriptMsg(msg, uid, mgr);

  switch (msg) {
  case EScriptObjectMessage::Activate:
    AddToTeam(mgr);
    break;
  case EScriptObjectMessage::Deactivate:
  case EScriptObjectMessage::Deleted:
    RemoveFromTeam(mgr);
    break;
  case EScriptObjectMessage::Action:
    if (x664_25_flinch)
      x665_29_aggressive = true;
    break;
  case EScriptObjectMessage::Alert:
    if (x664_26_)
      break;
    x664_26_ = true;
    x400_24_hitByPlayerProjectile = true;
    break;
  case EScriptObjectMessage::Falling:
  case EScriptObjectMessage::Jumped: {
    if (!x328_25_verticalMovement)
      x150_momentum = {0.f, 0.f, -(GetGravityConstant() * GetMass())};
    break;
  }
  case EScriptObjectMessage::InitializedInArea:
    if (GetActive())
      AddToTeam(mgr);
    break;
  default:
    break;
  }
}

void CChozoGhost::Think(float dt, CStateManager& mgr) {
  if (!GetActive())
    return;

  CPatterned::Think(dt, mgr);
  UpdateThermalFrozenState(false);
  x68c_boneTracking.Update(dt);
  x6c8_ = std::max(0.f, x6c8_ - dt);
  xe7_31_targetable = IsVisibleEnough(mgr);
}

void CChozoGhost::PreRender(CStateManager& mgr, const zeus::CFrustum& frustum) {
  x402_29_drawParticles = mgr.GetPlayerState()->GetActiveVisor(mgr) == CPlayerState::EPlayerVisor::XRay;
  if (mgr.GetPlayerState()->GetActiveVisor(mgr) == CPlayerState::EPlayerVisor::Thermal) {
    SetCalculateLighting(false);
    x90_actorLights->BuildConstantAmbientLighting(zeus::skWhite);
  } else {
    SetCalculateLighting(true);
  }

  u8 alpha = GetModelAlphau8(mgr);
  u8 r, g, b, a;
  x42c_color.toRGBA8(r, g, b, a);
  if (alpha < 255 || r == 0) {
    zeus::CColor col;
    if (r == 0) {
      col = zeus::skWhite;
    } else {
      float v = std::max<s8>(0, r - 255) * (1.f / 255.f);
      col.r() = 1.f;
      col.g() = v;
      col.b() = v;
    }
    col.a() = alpha * (1.f / 255.f);
    xb4_drawFlags = CModelFlags(5, 0, 3, col);
  } else {
    xb4_drawFlags = CModelFlags(0, 0, 3, zeus::skWhite);
  }
  CPatterned::PreRender(mgr, frustum);
  x68c_boneTracking.PreRender(mgr, *GetModelData()->GetAnimationData(), GetTransform(), GetModelData()->GetScale(),
                              *GetBodyController());
}

void CChozoGhost::Render(const CStateManager& mgr) const {
  if (x6c8_ > 0.f)
    mgr.DrawSpaceWarp(x6cc_, std::sin((M_PIF * x6c8_) / x56c_fadeOutDelay));

  if (mgr.GetPlayerState()->GetActiveVisor(mgr) == CPlayerState::EPlayerVisor::XRay) {
    CElementGen::SetSubtractBlend(true);
    CElementGen::g_ParticleSystemInitialized = true;
    CGraphics::SetFog(ERglFogMode::PerspLin, 0.f, 75.f, zeus::skBlack);
    mgr.SetupFogForArea3XRange(GetAreaIdAlways());
  }

  CPatterned::Render(mgr);

  if (mgr.GetPlayerState()->GetActiveVisor(mgr) == CPlayerState::EPlayerVisor::XRay) {
    CGraphics::SetFog(ERglFogMode::PerspLin, 0.f, 75.f, zeus::skBlack);
    GetModelData()->GetAnimationData()->GetParticleDB().RenderSystemsToBeDrawnLast();
    mgr.SetupFogForArea(GetAreaIdAlways());
    CElementGen::SetSubtractBlend(false);
    CElementGen::g_ParticleSystemInitialized = false;
  }
}

void CChozoGhost::Touch(CActor& act, CStateManager& mgr) {

  if (IsVisibleEnough(mgr)) {
    if (TCastToPtr<CPlayer> pl = act) {
      if (x420_curDamageRemTime <= 0.f) {
        mgr.ApplyDamage(GetUniqueId(), pl->GetUniqueId(), GetUniqueId(), GetContactDamage(),
                        CMaterialFilter::MakeIncludeExclude({EMaterialTypes::Solid}, {}), {});
        x420_curDamageRemTime = x424_damageWaitTime;
      }
    }
  }
  CPatterned::Touch(act, mgr);
}

EWeaponCollisionResponseTypes CChozoGhost::GetCollisionResponseType(const zeus::CVector3f& pos,
                                                                    const zeus::CVector3f& dir, const CWeaponMode& mode,
                                                                    EProjectileAttrib attrib) const {
  return EWeaponCollisionResponseTypes::ChozoGhost;
}

void CChozoGhost::DoUserAnimEvent(CStateManager& mgr, const CInt32POINode& node, EUserEventType type, float dt) {

  if (type == EUserEventType::FadeIn) {
    if (x664_30_) {
      x3e8_alphaDelta = 2.f;
      CSfxManager::AddEmitter(x630_, GetTranslation(), {}, false, false, 127, -1);
    }
    AddMaterial(EMaterialTypes::Target, mgr);
    x664_30_ = false;
    x664_29_ = true;
    return;
  } else if (type == EUserEventType::Projectile) {
    zeus::CTransform xf =
        zeus::lookAt(GetLctrTransform(node.GetLocatorName()).origin, mgr.GetPlayer().GetAimPosition(mgr, 0.f));
    if (x67c_ == 2) {
      CGameProjectile* proj =
          LaunchProjectile(xf, mgr, 2, EProjectileAttrib::BigStrike | EProjectileAttrib::StaticInterference, true,
                           x640_projectileVisor, x650_sound_ProjectileVisor, false, zeus::skOne3f);
      if (proj) {
        proj->AddAttrib(EProjectileAttrib::BigStrike);
        proj->SetDamageDuration(x62c_);
        proj->AddAttrib(EProjectileAttrib::StaticInterference);
        proj->SetInterferenceDuration(x62c_);
        proj->SetMinHomingDistance(x634_);
      }
    } else {
      CGameProjectile* proj =
          LaunchProjectile(xf, mgr, 5, EProjectileAttrib::DamageFalloff | EProjectileAttrib::StaticInterference, true,
                           {x640_projectileVisor}, x650_sound_ProjectileVisor, false, zeus::skOne3f);
      if (proj) {
        proj->AddAttrib(EProjectileAttrib::BigStrike);
        proj->SetDamageDuration(x62c_);
        proj->AddAttrib(EProjectileAttrib::StaticInterference);
        proj->SetInterferenceDuration(x62c_);
        proj->SetMinHomingDistance(x634_);
      }
    }
    return;
  } else if (type == EUserEventType::FadeOut) {
    if (x664_29_) {
      x3e8_alphaDelta = -2.f;
      CSfxManager::AddEmitter(x632_, GetTranslation(), {}, false, false, 127, -1);
    }
    RemoveMaterial(EMaterialTypes::Target, mgr);
    x664_29_ = false;
    x664_30_ = true;
    x665_26_ = true;
    return;
  }

  CPatterned::DoUserAnimEvent(mgr, node, type, dt);

  if (type == EUserEventType::Delete)
    x3e8_alphaDelta = -1.f;
}

void CChozoGhost::KnockBack(const zeus::CVector3f& dir, CStateManager& mgr, const CDamageInfo& info,
                            EKnockBackType type, bool inDeferred, float magnitude) {
  if (!IsAlive())
    x460_knockBackController.SetAvailableState(EKnockBackAnimationState::Hurled, false);
  else if (!x460_knockBackController.TestAvailableState(EKnockBackAnimationState::KnockBack) &&
           info.GetWeaponMode().IsCharged())
    x460_knockBackController.SetAnimationStateRange(EKnockBackAnimationState::Hurled, EKnockBackAnimationState::Fall);

  CPatterned::KnockBack(dir, mgr, info, type, inDeferred, magnitude);
  if (!IsAlive()) {
    Stop();
    x150_momentum.zeroOut();
  } else if (x460_knockBackController.GetActiveParms().x0_animState == EKnockBackAnimationState::Hurled) {
    x330_stateMachineState.SetState(mgr, *this, GetStateMachine(), "Hurled"sv);
  }
}

bool CChozoGhost::CanBeShot(const CStateManager& mgr, int w1) { return IsVisibleEnough(mgr); }

void CChozoGhost::Dead(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Activate) {
    ReleaseCoverPoint(mgr, x674_coverPoint);
    x3e8_alphaDelta = 4.f;
    x664_30_ = false;
    x664_29_ = false;
    x68c_boneTracking.SetActive(false);
    Stop();
  }
}

void CChozoGhost::SelectTarget(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Activate)
    FindBestAnchor(mgr);
}

void CChozoGhost::Run(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Activate) {
    GetBodyController()->SetLocomotionType(pas::ELocomotionType::Lurk);
    x400_24_hitByPlayerProjectile = false;
    x460_knockBackController.SetAvailableState(EKnockBackAnimationState::KnockBack, false);
    x665_28_inRange = false;
  } else if (msg == EStateMsg::Update) {
    GetBodyController()->GetCommandMgr().DeliverCmd(CBCLocomotionCmd(x688_.Seek(*this, x2e0_destPos), {}, 1.f));
    if (x665_26_) {
      x678_ = x2e0_destPos.z();
      FloatToLevel(x678_, arg);
      GetModelData()->GetAnimationData()->SetParticleEffectState("SpeedSwoosh", true, mgr);
      x665_24_ = false;
      if (!x665_28_inRange) {
        const float range = 2.5f * (arg * x138_velocity.magnitude()) + x66c_;
        x665_28_inRange = (GetTranslation() - x2e0_destPos).magSquared() < range * range;
      }
    }
  } else if (msg == EStateMsg::Deactivate) {
    GetBodyController()->SetLocomotionType(pas::ELocomotionType::Crouch);
    SetDestPos(mgr.GetPlayer().GetTranslation());
    GetModelData()->GetAnimationData()->SetParticleEffectState("SpeedSwoosh", false, mgr);
    x460_knockBackController.SetAvailableState(EKnockBackAnimationState::KnockBack, true);
    x665_28_inRange = false;
  }
}

void CChozoGhost::Generate(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Activate) {
    x330_stateMachineState.SetDelay(x56c_fadeOutDelay);
    x32c_animState = EAnimState::Ready;
    x664_27_ = false;
    CRayCastResult res = mgr.RayStaticIntersection(GetTranslation(), zeus::skDown, 100.f,
                                                   CMaterialFilter::MakeInclude({EMaterialTypes::Floor}));
    if (res.IsInvalid()) {
      x678_ = mgr.GetPlayer().GetTranslation().z();
    } else
      x678_ = res.GetPoint().z();
    x3e8_alphaDelta = 1.f;
    x664_29_ = true;
    if (x56c_fadeOutDelay > 0.f) {
      x6c8_ = x56c_fadeOutDelay;
      FindNearestSolid(mgr, zeus::skDown);
    }
  } else if (msg == EStateMsg::Update) {
    TryCommand(mgr, pas::EAnimationState::Jump, &CPatterned::TryJump, 0);
    if (x32c_animState == EAnimState::Over) {
      x68c_boneTracking.SetActive(true);
      x68c_boneTracking.SetTarget(mgr.GetPlayer().GetUniqueId());
      FloatToLevel(x678_, arg);
    } else if (x32c_animState == EAnimState::Repeat) {
      x450_bodyController->SetLocomotionType(pas::ELocomotionType::Crouch);
      if (!x664_27_) {
        zeus::CVector3f pos = GetTranslation();
        SetTranslation({pos.x(), pos.y(), x678_ + x668_});
        x664_27_ = true;
      }
    }
  } else if (msg == EStateMsg::Deactivate) {
    x32c_animState = EAnimState::NotReady;
    x665_24_ = false;
    x664_27_ = false;
  }
}

void CChozoGhost::Deactivate(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Activate) {
    x68c_boneTracking.SetActive(false);
    ReleaseCoverPoint(mgr, x674_coverPoint);
    x32c_animState = EAnimState::Ready;
    x665_24_ = true;
  } else if (msg == EStateMsg::Update) {
    TryCommand(mgr, pas::EAnimationState::Generate, &CPatterned::TryGenerate, 1);
    if (x32c_animState == EAnimState::Repeat)
      GetBodyController()->SetLocomotionType(pas::ELocomotionType::Relaxed);
  } else if (msg == EStateMsg::Deactivate) {
    x32c_animState = EAnimState::NotReady;
  }
}

void CChozoGhost::Attack(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Activate) {
    CTeamAiMgr::AddAttacker(CTeamAiMgr::EAttackType::Melee, mgr, x6c4_teamMgr, GetUniqueId());
    x32c_animState = EAnimState::Ready;
    if (x6d8_ == 1)
      x67c_ = 3;
    else if (x6d8_ == 2)
      x67c_ = 4;
    else if (x6d8_ == 3)
      x67c_ = 5;

    if (x665_26_ && mgr.RayStaticIntersection(GetTranslation() + (zeus::skUp * 0.5f), zeus::skUp, x670_,
                                              CMaterialFilter::MakeInclude({EMaterialTypes::Solid}))
                        .IsValid()) {
      x67c_ = 2;
      x460_knockBackController.SetAvailableState(EKnockBackAnimationState::KnockBack, false);
    }
    x150_momentum.zeroOut();
    xfc_constantForce.zeroOut();
  } else if (msg == EStateMsg::Update) {
    TryCommand(mgr, pas::EAnimationState::MeleeAttack, &CPatterned::TryMeleeAttack, x67c_);
    GetBodyController()->GetCommandMgr().SetTargetVector(mgr.GetPlayer().GetTranslation() - GetTranslation());
    if (x67c_ != 2)
      FloatToLevel(x678_, arg);
  } else if (msg == EStateMsg::Deactivate) {
    x32c_animState = EAnimState::NotReady;
    x665_26_ = false;
    x460_knockBackController.SetAvailableState(EKnockBackAnimationState::KnockBack, true);
    CTeamAiMgr::ResetTeamAiRole(CTeamAiMgr::EAttackType::Ranged, mgr, x6c4_teamMgr, GetUniqueId(), true);
  }
}

void CChozoGhost::Shuffle(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg != EStateMsg::Activate)
    return;

  const CBehaveChance& chance1 = ChooseBehaveChanceRange(mgr);
  CTeamAiRole* role = CTeamAiMgr::GetTeamAiRole(mgr, x6c4_teamMgr, GetUniqueId());
  if (role && role->GetTeamAiRole() == CTeamAiRole::ETeamAiRole::Ranged &&
      !CTeamAiMgr::CanAcceptAttacker(CTeamAiMgr::EAttackType::Ranged, mgr, x6c4_teamMgr, GetUniqueId())) {
    x680_behaveType = EBehaveType::Attack;
  }

  const CBehaveChance& chance2 = ChooseBehaveChanceRange(mgr);
  x680_behaveType = chance2.GetBehave(x680_behaveType, mgr);
  if (x680_behaveType == EBehaveType::Lurk)
    x684_lurkDelay = chance1.GetLurkTime();
  else if (x680_behaveType == EBehaveType::Attack) {
    x665_25_ = mgr.GetActiveRandom()->Float() < chance1.GetChargeAttack();
    const int rnd = mgr.GetActiveRandom()->Next();
    x6d8_ = (rnd - (rnd / chance1.GetNumBolts()) * chance1.GetNumBolts()) + 1;
  }
  x664_31_ = false;
  x665_27_playerInLeashRange = false;
}

void CChozoGhost::InActive(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Activate) {
    if (!x450_bodyController->GetActive())
      x450_bodyController->Activate(mgr);

    if (x63c_ == 3) {
      x450_bodyController->SetLocomotionType(pas::ELocomotionType::Crouch);
      x42c_color.a() = 1.f;
    } else {
      x450_bodyController->SetLocomotionType(pas::ELocomotionType::Relaxed);
      x42c_color.a() = 0.f;
    }

    RemoveMaterial(EMaterialTypes::Solid, mgr);
    x150_momentum.zeroOut();
    x665_24_ = true;
  }
}

void CChozoGhost::Taunt(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Activate) {
    x32c_animState = EAnimState::Ready;
  } else if (msg == EStateMsg::Update) {
    TryCommand(mgr, pas::EAnimationState::Taunt, &CPatterned::TryTaunt, 0);
    FloatToLevel(x678_, arg);
  } else {
    x32c_animState = EAnimState::NotReady;
    x665_26_ = false;
  }
}

void CChozoGhost::Hurled(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Activate) {
    x328_25_verticalMovement = false;
    x664_27_ = false;
    x665_24_ = true;
  } else if (msg == EStateMsg::Update) {
    x3e8_alphaDelta = 2.f;
    if (x664_27_)
      return;

    if (x138_velocity.z() < 0.f) {
      CRayCastResult res = mgr.RayStaticIntersection(GetTranslation() + zeus::skUp, zeus::skDown, 2.f,
                                                     CMaterialFilter::MakeInclude({EMaterialTypes::Floor}));
      if (res.IsValid()) {
        x664_27_ = true;
        x150_momentum.zeroOut();
        zeus::CVector3f velNoZ = x138_velocity;
        velNoZ.z() = 0.f;
        SetVelocityWR(velNoZ);
        x678_ = res.GetPoint().z();
        x330_stateMachineState.SetCodeTrigger();
      }
    }
    if (!x664_27_ && x638_hurlRecoverTime < x330_stateMachineState.GetTime()) {
      GetBodyController()->GetCommandMgr().DeliverCmd(CBodyStateCmd(EBodyStateCmd::ExitState));
      GetBodyController()->SetLocomotionType(pas::ELocomotionType::Lurk);
      x330_stateMachineState.SetCodeTrigger();
    }
  } else if (msg == EStateMsg::Deactivate) {
    x328_25_verticalMovement = true;
    x150_momentum.zeroOut();
  }
}

void CChozoGhost::WallDetach(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Activate) {
    x330_stateMachineState.SetDelay(x56c_fadeOutDelay);
    x3e8_alphaDelta = 0.f;
    x664_29_ = false;
    if (x56c_fadeOutDelay > 0.f) {
      x6c8_ = x56c_fadeOutDelay;
      FindNearestSolid(mgr, GetTransform().basis[1]);
    }
    TUniqueId wpId = GetWaypointForState(mgr, EScriptObjectState::Attack, EScriptObjectMessage::Follow);
    TCastToConstPtr<CScriptWaypoint> wp;
    if (wpId != kInvalidUniqueId) {
      wp = TCastToConstPtr<CScriptWaypoint>(mgr.GetObjectById(wpId));
    }
    if (wp)
      SetDestPos(wp->GetTranslation());
    else
      SetDestPos(GetTranslation() + (2.f * x66c_) * GetTranslation());

    SendScriptMsgs(EScriptObjectState::Attack, mgr, EScriptObjectMessage::Follow);
  } else if (msg == EStateMsg::Deactivate) {
    x68c_boneTracking.SetActive(true);
    x68c_boneTracking.SetTarget(mgr.GetPlayer().GetUniqueId());
    x665_24_ = false;
    x680_behaveType = EBehaveType::Move;
  }
}

void CChozoGhost::Growth(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Activate) {
    x330_stateMachineState.SetDelay(x56c_fadeOutDelay);
    GetBodyController()->SetLocomotionType(pas::ELocomotionType::Crouch);
    x3e8_alphaDelta = 1.f;
    x664_29_ = true;
    if (x56c_fadeOutDelay > 0.f) {
      x6c8_ = x56c_fadeOutDelay;
      FindNearestSolid(mgr, zeus::skUp);
    }
  } else if (msg == EStateMsg::Deactivate) {
    x665_24_ = false;
    x68c_boneTracking.SetActive(false);
    x68c_boneTracking.SetTarget(mgr.GetPlayer().GetUniqueId());
  }
}

void CChozoGhost::Land(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Update) {
    FloatToLevel(x678_, arg);
    if (std::fabs(x678_ - GetTranslation().z()) < 0.05f) {
      x330_stateMachineState.SetCodeTrigger();
    }
  }
}

void CChozoGhost::Lurk(CStateManager& mgr, EStateMsg msg, float arg) {
  if (msg == EStateMsg::Activate) {
    x330_stateMachineState.SetDelay(x684_lurkDelay);
  } else if (msg == EStateMsg::Update) {
    FloatToLevel(x678_, arg);
  }
}

bool CChozoGhost::Leash(CStateManager& mgr, float arg) {
  return x665_27_playerInLeashRange || CPatterned::Leash(mgr, arg);
}

bool CChozoGhost::InRange(CStateManager& mgr, float arg) { return x665_28_inRange; }

bool CChozoGhost::InPosition(CStateManager& mgr, float arg) { return x680_behaveType == EBehaveType::Attack; }

bool CChozoGhost::AggressionCheck(CStateManager& mgr, float arg) { return x665_29_aggressive; }

bool CChozoGhost::ShouldTaunt(CStateManager& mgr, float arg) { return x680_behaveType == EBehaveType::One; }

bool CChozoGhost::ShouldFlinch(CStateManager& mgr, float arg) { return x664_25_flinch; }

bool CChozoGhost::ShouldMove(CStateManager& mgr, float arg) { return x680_behaveType == EBehaveType::Move; }

bool CChozoGhost::AIStage(CStateManager& mgr, float arg) { return arg == x63c_; }

u8 CChozoGhost::GetModelAlphau8(const CStateManager& mgr) const {
  if (mgr.GetPlayerState()->GetActiveVisor(mgr) == CPlayerState::EPlayerVisor::XRay && IsAlive())
    return 255;
  return u8(x42c_color.a() * 255);
}

bool CChozoGhost::IsOnGround() const { return x664_24_onGround; }

CProjectileInfo* CChozoGhost::GetProjectileInfo() { return x67c_ == 2 ? &x578_ : &x5a0_; }

void CChozoGhost::AddToTeam(CStateManager& mgr) {
  if (x6c4_teamMgr == kInvalidUniqueId)
    x6c4_teamMgr = CTeamAiMgr::GetTeamAiMgr(*this, mgr);

  if (x6c4_teamMgr == kInvalidUniqueId)
    return;

  if (TCastToPtr<CTeamAiMgr> teamMgr = mgr.ObjectById(x6c4_teamMgr))
    teamMgr->AssignTeamAiRole(*this, CTeamAiRole::ETeamAiRole::Ranged, CTeamAiRole::ETeamAiRole::Unknown,
                              CTeamAiRole::ETeamAiRole::Invalid);
}

void CChozoGhost::RemoveFromTeam(CStateManager& mgr) {
  if (x6c4_teamMgr == kInvalidUniqueId)
    return;

  if (TCastToPtr<CTeamAiMgr> teamMgr = mgr.ObjectById(x6c4_teamMgr)) {
    if (teamMgr->IsPartOfTeam(GetUniqueId())) {
      teamMgr->RemoveTeamAiRole(GetUniqueId());
      x6c4_teamMgr = kInvalidUniqueId;
    }
  }
}

void CChozoGhost::FloatToLevel(float f1, float f2) {
  zeus::CVector3f pos = GetTranslation();
  pos.z() = 4.f * (f1 - pos.z()) * f2 + pos.z();
  SetTranslation(pos);
}

const CChozoGhost::CBehaveChance& CChozoGhost::ChooseBehaveChanceRange(CStateManager& mgr) {
  const float dist = (GetTranslation() - mgr.GetPlayer().GetTranslation()).magnitude();
  if (x654_ <= dist && x658_ > dist)
    return x5e8_;
  else if (x658_ <= dist)
    return x608_;
  else
    return x5c8_;
}
void CChozoGhost::FindNearestSolid(CStateManager& mgr, const zeus::CVector3f& dir) {
  CRayCastResult res = mgr.RayStaticIntersection(GetBoundingBox().center() + (dir * 8.f), -dir, 8.f,
                                                 CMaterialFilter::MakeInclude({EMaterialTypes::Solid}));
  if (res.IsInvalid()) {
    x6cc_ = GetBoundingBox().center() + dir;
  } else
    x6cc_ = res.GetPoint();
}

void CChozoGhost::FindBestAnchor(urde::CStateManager& mgr) {
  x665_27_playerInLeashRange = false;
  u32 chance = mgr.GetActiveRandom()->Next() % 100;
  chance = chance < x65c_nearChance ? 0 : (chance < (x65c_nearChance + x660_midChance)) + 2;
  float dVar10 = 10.f;
  float dVar15 = dVar10 * x658_;
  float dVar14 = dVar15;
  float dVar13 = dVar15;
  if (chance == 0) {
    dVar13 = dVar15 * dVar10;
    dVar15 *= 5.f;
  } else if (chance == 1) {
    dVar13 = dVar15 * 5.f;
    dVar15 *= dVar10;
  } else if (chance == 2) {
    dVar14 = dVar15 * 5.f;
    dVar15 *= dVar10;
  }

  float prevDist = FLT_MAX;
  CScriptCoverPoint* target = nullptr;
  for (CEntity* ent : mgr.GetAiWaypointObjectList()) {
    if (TCastToPtr<CScriptCoverPoint> cover = ent) {
      if (cover->GetActive() && !cover->GetInUse(kInvalidUniqueId) && cover->GetAreaIdAlways() == GetAreaId()) {
        float fVar17 = (cover->GetTranslation() - GetTranslation()).magnitude();
        if (2.f * x66c_ <= fVar17) {
          float dist = std::max(0.f, x654_ - fVar17);
          zeus::CVector3f diff = (cover->GetTranslation() - mgr.GetPlayer().GetTranslation());
          fVar17 = diff.magnitude();
          if (x2fc_minAttackRange <= fVar17) {
            if (std::fabs(diff.z()) / fVar17 < 0.2f) {
              dist = (20.f * x658_) * ((std::fabs(diff.z()) / fVar17) - 0.2f) + dist;
            }
            if (x654_ <= dVar10) {
              if (x658_ <= dVar10) {
                dist = (dist + dVar13);
              } else {
                dist = (dist + dVar14);
                if (dist < prevDist) {
                  fVar17 = 1.f / dVar10;
                  diff = diff * fVar17;
                  dist += (10.f * x658_) * (1.f * mgr.GetPlayer().GetTransform().basis[1].dot(diff));
                }
              }
            } else {
              dist += dVar15;
              if (dist < prevDist) {
                fVar17 = 1.f / dVar10;
                diff = diff * fVar17;
                dist += (10.f * x658_) * (1.f * mgr.GetPlayer().GetTransform().basis[1].dot(diff));
              }
            }
            if (dist < prevDist) {
              dist += x658_ * mgr.GetActiveRandom()->Float();
              if (dist < prevDist) {
                x665_27_playerInLeashRange = false;
                target = cover;
                prevDist = dist;
              }
            }
          }
        }
      }
    }
  }

  if (target) {
    x2dc_destObj = target->GetUniqueId();
    SetDestPos(target->GetTranslation());
    ReleaseCoverPoint(mgr, x674_coverPoint);
    SetCoverPoint(target, x674_coverPoint);
  } else if (mgr.GetPlayer().GetAreaIdAlways() == GetAreaIdAlways()) {
    x2dc_destObj = mgr.GetPlayer().GetUniqueId();
    zeus::CVector3f destPos =
        mgr.GetPlayer().GetTranslation() - x654_ * (mgr.GetPlayer().GetTranslation() - GetTranslation()).normalized();
    CRayCastResult res =
        mgr.RayStaticIntersection(destPos, zeus::skDown, 8.f, CMaterialFilter::MakeInclude(EMaterialTypes::Floor));
    if (res.IsValid())
      destPos = res.GetPoint();
    SetDestPos(destPos);
  } else {
    x2dc_destObj = kInvalidUniqueId;
    x2e0_destPos = GetTranslation();
  }
}
} // namespace urde::MP1