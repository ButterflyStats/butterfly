/**
 * @file protobuf.hpp
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
 *
 * @par Description
 *    Javascript protobuf abstraction.
 */

#ifndef BUTTERFLYJS_PROTOBUF_HPP
#define BUTTERFLYJS_PROTOBUF_HPP

#include <string>

#include <emscripten.h>
#include <emscripten/bind.h>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "base64.hpp"

namespace butterfly {
    using namespace google::protobuf;
    using namespace emscripten;

    // forward decl for parse_msg
    static val parse_msg(const Message *msg);

    /** Convert a protobuf field into it's emscripten value */
    static val field2val(const Message *msg, const FieldDescriptor *field) {
        const Reflection *ref = msg->GetReflection();
        const bool repeated = field->is_repeated();

        size_t array_size = 0;
        if (repeated) {
           array_size = ref->FieldSize(*msg, field);
        }

        val v = val::object();

        if (repeated) {
            v = val::array();
        }

        switch (field->cpp_type()) {
        case FieldDescriptor::CPPTYPE_DOUBLE:
            if (repeated) {
               for (size_t i = 0; i != array_size; ++i) {
                   v.set(i, ref->GetRepeatedDouble(*msg, field, i));
               }
            } else {
               v = val(ref->GetDouble(*msg, field));
            }
            break;
        case FieldDescriptor::CPPTYPE_FLOAT:
            if (repeated) {
               for (size_t i = 0; i != array_size; ++i) {
                   v.set(i, ref->GetRepeatedFloat(*msg, field, i));
               }
            } else {
               v = val(ref->GetFloat(*msg, field));
            }
            break;
        case FieldDescriptor::CPPTYPE_INT64:
            if (repeated) {
               for (size_t i = 0; i != array_size; ++i) {
                   v.set(i, ref->GetRepeatedInt64(*msg, field, i));
               }
            } else {
               v = val(ref->GetInt64(*msg, field));
            }
            break;
        case FieldDescriptor::CPPTYPE_UINT64:
            if (repeated) {
               for (size_t i = 0; i != array_size; ++i) {
                   v.set(i, ref->GetRepeatedUInt64(*msg, field, i));
               }
            } else {
               v = val(ref->GetUInt64(*msg, field));
            }
            break;
        case FieldDescriptor::CPPTYPE_INT32:
            if (repeated) {
               for (size_t i = 0; i != array_size; ++i) {
                   v.set(i, ref->GetRepeatedInt32(*msg, field, i));
               }
            } else {
               v = val(ref->GetInt32(*msg, field));
            }
            break;
        case FieldDescriptor::CPPTYPE_UINT32:
            if (repeated) {
               for (size_t i = 0; i != array_size; ++i) {
                   v.set(i, ref->GetRepeatedUInt32(*msg, field, i));
               }
            } else {
               v = val(ref->GetUInt32(*msg, field));
            }
            break;
        case FieldDescriptor::CPPTYPE_BOOL:
            if (repeated) {
               for (size_t i = 0; i != array_size; ++i) {
                   v.set(i, ref->GetRepeatedBool(*msg, field, i) ? "True" : "False");
               }
            } else {
               v = val(ref->GetBool(*msg, field) ? "True" : "False");
            }
            break;
        case FieldDescriptor::CPPTYPE_STRING: {
            bool is_binary = field->type() == FieldDescriptor::TYPE_BYTES;
            if (repeated) {
               for (size_t i = 0; i != array_size; ++i) {
                   std::string value = ref->GetRepeatedString(*msg, field, i);
                   if (is_binary) {
                       value = base64_encode(value);
                   }

                   v.set(i, value);
               }
            } else {
               std::string value = ref->GetString(*msg, field);
               if (is_binary) {
                   value = base64_encode(value);
               }

               v = val(value);
            }
            } break;
        case FieldDescriptor::CPPTYPE_MESSAGE:
            if (repeated) {
               for (size_t i = 0; i != array_size; ++i) {
                   const Message *value = &(ref->GetRepeatedMessage(*msg, field, i));
                   v.set(i, parse_msg(value));
               }
            } else {
                const Message *value = &(ref->GetMessage(*msg, field));
                v = parse_msg(value);
            }
            break;
        case FieldDescriptor::CPPTYPE_ENUM:
            if (repeated) {
               for (size_t i = 0; i != array_size; ++i) {
                   v.set(i, ref->GetRepeatedEnum(*msg, field, i)->number());
               }
            } else {
               v = val(ref->GetEnum(*msg, field)->number());
            }
            break;
        default:
           break;
        }

        return v;
    }

    /** Parse a top level message */
    static val parse_msg(const Message *msg) {
        val v = val::object();
        const Descriptor *d = msg->GetDescriptor();

        if (!d) {
            return v;
        }

        size_t count = d->field_count();

        for (size_t i = 0; i != count; ++i) {
            const FieldDescriptor *field = d->field(i);
            const Reflection *ref = msg->GetReflection();

            if (field->is_optional() && !ref->HasField(*msg, field)) {
                //do nothing
            } else {
                v.set(field->name(), field2val(msg, field));
            }
        }

        return v;
    }

    /** Convert protobuf to emscripten val */
    val pb2js(const Message* msg) {
        return parse_msg(msg);
    }
}

#endif /* BUTTERFLYJS_PROTOBUF_HPP */
