#define MATCH_VECTOR(hash_) \
case hash_: \
    ret->is_array = true; \
    break;

// dynamic lists
MATCH_VECTOR("m_SpeechBubbles"_chash)
MATCH_VECTOR("CDOTA_CombatLogQueryProgress"_chash)

#undef MATCH_VECTOR

#define MATCH_SIMPLE(hash_, decoder_, type_) \
case hash_: \
    ret->decoder_type = decoder_; \
    ret_info->type = type_; \
    break;

// string
MATCH_SIMPLE("CUtlString"_chash, PropertyDecoderType::STRING, PropertyType::STRING)
MATCH_SIMPLE("CUtlSymbolLarge"_chash, PropertyDecoderType::STRING, PropertyType::STRING)

// vectors
MATCH_SIMPLE("Quaternion"_chash, PropertyDecoderType::QUATERNION, PropertyType::QUATERNION)
MATCH_SIMPLE("Vector4D"_chash, PropertyDecoderType::QUATERNION, PropertyType::QUATERNION)
MATCH_SIMPLE("Vector"_chash, PropertyDecoderType::VECTOR, PropertyType::VECTOR3)
MATCH_SIMPLE("Vector2D"_chash, PropertyDecoderType::VECTOR2, PropertyType::VECTOR2)
MATCH_SIMPLE("QAngle"_chash, PropertyDecoderType::QANGLE, PropertyType::VECTOR3)

// varints
MATCH_SIMPLE("CEntityHandle"_chash, PropertyDecoderType::VARUINT, PropertyType::UINT32)
MATCH_SIMPLE("CGameSceneNodeHandle"_chash, PropertyDecoderType::VARUINT, PropertyType::UINT64)
MATCH_SIMPLE("HSequence"_chash, PropertyDecoderType::VARUINT, PropertyType::UINT32)
MATCH_SIMPLE("CUtlStringToken"_chash, PropertyDecoderType::VARUINT, PropertyType::UINT32)
MATCH_SIMPLE("Color"_chash, PropertyDecoderType::VARUINT, PropertyType::UINT32)
MATCH_SIMPLE("color32"_chash, PropertyDecoderType::VARUINT, PropertyType::UINT32)

// raw float
MATCH_SIMPLE("GameTime_t"_chash, PropertyDecoderType::FLOAT_NO_DECODER, PropertyType::FLOAT)

// quantized float
MATCH_SIMPLE("CNetworkedQuantizedFloat"_chash, PropertyDecoderType::FLOAT, PropertyType::FLOAT)

// basic types
MATCH_SIMPLE("bool"_chash, PropertyDecoderType::BOOL, PropertyType::BOOL)
MATCH_SIMPLE("int8"_chash, PropertyDecoderType::VARINT, PropertyType::INT32)
MATCH_SIMPLE("uint8"_chash, PropertyDecoderType::VARUINT, PropertyType::UINT32)
MATCH_SIMPLE("int16"_chash, PropertyDecoderType::VARINT, PropertyType::INT32)
MATCH_SIMPLE("uint16"_chash, PropertyDecoderType::VARUINT, PropertyType::UINT32)
MATCH_SIMPLE("int32"_chash, PropertyDecoderType::VARINT, PropertyType::INT32)
MATCH_SIMPLE("uint32"_chash, PropertyDecoderType::VARUINT, PropertyType::UINT32)
MATCH_SIMPLE("int64"_chash, PropertyDecoderType::VARINT, PropertyType::INT64)
MATCH_SIMPLE("uint64"_chash, PropertyDecoderType::VARUINT, PropertyType::UINT64)
MATCH_SIMPLE("float32"_chash, PropertyDecoderType::FLOAT, PropertyType::FLOAT)

#define MATCH_POINTER(hash_) MATCH_SIMPLE(hash_, PropertyDecoderType::TABLE_PTR, PropertyType::BOOL)

// pointer types
MATCH_POINTER("CBodyComponent"_chash)
MATCH_POINTER("CLightComponent"_chash)
MATCH_POINTER("CPhysicsComponent"_chash)
MATCH_POINTER("CRenderComponent"_chash)
MATCH_POINTER("CPlayerLocalData"_chash)

#undef MATCH_POINTER
#undef MATCH_SIMPLE
