#pragma once
#include 

#define BEGIN_BIND_FROM_JSON(type) \
void from_json(const nlohmann::json& json, type& dt) { 

#define ADD_BINDING_FROM_RULE(MemberName, MemberInstance) json.at(MemberName).get_to(MemberInstance);

#define END_BIND_FROM_JSON }

#define BEGIN_BIND_TO_JSON(type) \
void to_json(nlohmann::json& json, const type& dt) { \
j = {

#define ADD_BINDING_TO_RULE(MemberName, MemberInstance) {MemberName, MemberInstance},

#define END_BIND_TO_JSON };}