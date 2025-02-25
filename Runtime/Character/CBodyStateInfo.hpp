#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "Runtime/RetroTypes.hpp"
#include "Runtime/Character/CAdditiveBodyState.hpp"
#include "Runtime/Character/CBodyState.hpp"
#include "Runtime/Character/CharacterCommon.hpp"

namespace urde {
class CActor;

class CBodyStateInfo {
  friend class CBodyController;
  std::map<pas::EAnimationState, std::unique_ptr<CBodyState>> x0_stateMap;
  pas::EAnimationState x14_state = pas::EAnimationState::Invalid;
  CBodyController* x18_bodyController = nullptr;
  std::vector<std::pair<pas::EAnimationState, std::unique_ptr<CAdditiveBodyState>>> x1c_additiveStates;
  pas::EAnimationState x2c_additiveState = pas::EAnimationState::AdditiveIdle;
  float x30_maxPitch = 0.f;
  bool x34_24_changeLocoAtEndOfAnimOnly;
  std::unique_ptr<CBodyState> SetupRestrictedFlyerBodyStates(pas::EAnimationState stateId, CActor& actor) const;
  std::unique_ptr<CBodyState> SetupNewFlyerBodyStates(pas::EAnimationState stateId, CActor& actor) const;
  std::unique_ptr<CBodyState> SetupWallWalkerBodyStates(pas::EAnimationState stateId, CActor& actor) const;
  std::unique_ptr<CBodyState> SetupPitchableBodyStates(pas::EAnimationState stateId, CActor& actor) const;
  std::unique_ptr<CBodyState> SetupFlyerBodyStates(pas::EAnimationState stateId, CActor& actor) const;
  std::unique_ptr<CBodyState> SetupRestrictedBodyStates(pas::EAnimationState stateId, CActor& actor) const;
  std::unique_ptr<CBodyState> SetupBiPedalBodyStates(pas::EAnimationState stateId, CActor& actor) const;

public:
  CBodyStateInfo(CActor& actor, EBodyType type);
  float GetLocomotionSpeed(pas::ELocomotionAnim anim) const;
  float GetMaxSpeed() const;
  float GetMaximumPitch() const { return x30_maxPitch; }
  void SetMaximumPitch(float pitch) { x30_maxPitch = pitch; }
  bool GetLocoAnimChangeAtEndOfAnimOnly() const { return x34_24_changeLocoAtEndOfAnimOnly; }
  void SetLocoAnimChangeAtEndOfAnimOnly(bool s) { x34_24_changeLocoAtEndOfAnimOnly = s; }
  CBodyState* GetCurrentState();
  const CBodyState* GetCurrentState() const;
  pas::EAnimationState GetCurrentStateId() const { return x14_state; }
  void SetState(pas::EAnimationState s);
  CAdditiveBodyState* GetCurrentAdditiveState();
  pas::EAnimationState GetCurrentAdditiveStateId() const { return x2c_additiveState; }
  void SetAdditiveState(pas::EAnimationState s);
  bool ApplyHeadTracking() const;
};

} // namespace urde
