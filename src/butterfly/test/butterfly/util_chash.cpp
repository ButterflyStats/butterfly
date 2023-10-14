/**
 * @file util_chash.cpp
 * @author Robin Dietrich <me (at) invokr (dot) org>
 *
 * @par License
 *    Butterfly Replay Parser
 *    Copyright 2014-2016 Robin Dietrich
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <unordered_map>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <butterfly/util_chash.hpp>

using namespace butterfly;

#define TEST_HASH( hash_string )                                                                                       \
    {                                                                                                                  \
        auto it = clashmap.find( hash_string##_chash );                                                                \
        REQUIRE( it == clashmap.end() );                                                                               \
        clashmap[hash_string##_chash] = hash_string;                                                                   \
    }

TEST_CASE( "constexpr_hash equality", "[util_chash.hpp]" ) {
    auto hash1 = "HashMePlease"_chash;                // From string literal
    auto hash2 = constexpr_hash_rt( "HashMePlease" ); // At compile time
    auto hash3 = constexpr_hash( "HashMePlease" );    // At run time

    REQUIRE( hash1 == hash2 );
    REQUIRE( hash2 == hash3 );
    REQUIRE( hash1 != 0 );
}

TEST_CASE( "constexpr_hash collision", "[util_chash.hpp]" ) {
    std::unordered_map<uint64_t, std::string> clashmap;

    // Types
    TEST_HASH( "BeamClipStyle_t" )
    TEST_HASH( "BeamType_t" )
    TEST_HASH( "CBodyComponent" )
    TEST_HASH( "CDOTAGameManager*" )
    TEST_HASH( "CDOTAGamerules*" )
    TEST_HASH( "CDOTASpectatorGraphManager*" )
    TEST_HASH( "CDOTA_AbilityDraftAbilityState[MAX_ABILITY_DRAFT_ABILITIES]" )
    TEST_HASH( "CEntityHandle" )
    TEST_HASH( "CEntityIdentity*" )
    TEST_HASH( "CGameSceneNodeHandle" )
    TEST_HASH( "CHandle< CBaseCombatCharacter >" )
    TEST_HASH( "CHandle< CBaseCombatWeapon >" )
    TEST_HASH( "CHandle< CBaseEntity >" )
    TEST_HASH( "CHandle< CBaseEntity >[10]" )
    TEST_HASH( "CHandle< CBaseEntity >[14]" )
    TEST_HASH( "CHandle< CBaseEntity >[17]" )
    TEST_HASH( "CHandle< CBaseEntity >[2]" )
    TEST_HASH( "CHandle< CBaseEntity >[4]" )
    TEST_HASH( "CHandle< CBaseEntity >[64]" )
    TEST_HASH( "CHandle< CBaseModelEntity >" )
    TEST_HASH( "CHandle< CBasePlayer >" )
    TEST_HASH( "CHandle< CBasePropDoor >" )
    TEST_HASH( "CHandle< CBaseVRHandAttachment >" )
    TEST_HASH( "CHandle< CBaseViewModel >[2]" )
    TEST_HASH( "CHandle< CColorCorrection >" )
    TEST_HASH( "CHandle< CDOTAPlayer >" )
    TEST_HASH( "CHandle< CDOTASpecGraphPlayerData >[24]" )
    TEST_HASH( "CHandle< CDOTA_Ability_Meepo_DividedWeStand >" )
    TEST_HASH( "CHandle< CDOTA_BaseNPC >" )
    TEST_HASH( "CHandle< CDOTA_BaseNPC >[10]" )
    TEST_HASH( "CHandle< CDOTA_BaseNPC_Hero >" )
    TEST_HASH( "CHandle< CDOTA_Item >" )
    TEST_HASH( "CHandle< CDotaSubquestBase >[8]" )
    TEST_HASH( "CHandle< CFogController >" )
    TEST_HASH( "CHandle< CPostProcessController >" )
    TEST_HASH( "CHandle< CPropHMDAvatar >" )
    TEST_HASH( "CHandle< CPropVRHand >" )
    TEST_HASH( "CHandle< CPropVRHand >[2]" )
    TEST_HASH( "CHandle< CPropZipline >" )
    TEST_HASH( "CLightComponent" )
    TEST_HASH( "CNetworkedQuantizedFloat" )
    TEST_HASH( "CPhysicsComponent" )
    TEST_HASH( "CPlayerLocalData" )
    TEST_HASH( "CRenderComponent" )
    TEST_HASH( "CStrongHandle< InfoForResourceTypeCModel >" )
    TEST_HASH( "CStrongHandle< InfoForResourceTypeCPostProcessingResource >" )
    TEST_HASH( "CStrongHandle< InfoForResourceTypeCTextureBase >" )
    TEST_HASH( "CStrongHandle< InfoForResourceTypeIMaterial2 >" )
    TEST_HASH( "CStrongHandle< InfoForResourceTypeIMaterial2 >[6]" )
    TEST_HASH( "CStrongHandle< InfoForResourceTypeIParticleSystemDefinition >" )
    TEST_HASH( "CUtlString" )
    TEST_HASH( "CUtlStringToken" )
    TEST_HASH( "CUtlSymbolLarge" )
    TEST_HASH( "CUtlSymbolLarge[10]" )
    TEST_HASH( "CUtlVector< CAnimationLayer >" )
    TEST_HASH( "CUtlVector< CDOTASubChallengeInfo >" )
    TEST_HASH( "CUtlVector< CDOTA_ItemStockInfo >" )
    TEST_HASH( "CUtlVector< CDOTA_PlayerChallengeInfo >" )
    TEST_HASH( "CUtlVector< CEconItemAttribute >" )
    TEST_HASH( "CUtlVector< CHandle< CBaseCombatWeapon > >" )
    TEST_HASH( "CUtlVector< CHandle< CBaseEntity > >" )
    TEST_HASH( "CUtlVector< CHandle< CBaseFlex > >" )
    TEST_HASH( "CUtlVector< CHandle< CBasePlayer > >" )
    TEST_HASH( "CUtlVector< CHandle< CEconWearable > >" )
    TEST_HASH( "CUtlVector< CHandle< CIngameEvent_Base > >" )
    TEST_HASH( "CUtlVector< CHeroStatueLiked >" )
    TEST_HASH( "CUtlVector< CHeroesPerPlayer >" )
    TEST_HASH( "CUtlVector< CShowcaseSlot >" )
    TEST_HASH( "CUtlVector< DataTeamPlayer_t >" )
    TEST_HASH( "CUtlVector< PlayerResourceBroadcasterData_t >" )
    TEST_HASH( "CUtlVector< PlayerResourcePlayerData_t >" )
    TEST_HASH( "CUtlVector< PlayerResourcePlayerTeamData_t >" )
    TEST_HASH( "CUtlVector< TempViewerInfo_t >" )
    TEST_HASH( "CUtlVector< float32 >" )
    TEST_HASH( "CUtlVector< int32 >" )
    TEST_HASH( "Color" )
    TEST_HASH( "CourierState_t" )
    TEST_HASH( "DOTA_CombatLogQueryProgress" )
    TEST_HASH( "DOTA_HeroPickState" )
    TEST_HASH( "DOTA_SHOP_TYPE" )
    TEST_HASH( "DamageOptions_t" )
    TEST_HASH( "DoorState_t" )
    TEST_HASH( "ECNYBeastAbility[24]" )
    TEST_HASH( "EShowcaseEditorMode" )
    TEST_HASH( "ETeamShowcase_SlotType" )
    TEST_HASH( "EntityDisolveType_t" )
    TEST_HASH( "HSequence" )
    TEST_HASH( "MaterialModifyMode_t" )
    TEST_HASH( "MoveCollide_t" )
    TEST_HASH( "MoveType_t" )
    TEST_HASH( "PointWorldTextJustifyHorizontal_t" )
    TEST_HASH( "PointWorldTextJustifyVertical_t" )
    TEST_HASH( "PointWorldTextReorientMode_t" )
    TEST_HASH( "PoseController_FModType_t" )
    TEST_HASH( "PrecipitationType_t" )
    TEST_HASH( "QAngle" )
    TEST_HASH( "QAngle[32]" )
    TEST_HASH( "RenderFx_t" )
    TEST_HASH( "RenderMode_t" )
    TEST_HASH( "ScreenEffectType_t" )
    TEST_HASH( "ShopItemViewMode_t" )
    TEST_HASH( "SolidType_t" )
    TEST_HASH( "SurroundingBoundsType_t" )
    TEST_HASH( "ValueRemapperHapticsType_t" )
    TEST_HASH( "ValueRemapperInputType_t" )
    TEST_HASH( "ValueRemapperMomentumType_t" )
    TEST_HASH( "ValueRemapperOutputType_t" )
    TEST_HASH( "ValueRemapperRatchetType_t" )
    TEST_HASH( "Vector" )
    TEST_HASH( "Vector[32]" )
    TEST_HASH( "Vector[4]" )
    TEST_HASH( "Vector[8]" )
    TEST_HASH( "WeaponState_t" )
    TEST_HASH( "attributeprovidertypes_t" )
    TEST_HASH( "bool" )
    TEST_HASH( "bool[10]" )
    TEST_HASH( "bool[128]" )
    TEST_HASH( "bool[24]" )
    TEST_HASH( "bool[66]" )
    TEST_HASH( "bool[9]" )
    TEST_HASH( "char[128]" )
    TEST_HASH( "char[129]" )
    TEST_HASH( "char[18]" )
    TEST_HASH( "char[255]" )
    TEST_HASH( "char[256]" )
    TEST_HASH( "char[260]" )
    TEST_HASH( "char[32]" )
    TEST_HASH( "char[33]" )
    TEST_HASH( "char[512]" )
    TEST_HASH( "char[64]" )
    TEST_HASH( "color32" )
    TEST_HASH( "float32" )
    TEST_HASH( "float32[10]" )
    TEST_HASH( "float32[14]" )
    TEST_HASH( "float32[150]" )
    TEST_HASH( "float32[24]" )
    TEST_HASH( "float32[2]" )
    TEST_HASH( "float32[3]" )
    TEST_HASH( "gender_t" )
    TEST_HASH( "int16" )
    TEST_HASH( "int32" )
    TEST_HASH( "int32[100]" )
    TEST_HASH( "int32[10]" )
    TEST_HASH( "int32[12]" )
    TEST_HASH( "int32[14]" )
    TEST_HASH( "int32[24]" )
    TEST_HASH( "int32[2]" )
    TEST_HASH( "int32[4]" )
    TEST_HASH( "int32[64]" )
    TEST_HASH( "int32[66]" )
    TEST_HASH( "int32[99]" )
    TEST_HASH( "int32[9]" )
    TEST_HASH( "int64" )
    TEST_HASH( "int8" )
    TEST_HASH( "item_definition_index_t" )
    TEST_HASH( "itemid_t" )
    TEST_HASH( "m_SpeechBubbles" )
    TEST_HASH( "uint16" )
    TEST_HASH( "uint32" )
    TEST_HASH( "uint32[32]" )
    TEST_HASH( "uint32[66]" )
    TEST_HASH( "uint64" )
    TEST_HASH( "uint64[256]" )
    TEST_HASH( "uint64[4]" )
    TEST_HASH( "uint8" )
    TEST_HASH( "uint8[10]" )
    TEST_HASH( "uint8[16]" )
    TEST_HASH( "uint8[18]" )
    TEST_HASH( "uint8[24]" )
    TEST_HASH( "uint8[2]" )
    TEST_HASH( "uint8[32]" )
    TEST_HASH( "uint8[4]" )
    TEST_HASH( "uint8[64]" )
    TEST_HASH( "uint8[6]" )
    TEST_HASH( "uint8[8]" )

    // Encoders
    TEST_HASH( "" );
    TEST_HASH( "coord" );
    TEST_HASH( "fixed64" );
    TEST_HASH( "normal" );
    TEST_HASH( "qangle_pitch_yaw" );
}
