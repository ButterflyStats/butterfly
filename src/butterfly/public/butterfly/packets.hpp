/**
 * @file packets.hpp
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

#ifndef BUTTERFLY_PACKETS_HPP
#define BUTTERFLY_PACKETS_HPP

// Macros to register demo packets in packet_create

#define PKG_CREATE( _ID_, _NAME_ )                                                                                     \
    case _ID_: {                                                                                                       \
        _NAME_* proto = new _NAME_;                                                                                    \
        proto->ParseFromArray( data, size );                                                                           \
        return proto;                                                                                                  \
    } break;

#define PKG_CREATE_NET( _ID_ )                                                                                         \
    case net_##_ID_: {                                                                                                 \
        CNETMsg_##_ID_* proto = new CNETMsg_##_ID_;                                                                    \
        proto->ParseFromArray( data, size );                                                                           \
        return proto;                                                                                                  \
    } break;

#define PKG_CREATE_SVC( _ID_ )                                                                                         \
    case svc_##_ID_: {                                                                                                 \
        CSVCMsg_##_ID_* proto = new CSVCMsg_##_ID_;                                                                    \
        proto->ParseFromArray( data, size );                                                                           \
        return proto;                                                                                                  \
    } break;

#define PKG_CREATE_UM( _ID_ )                                                                                          \
    case UM_##_ID_: {                                                                                                  \
        CUserMessage##_ID_* proto = new CUserMessage##_ID_;                                                            \
        proto->ParseFromArray( data, size );                                                                           \
        return proto;                                                                                                  \
    } break;

#define PKG_CREATE_GE( _ID_ )                                                                                          \
    case GE_##_ID_: {                                                                                                  \
        CMsg##_ID_* proto = new CMsg##_ID_;                                                                            \
        proto->ParseFromArray( data, size );                                                                           \
        return proto;                                                                                                  \
    } break;

#define PKG_CREATE_DUM( _ID_ )                                                                                         \
    case DOTA_UM_##_ID_: {                                                                                             \
        CDOTAUserMsg_##_ID_* proto = new CDOTAUserMsg_##_ID_;                                                          \
        proto->ParseFromArray( data, size );                                                                           \
        return proto;                                                                                                  \
    } break;

// dispatch macros

#define PKG_DISPATCH( _ID_, _NAME_ )                                                                                   \
    case _ID_: {                                                                                                       \
        if ( has_on_packet<V, _NAME_*>::value ) {                                                                      \
            b.readBytes( data, size );                                                                                 \
            _NAME_* proto = new _NAME_;                                                                                \
            proto->ParseFromArray( data, size );                                                                       \
            v->on_packet( proto );                                                                                     \
            return true;                                                                                               \
        }                                                                                                              \
    } break;

#define PKG_DISPATCH_NET( _ID_ )                                                                                       \
    case net_##_ID_: {                                                                                                 \
        if ( has_on_packet<V, CNETMsg_##_ID_*>::value ) {                                                              \
            b.readBytes( data, size );                                                                                 \
            CNETMsg_##_ID_* proto = new CNETMsg_##_ID_;                                                                \
            proto->ParseFromArray( data, size );                                                                       \
            v->on_packet( proto );                                                                                     \
            return true;                                                                                               \
        }                                                                                                              \
    } break;

#define PKG_DISPATCH_SVC( _ID_ )                                                                                       \
    case svc_##_ID_: {                                                                                                 \
        if ( has_on_packet<V, CSVCMsg_##_ID_*>::value ) {                                                              \
            b.readBytes( data, size );                                                                                 \
            CSVCMsg_##_ID_* proto = new CSVCMsg_##_ID_;                                                                \
            proto->ParseFromArray( data, size );                                                                       \
            v->on_packet( proto );                                                                                     \
            return true;                                                                                               \
        }                                                                                                              \
    } break;

#define PKG_DISPATCH_UM( _ID_ )                                                                                        \
    case UM_##_ID_: {                                                                                                  \
        if ( has_on_packet<V, CUserMessage##_ID_*>::value ) {                                                          \
            b.readBytes( data, size );                                                                                 \
            CUserMessage##_ID_* proto = new CUserMessage##_ID_;                                                        \
            proto->ParseFromArray( data, size );                                                                       \
            v->on_packet( proto );                                                                                     \
            return true;                                                                                               \
        }                                                                                                              \
    } break;

#define PKG_DISPATCH_GE( _ID_ )                                                                                        \
    case GE_##_ID_: {                                                                                                  \
        if ( has_on_packet<V, CMsg##_ID_*>::value ) {                                                                  \
            b.readBytes( data, size );                                                                                 \
            CMsg##_ID_* proto = new CMsg##_ID_;                                                                        \
            proto->ParseFromArray( data, size );                                                                       \
            v->on_packet( proto );                                                                                     \
            return true;                                                                                               \
        }                                                                                                              \
    } break;

#define PKG_DISPATCH_DUM( _ID_ )                                                                                       \
    case DOTA_UM_##_ID_: {                                                                                             \
        if ( has_on_packet<V, CDOTAUserMsg_##_ID_*>::value ) {                                                         \
            b.readBytes( data, size );                                                                                 \
            CDOTAUserMsg_##_ID_* proto = new CDOTAUserMsg_##_ID_;                                                      \
            proto->ParseFromArray( data, size );                                                                       \
            v->on_packet( proto );                                                                                     \
            return true;                                                                                               \
        }                                                                                                              \
    } break;

#include <butterfly/proto/dota_usermessages.pb.h>
#include <butterfly/proto/networkbasetypes.pb.h>
#include <butterfly/proto/netmessages.pb.h>
#include <butterfly/proto/gameevents.pb.h>
#include <butterfly/proto/usermessages.pb.h>
#include <butterfly/proto/dota_gcmessages_common.pb.h>

#include <butterfly/util_bitstream.hpp>

namespace butterfly {
    /** Verify that an overload for a given message exists */
    template <typename V, typename T>
    class has_on_packet {
    private:
        typedef std::true_type yes;
        typedef std::false_type no;

        template <typename U, void ( U::* )( T )>
        struct Check;
        template <typename U>
        static yes valid( Check<U, &U::on_packet>* );
        template <typename U>
        static no valid( ... );

    public:
        static constexpr bool value = std::is_same<decltype( valid<V>( 0 ) ), yes>::value;
    };

    /** Return protobuf message from packet */
    inline google::protobuf::Message* parse_proto( uint32_t id, const char* data, uint32_t size ) {
        switch ( id ) {
            // from networkbasetypes.h
            PKG_CREATE_NET( NOP );
            PKG_CREATE_NET( Disconnect );
            PKG_CREATE_NET( SplitScreenUser );
            PKG_CREATE_NET( Tick );
            PKG_CREATE_NET( StringCmd );
            PKG_CREATE_NET( SetConVar );
            PKG_CREATE_NET( SignonState );
            PKG_CREATE_NET( SpawnGroup_Load );
            PKG_CREATE_NET( SpawnGroup_ManifestUpdate );
            PKG_CREATE_NET( SpawnGroup_SetCreationTick );
            PKG_CREATE_NET( SpawnGroup_Unload );
            PKG_CREATE_NET( SpawnGroup_LoadCompleted );

            // from netmessages.h
            PKG_CREATE_SVC( ServerInfo );
            PKG_CREATE_SVC( FlattenedSerializer );
            PKG_CREATE_SVC( ClassInfo );
            PKG_CREATE_SVC( SetPause );
            PKG_CREATE_SVC( CreateStringTable );
            PKG_CREATE_SVC( UpdateStringTable );
            PKG_CREATE_SVC( VoiceInit );
            PKG_CREATE_SVC( VoiceData );
            PKG_CREATE_SVC( Print );
            PKG_CREATE_SVC( Sounds );
            PKG_CREATE_SVC( SetView );
            PKG_CREATE_SVC( ClearAllStringTables );
            PKG_CREATE_SVC( CmdKeyValues );
            PKG_CREATE_SVC( BSPDecal );
            PKG_CREATE_SVC( SplitScreen );
            PKG_CREATE_SVC( PacketEntities );
            PKG_CREATE_SVC( Prefetch );
            PKG_CREATE_SVC( Menu );
            PKG_CREATE_SVC( GetCvarValue );
            PKG_CREATE_SVC( StopSound );
            PKG_CREATE_SVC( PeerList );
            PKG_CREATE_SVC( PacketReliable );
            PKG_CREATE_SVC( HLTVStatus );
            PKG_CREATE_SVC( FullFrameSplit );

            // from usermessages.h
            PKG_CREATE_UM( AchievementEvent );
            PKG_CREATE_UM( CloseCaption );
            PKG_CREATE_UM( CloseCaptionDirect );
            PKG_CREATE_UM( CurrentTimescale );
            PKG_CREATE_UM( DesiredTimescale );
            PKG_CREATE_UM( Fade );
            PKG_CREATE_UM( GameTitle );
            PKG_CREATE_UM( HudMsg );
            PKG_CREATE_UM( HudText );
            PKG_CREATE_UM( ColoredText );
            PKG_CREATE_UM( RequestState );
            PKG_CREATE_UM( ResetHUD );
            PKG_CREATE_UM( Rumble );
            PKG_CREATE_UM( SayText );
            PKG_CREATE_UM( SayText2 );
            PKG_CREATE_UM( SayTextChannel );
            PKG_CREATE_UM( Shake );
            PKG_CREATE_UM( ShakeDir );
            PKG_CREATE_UM( TextMsg );
            PKG_CREATE_UM( ScreenTilt );
            PKG_CREATE_UM( VoiceMask );
            PKG_CREATE_UM( SendAudio );
            PKG_CREATE_UM( ItemPickup );
            PKG_CREATE_UM( AmmoDenied );
            PKG_CREATE_UM( ShowMenu );
            PKG_CREATE_UM( CreditsMsg );
            PKG_CREATE_UM( CloseCaptionPlaceholder );
            PKG_CREATE_UM( CameraTransition );
            PKG_CREATE_UM( AudioParameter );
            case DOTA_UM_ParticleManager:
            PKG_CREATE( UM_ParticleManager, CUserMsg_ParticleManager );
            PKG_CREATE( UM_HudError, CUserMsg_HudError );
            PKG_CREATE( UM_CustomGameEvent, CUserMsg_CustomGameEvent );

            // from gameevents.h
            PKG_CREATE_GE( VDebugGameSessionIDEvent );
            PKG_CREATE_GE( PlaceDecalEvent );
            PKG_CREATE_GE( ClearWorldDecalsEvent );
            PKG_CREATE_GE( ClearEntityDecalsEvent );
            PKG_CREATE_GE( ClearDecalsForSkeletonInstanceEvent );
            PKG_CREATE_GE( Source1LegacyGameEventList );
            PKG_CREATE_GE( Source1LegacyListenEvents );
            PKG_CREATE_GE( Source1LegacyGameEvent );
            PKG_CREATE_GE( SosStartSoundEvent );
            PKG_CREATE_GE( SosStopSoundEvent );
            PKG_CREATE_GE( SosSetSoundEventParams );
            PKG_CREATE_GE( SosSetLibraryStackFields );
            PKG_CREATE_GE( SosStopSoundEventHash );

            // from dota_usermessages.h
            // PKG_FREE_DUM(AddUnitToSelection);
            PKG_CREATE_DUM( AIDebugLine );
            PKG_CREATE_DUM( ChatEvent );
            PKG_CREATE_DUM( CombatHeroPositions );
            // PKG_FREE_DUM(CombatLogData);
            PKG_CREATE_DUM( CreateLinearProjectile );
            PKG_CREATE_DUM( DestroyLinearProjectile );
            PKG_CREATE_DUM( DodgeTrackingProjectiles );
            PKG_CREATE_DUM( GlobalLightColor );
            PKG_CREATE_DUM( GlobalLightDirection );
            PKG_CREATE_DUM( InvalidCommand );
            PKG_CREATE_DUM( LocationPing );
            PKG_CREATE_DUM( MapLine );
            PKG_CREATE_DUM( MiniKillCamInfo );
            PKG_CREATE_DUM( MinimapDebugPoint );
            PKG_CREATE_DUM( MinimapEvent );
            PKG_CREATE_DUM( NevermoreRequiem );
            PKG_CREATE_DUM( OverheadEvent );
            PKG_CREATE_DUM( SetNextAutobuyItem );
            PKG_CREATE_DUM( SharedCooldown );
            PKG_CREATE_DUM( SpectatorPlayerClick );
            PKG_CREATE_DUM( TutorialTipInfo );
            PKG_CREATE_DUM( UnitEvent );
            PKG_CREATE_DUM( BotChat );
            PKG_CREATE_DUM( HudError );
            PKG_CREATE_DUM( ItemPurchased );
            PKG_CREATE_DUM( Ping );
            PKG_CREATE_DUM( ItemFound );
            // PKG_FREE_DUM(CharacterSpeakConcept);
            PKG_CREATE_DUM( SwapVerify );
            PKG_CREATE_DUM( WorldLine );
            // PKG_FREE_DUM(TournamentDrop);
            PKG_CREATE_DUM( ItemAlert );
            PKG_CREATE_DUM( HalloweenDrops );
            PKG_CREATE_DUM( ChatWheel );
            PKG_CREATE_DUM( ReceivedXmasGift );
            PKG_CREATE_DUM( UpdateSharedContent );
            PKG_CREATE_DUM( TutorialRequestExp );
            PKG_CREATE_DUM( TutorialPingMinimap );
            PKG_CREATE_DUM( GamerulesStateChanged );
            PKG_CREATE_DUM( ShowSurvey );
            PKG_CREATE_DUM( TutorialFade );
            PKG_CREATE_DUM( AddQuestLogEntry );
            PKG_CREATE_DUM( SendStatPopup );
            PKG_CREATE_DUM( TutorialFinish );
            PKG_CREATE_DUM( SendRoshanPopup );
            PKG_CREATE_DUM( SendGenericToolTip );
            PKG_CREATE_DUM( SendFinalGold );
            PKG_CREATE_DUM( CustomMsg );
            PKG_CREATE_DUM( CoachHUDPing );
            PKG_CREATE_DUM( ClientLoadGridNav );
            PKG_CREATE_DUM( TE_Projectile );
            PKG_CREATE_DUM( TE_ProjectileLoc );
            PKG_CREATE_DUM( TE_DotaBloodImpact );
            PKG_CREATE_DUM( TE_UnitAnimation );
            PKG_CREATE_DUM( TE_UnitAnimationEnd );
            PKG_CREATE_DUM( AbilityPing );
            PKG_CREATE_DUM( ShowGenericPopup );
            PKG_CREATE_DUM( VoteStart );
            PKG_CREATE_DUM( VoteUpdate );
            PKG_CREATE_DUM( VoteEnd );
            PKG_CREATE_DUM( BoosterState );
            PKG_CREATE_DUM( WillPurchaseAlert );
            PKG_CREATE_DUM( TutorialMinimapPosition );
            PKG_CREATE_DUM( PlayerMMR );
            PKG_CREATE_DUM( AbilitySteal );
            PKG_CREATE_DUM( CourierKilledAlert );
            PKG_CREATE_DUM( EnemyItemAlert );
            PKG_CREATE_DUM( StatsMatchDetails );
            PKG_CREATE_DUM( MiniTaunt );
            PKG_CREATE_DUM( BuyBackStateAlert );
            PKG_CREATE_DUM( SpeechBubble );
            PKG_CREATE_DUM( CustomHeaderMessage );
            PKG_CREATE_DUM( QuickBuyAlert );
            // PKG_FREE_DUM(StatsHeroDetails);
            PKG_CREATE_DUM( PredictionResult );
            PKG_CREATE_DUM( ModifierAlert );
            PKG_CREATE_DUM( HPManaAlert );
            PKG_CREATE_DUM( GlyphAlert );
            PKG_CREATE_DUM( BeastChat );
            PKG_CREATE_DUM( SpectatorPlayerUnitOrders );
            PKG_CREATE_DUM( CustomHudElement_Create );
            PKG_CREATE_DUM( CustomHudElement_Modify );
            PKG_CREATE_DUM( CustomHudElement_Destroy );
            PKG_CREATE_DUM( CompendiumState );
            PKG_CREATE_DUM( ProjectionAbility );
            PKG_CREATE_DUM( ProjectionEvent );
            PKG_CREATE( DOTA_UM_CombatLogDataHLTV, CMsgDOTACombatLogEntry);
            PKG_CREATE_DUM( XPAlert );
            // PKG_CREATE_DUM(UpdateQuestProgress);
            // PKG_CREATE_DUM(MatchMetadata);
            PKG_CREATE(DOTA_UM_MatchDetails, CDOTAUserMsg_StatsMatchDetails);
            PKG_CREATE_DUM(QuestStatus);
            PKG_CREATE_DUM(SuggestHeroPick);
            PKG_CREATE_DUM(SuggestHeroRole);
            PKG_CREATE_DUM(KillcamDamageTaken);
            PKG_CREATE_DUM(SelectPenaltyGold);
            PKG_CREATE_DUM(RollDiceResult);
            PKG_CREATE_DUM(FlipCoinResult);

        // Stub packet for unkown types
        default:
            return nullptr;
        }
    }

    /** Return protobuf message from packet */
    template <typename V>
    inline bool dispatch_proto( uint32_t id, char* data, uint32_t size, V* v, bitstream& b ) {
        switch ( id ) {
            // from networkbasetypes.h
            PKG_DISPATCH_NET( NOP );
            PKG_DISPATCH_NET( Disconnect );
            PKG_DISPATCH_NET( SplitScreenUser );
            PKG_DISPATCH_NET( Tick );
            PKG_DISPATCH_NET( StringCmd );
            PKG_DISPATCH_NET( SetConVar );
            PKG_DISPATCH_NET( SignonState );
            PKG_DISPATCH_NET( SpawnGroup_Load );
            PKG_DISPATCH_NET( SpawnGroup_ManifestUpdate );
            PKG_DISPATCH_NET( SpawnGroup_SetCreationTick );
            PKG_DISPATCH_NET( SpawnGroup_Unload );
            PKG_DISPATCH_NET( SpawnGroup_LoadCompleted );

            // from netmessages.h
            PKG_DISPATCH_SVC( ServerInfo );
            PKG_DISPATCH_SVC( FlattenedSerializer );
            PKG_DISPATCH_SVC( ClassInfo );
            PKG_DISPATCH_SVC( SetPause );
            PKG_DISPATCH_SVC( CreateStringTable );
            PKG_DISPATCH_SVC( UpdateStringTable );
            PKG_DISPATCH_SVC( VoiceInit );
            PKG_DISPATCH_SVC( VoiceData );
            PKG_DISPATCH_SVC( Print );
            PKG_DISPATCH_SVC( Sounds );
            PKG_DISPATCH_SVC( SetView );
            PKG_DISPATCH_SVC( ClearAllStringTables );
            PKG_DISPATCH_SVC( CmdKeyValues );
            PKG_DISPATCH_SVC( BSPDecal );
            PKG_DISPATCH_SVC( SplitScreen );
            PKG_DISPATCH_SVC( PacketEntities );
            PKG_DISPATCH_SVC( Prefetch );
            PKG_DISPATCH_SVC( Menu );
            PKG_DISPATCH_SVC( GetCvarValue );
            PKG_DISPATCH_SVC( StopSound );
            PKG_DISPATCH_SVC( PeerList );
            PKG_DISPATCH_SVC( PacketReliable );
            PKG_DISPATCH_SVC( HLTVStatus );
            PKG_DISPATCH_SVC( FullFrameSplit );

            // from usermessages.h
            PKG_DISPATCH_UM( AchievementEvent );
            PKG_DISPATCH_UM( CloseCaption );
            PKG_DISPATCH_UM( CloseCaptionDirect );
            PKG_DISPATCH_UM( CurrentTimescale );
            PKG_DISPATCH_UM( DesiredTimescale );
            PKG_DISPATCH_UM( Fade );
            PKG_DISPATCH_UM( GameTitle );
            PKG_DISPATCH_UM( HudMsg );
            PKG_DISPATCH_UM( HudText );
            PKG_DISPATCH_UM( ColoredText );
            PKG_DISPATCH_UM( RequestState );
            PKG_DISPATCH_UM( ResetHUD );
            PKG_DISPATCH_UM( Rumble );
            PKG_DISPATCH_UM( SayText );
            PKG_DISPATCH_UM( SayText2 );
            PKG_DISPATCH_UM( SayTextChannel );
            PKG_DISPATCH_UM( Shake );
            PKG_DISPATCH_UM( ShakeDir );
            PKG_DISPATCH_UM( TextMsg );
            PKG_DISPATCH_UM( ScreenTilt );
            PKG_DISPATCH_UM( VoiceMask );
            PKG_DISPATCH_UM( SendAudio );
            PKG_DISPATCH_UM( ItemPickup );
            PKG_DISPATCH_UM( AmmoDenied );
            PKG_DISPATCH_UM( ShowMenu );
            PKG_DISPATCH_UM( CreditsMsg );
            PKG_DISPATCH_UM( CloseCaptionPlaceholder );
            PKG_DISPATCH_UM( CameraTransition );
            PKG_DISPATCH_UM( AudioParameter );
            case DOTA_UM_ParticleManager:
            PKG_DISPATCH( UM_ParticleManager, CUserMsg_ParticleManager );
            PKG_DISPATCH( UM_HudError, CUserMsg_HudError );
            PKG_DISPATCH( UM_CustomGameEvent, CUserMsg_CustomGameEvent );

            // from gameevents.h
            PKG_DISPATCH_GE( VDebugGameSessionIDEvent );
            PKG_DISPATCH_GE( PlaceDecalEvent );
            PKG_DISPATCH_GE( ClearWorldDecalsEvent );
            PKG_DISPATCH_GE( ClearEntityDecalsEvent );
            PKG_DISPATCH_GE( ClearDecalsForSkeletonInstanceEvent );
            PKG_DISPATCH_GE( Source1LegacyGameEventList );
            PKG_DISPATCH_GE( Source1LegacyListenEvents );
            PKG_DISPATCH_GE( Source1LegacyGameEvent );
            PKG_DISPATCH_GE( SosStartSoundEvent );
            PKG_DISPATCH_GE( SosStopSoundEvent );
            PKG_DISPATCH_GE( SosSetSoundEventParams );
            PKG_DISPATCH_GE( SosSetLibraryStackFields );
            PKG_DISPATCH_GE( SosStopSoundEventHash );

            // from dota_usermessages.h
            // PKG_FREE_DUM(AddUnitToSelection);
            PKG_DISPATCH_DUM( AIDebugLine );
            PKG_DISPATCH_DUM( ChatEvent );
            PKG_DISPATCH_DUM( CombatHeroPositions );
            // PKG_FREE_DUM(CombatLogData);
            PKG_DISPATCH_DUM( CreateLinearProjectile );
            PKG_DISPATCH_DUM( DestroyLinearProjectile );
            PKG_DISPATCH_DUM( DodgeTrackingProjectiles );
            PKG_DISPATCH_DUM( GlobalLightColor );
            PKG_DISPATCH_DUM( GlobalLightDirection );
            PKG_DISPATCH_DUM( InvalidCommand );
            PKG_DISPATCH_DUM( LocationPing );
            PKG_DISPATCH_DUM( MapLine );
            PKG_DISPATCH_DUM( MiniKillCamInfo );
            PKG_DISPATCH_DUM( MinimapDebugPoint );
            PKG_DISPATCH_DUM( MinimapEvent );
            PKG_DISPATCH_DUM( NevermoreRequiem );
            PKG_DISPATCH_DUM( OverheadEvent );
            PKG_DISPATCH_DUM( SetNextAutobuyItem );
            PKG_DISPATCH_DUM( SharedCooldown );
            PKG_DISPATCH_DUM( SpectatorPlayerClick );
            PKG_DISPATCH_DUM( TutorialTipInfo );
            PKG_DISPATCH_DUM( UnitEvent );
            PKG_DISPATCH_DUM( BotChat );
            PKG_DISPATCH_DUM( HudError );
            PKG_DISPATCH_DUM( ItemPurchased );
            PKG_DISPATCH_DUM( Ping );
            PKG_DISPATCH_DUM( ItemFound );
            // PKG_FREE_DUM(CharacterSpeakConcept);
            PKG_DISPATCH_DUM( SwapVerify );
            PKG_DISPATCH_DUM( WorldLine );
            // PKG_FREE_DUM(TournamentDrop);
            PKG_DISPATCH_DUM( ItemAlert );
            PKG_DISPATCH_DUM( HalloweenDrops );
            PKG_DISPATCH_DUM( ChatWheel );
            PKG_DISPATCH_DUM( ReceivedXmasGift );
            PKG_DISPATCH_DUM( UpdateSharedContent );
            PKG_DISPATCH_DUM( TutorialRequestExp );
            PKG_DISPATCH_DUM( TutorialPingMinimap );
            PKG_DISPATCH_DUM( GamerulesStateChanged );
            PKG_DISPATCH_DUM( ShowSurvey );
            PKG_DISPATCH_DUM( TutorialFade );
            PKG_DISPATCH_DUM( AddQuestLogEntry );
            PKG_DISPATCH_DUM( SendStatPopup );
            PKG_DISPATCH_DUM( TutorialFinish );
            PKG_DISPATCH_DUM( SendRoshanPopup );
            PKG_DISPATCH_DUM( SendGenericToolTip );
            PKG_DISPATCH_DUM( SendFinalGold );
            PKG_DISPATCH_DUM( CustomMsg );
            PKG_DISPATCH_DUM( CoachHUDPing );
            PKG_DISPATCH_DUM( ClientLoadGridNav );
            PKG_DISPATCH_DUM( TE_Projectile );
            PKG_DISPATCH_DUM( TE_ProjectileLoc );
            PKG_DISPATCH_DUM( TE_DotaBloodImpact );
            PKG_DISPATCH_DUM( TE_UnitAnimation );
            PKG_DISPATCH_DUM( TE_UnitAnimationEnd );
            PKG_DISPATCH_DUM( AbilityPing );
            PKG_DISPATCH_DUM( ShowGenericPopup );
            PKG_DISPATCH_DUM( VoteStart );
            PKG_DISPATCH_DUM( VoteUpdate );
            PKG_DISPATCH_DUM( VoteEnd );
            PKG_DISPATCH_DUM( BoosterState );
            PKG_DISPATCH_DUM( WillPurchaseAlert );
            PKG_DISPATCH_DUM( TutorialMinimapPosition );
            PKG_DISPATCH_DUM( PlayerMMR );
            PKG_DISPATCH_DUM( AbilitySteal );
            PKG_DISPATCH_DUM( CourierKilledAlert );
            PKG_DISPATCH_DUM( EnemyItemAlert );
            PKG_DISPATCH_DUM( StatsMatchDetails );
            PKG_DISPATCH_DUM( MiniTaunt );
            PKG_DISPATCH_DUM( BuyBackStateAlert );
            PKG_DISPATCH_DUM( SpeechBubble );
            PKG_DISPATCH_DUM( CustomHeaderMessage );
            PKG_DISPATCH_DUM( QuickBuyAlert );
            // PKG_FREE_DUM(StatsHeroDetails);
            PKG_DISPATCH_DUM( PredictionResult );
            PKG_DISPATCH_DUM( ModifierAlert );
            PKG_DISPATCH_DUM( HPManaAlert );
            PKG_DISPATCH_DUM( GlyphAlert );
            PKG_DISPATCH_DUM( BeastChat );
            PKG_DISPATCH_DUM( SpectatorPlayerUnitOrders );
            PKG_DISPATCH_DUM( CustomHudElement_Create );
            PKG_DISPATCH_DUM( CustomHudElement_Modify );
            PKG_DISPATCH_DUM( CustomHudElement_Destroy );
            PKG_DISPATCH_DUM( CompendiumState );
            PKG_DISPATCH_DUM( ProjectionAbility );
            PKG_DISPATCH_DUM( ProjectionEvent );
            // PKG_CREATE_DUM(CombatLogDataHLTV);
            PKG_DISPATCH_DUM( XPAlert );
            // PKG_CREATE_DUM(UpdateQuestProgress);
            // PKG_CREATE_DUM(MatchMetadata);
        }

        return false;
    }
} /* butterfly */

// clean up
#undef PKG_CREATE
#undef PKG_CREATE_NET
#undef PKG_CREATE_SVC
#undef PKG_CREATE_UM
#undef PKG_CREATE_GE
#undef PKG_CREATE_DUM
#undef PKG_DISPATCH
#undef PKG_DISPATCH_NET
#undef PKG_DISPATCH_SVC
#undef PKG_DISPATCH_UM
#undef PKG_DISPATCH_GE
#undef PKG_DISPATCH_DUM

#endif /* BUTTERFLY_PACKETS_HPP */
