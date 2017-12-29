#ifndef _DNAMP1_SCRIPTTYPES_HPP_
#define _DNAMP1_SCRIPTTYPES_HPP_
#include "Actor.hpp"
#include "ActorContraption.hpp"
#include "ActorKeyframe.hpp"
#include "ActorRotate.hpp"
#include "AIJumpPoint.hpp"
#include "AIKeyframe.hpp"
#include "AmbientAI.hpp"
#include "AreaAttributes.hpp"
#include "AtomicAlpha.hpp"
#include "AtomicBeta.hpp"
#include "Babygoth.hpp"
#include "BallTrigger.hpp"
#include "Beetle.hpp"
#include "BloodFlower.hpp"
#include "Burrower.hpp"
#include "Camera.hpp"
#include "CameraBlurKeyframe.hpp"
#include "CameraFilterKeyframe.hpp"
#include "CameraHint.hpp"
#include "CameraHintTrigger.hpp"
#include "CameraPitchVolume.hpp"
#include "CameraShaker.hpp"
#include "CameraWaypoint.hpp"
#include "ChozoGhost.hpp"
#include "ColorModulate.hpp"
#include "ControllerAction.hpp"
#include "Counter.hpp"
#include "CoverPoint.hpp"
#include "DamageableTrigger.hpp"
#include "Debris.hpp"
#include "DebrisExtended.hpp"
#include "DebugCameraWaypoint.hpp"
#include "DistanceFog.hpp"
#include "Dock.hpp"
#include "DockAreaChange.hpp"
#include "DoorArea.hpp"
#include "Drone.hpp"
#include "Effect.hpp"
#include "ElectroMagneticPulse.hpp"
#include "ElitePirate.hpp"
#include "EnergyBall.hpp"
#include "EnvFxDensityController.hpp"
#include "Eyeball.hpp"
#include "FireFlea.hpp"
#include "FishCloud.hpp"
#include "FishCloudModifier.hpp"
#include "Flaahgra.hpp"
#include "FlaahgraTentacle.hpp"
#include "FlickerBat.hpp"
#include "FlyingPirate.hpp"
#include "FogVolume.hpp"
#include "Geemer.hpp"
#include "Generator.hpp"
#include "GrapplePoint.hpp"
#include "GunTurret.hpp"
#include "HUDMemo.hpp"
#include "IceSheegoth.hpp"
#include "IceZoomer.hpp"
#include "JellyZap.hpp"
#include "Magdolite.hpp"
#include "MazeNode.hpp"
#include "MemoryRelay.hpp"
#include "MetareeAlpha.hpp"
#include "MetroidAlpha.hpp"
#include "MetroidBeta.hpp"
#include "MetroidPrimeStage1.hpp"
#include "MetroidPrimeStage2.hpp"
#include "Midi.hpp"
#include "NewCameraShaker.hpp"
#include "NewIntroBoss.hpp"
#include "Oculus.hpp"
#include "OmegaPirate.hpp"
#include "Parasite.hpp"
#include "PathCamera.hpp"
#include "PhazonHealingNodule.hpp"
#include "PhazonPool.hpp"
#include "Pickup.hpp"
#include "PickupGenerator.hpp"
#include "Platform.hpp"
#include "PlayerActor.hpp"
#include "PlayerHint.hpp"
#include "PlayerStateChange.hpp"
#include "PointOfInterest.hpp"
#include "PuddleSpore.hpp"
#include "PuddleToadGamma.hpp"
#include "Puffer.hpp"
#include "RadialDamage.hpp"
#include "RandomRelay.hpp"
#include "Relay.hpp"
#include "Repulsor.hpp"
#include "Ridley.hpp"
#include "Ripper.hpp"
#include "Ripple.hpp"
#include "RoomAcoustics.hpp"
#include "RumbleEffect.hpp"
#include "ScriptBeam.hpp"
#include "Seedling.hpp"
#include "ShadowProjector.hpp"
#include "SnakeWeedSwarm.hpp"
#include "Sound.hpp"
#include "SpacePirate.hpp"
#include "SpankWeed.hpp"
#include "SpawnPoint.hpp"
#include "SpecialFunction.hpp"
#include "SpiderBallAttractionSurface.hpp"
#include "SpiderBallWaypoint.hpp"
#include "SpindleCamera.hpp"
#include "Steam.hpp"
#include "StreamedAudio.hpp"
#include "Switch.hpp"
#include "TargetingPoint.hpp"
#include "TeamAIMgr.hpp"
#include "Thardus.hpp"
#include "ThardusRockProjectile.hpp"
#include "ThermalHeatFader.hpp"
#include "Timer.hpp"
#include "Trigger.hpp"
#include "Tryclops.hpp"
#include "VisorFlare.hpp"
#include "VisorGoo.hpp"
#include "WallCrawlerSwarm.hpp"
#include "Warwasp.hpp"
#include "Water.hpp"
#include "Waypoint.hpp"
#include "WorldLightFader.hpp"
#include "WorldTeleporter.hpp"
#include <functional>
#include <list>

namespace DataSpec::DNAMP1
{
struct ScriptObjectSpec
{
    atUint8 type;
    std::function<IScriptObject*()> a;
};

extern const std::vector<const struct ScriptObjectSpec*> SCRIPT_OBJECT_DB;
}
#endif
