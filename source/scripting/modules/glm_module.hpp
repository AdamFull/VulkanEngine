#pragma once
#include "chaiscript/chaiscript.hpp"

template<class _Ty>
struct is_not_vec : std::bool_constant<
!std::is_same_v<_Ty, glm::vec1> && 
!std::is_same_v<_Ty, glm::vec2> &&
!std::is_same_v<_Ty, glm::vec3> &&
!std::is_same_v<_Ty, glm::vec4>>
{};

template<class _Ty>
struct is_not_non_standart_matrix : std::bool_constant<
!std::is_same_v<_Ty, glm::mat2x3> &&
!std::is_same_v<_Ty, glm::mat3x2> && 
!std::is_same_v<_Ty, glm::mat2x4> && 
!std::is_same_v<_Ty, glm::mat4x2> && 
!std::is_same_v<_Ty, glm::mat3x4> && 
!std::is_same_v<_Ty, glm::mat4x3>>
{};

template<class _Ty>
struct is_not_four_cmp_vec : std::bool_constant<
!std::is_same_v<_Ty, glm::vec4> && 
!std::is_same_v<_Ty, glm::mat4> &&
!std::is_same_v<_Ty, glm::mat4x3> &&
!std::is_same_v<_Ty, glm::mat4x2>>
{};

template<class _Ty>
struct is_not_matrix : std::bool_constant<
!std::is_same_v<_Ty, glm::mat2> && 
!std::is_same_v<_Ty, glm::mat3> &&
!std::is_same_v<_Ty, glm::mat4> &&
!std::is_same_v<_Ty, glm::mat2x3> &&
!std::is_same_v<_Ty, glm::mat3x2> && 
!std::is_same_v<_Ty, glm::mat2x4> && 
!std::is_same_v<_Ty, glm::mat4x2> && 
!std::is_same_v<_Ty, glm::mat3x4> && 
!std::is_same_v<_Ty, glm::mat4x3>>
{};

template<class _Ty>
constexpr void register_same(const std::string& name, chaiscript::ModulePtr& pModule)
{
    pModule->add(chaiscript::user_type<_Ty>(), name);
    pModule->add(chaiscript::constructor<_Ty()>(), name);

    if constexpr(!std::is_same_v<_Ty, glm::quat>)
    {
        pModule->add(chaiscript::constructor<_Ty(float)>(), name);

        if constexpr(is_not_four_cmp_vec<_Ty>::value)
        {
            pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, float)>(glm::operator*)), "*");
            pModule->add(chaiscript::fun(static_cast<_Ty (*)(float, const _Ty&)>(glm::operator*)), "*");
            pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, float)>(glm::operator/)), "/");
            pModule->add(chaiscript::fun(static_cast<_Ty (*)(float, const _Ty&)>(glm::operator/)), "/");

            if constexpr(is_not_non_standart_matrix<_Ty>::value)
            {
                pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, float)>(glm::operator+)), "+");
                pModule->add(chaiscript::fun(static_cast<_Ty (*)(float, const _Ty&)>(glm::operator+)), "+");
                pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&)>(glm::operator/)), "/");
            }
            
            if constexpr(is_not_matrix<_Ty>::value)
            {
                pModule->add(chaiscript::fun(static_cast<_Ty (*)(const glm::vec1&, const _Ty&)>(glm::operator+)), "+");
                pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const glm::vec1&)>(glm::operator+)), "+");
                pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const glm::vec1&)>(glm::operator*)), "*");
                pModule->add(chaiscript::fun(static_cast<_Ty (*)(const glm::vec1&, const _Ty&)>(glm::operator*)), "*");
                pModule->add(chaiscript::fun(static_cast<_Ty (*)(const glm::vec1&, const _Ty&)>(glm::operator/)), "/");
                pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const glm::vec1&)>(glm::operator/)), "/");
                //pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, float)>(glm::operator%)), "%");
                //pModule->add(chaiscript::fun(static_cast<_Ty (*)(float, const _Ty&)>(glm::operator%)), "%");
                //pModule->add(chaiscript::fun(static_cast<_Ty (*)(const glm::vec1&, const _Ty&)>(glm::operator%)), "%");
                //pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const glm::vec1&)>(glm::operator%)), "%");
            }
        }
    }

    if constexpr(is_not_non_standart_matrix<_Ty>::value)
    {
        pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&)>(glm::operator*)), "*");
    }

    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&)>(glm::operator+)), "+");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::operator+)), "+");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&)>(glm::operator-)), "-");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::operator-)), "-");
    pModule->add(chaiscript::fun(static_cast<bool (*)(const _Ty&, const _Ty&)>(glm::operator==)), "==");
    pModule->add(chaiscript::fun(static_cast<bool (*)(const _Ty&, const _Ty&)>(glm::operator==)), "!=");
    pModule->add(chaiscript::fun([](const _Ty& t, const int& index){ return t[index]; }), "[]");
    //pModule->add(chaiscript::fun(static_cast<_Ty& (*)(glm::length_t)>(_Ty::operator[])), "[]");
    //pModule->add(chaiscript::fun(static_cast<const _Ty& (*)(glm::length_t) const>(_Ty::operator[])), "[]");
}

template<class _Ty>
constexpr void register_vector(const std::string& name, chaiscript::ModulePtr& pModule)
{
    register_same<_Ty>(name, pModule);
    pModule->add(chaiscript::constructor<_Ty(const _Ty&)>(), name);

    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&)>(glm::pow)), "pow");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::exp)), "exp");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::log)), "log");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::exp2)), "exp2");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::log2)), "log2");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::sqrt)), "sqrt");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::inversesqrt)), "inversesqrt");

    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::abs)), "abs");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::sign)), "sign");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::floor)), "floor");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::trunc)), "trunc");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::round)), "round");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::roundEven)), "roundEven");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::ceil)), "ceil");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::fract)), "fract");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, float)>(glm::mod)), "mod");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&)>(glm::mod)), "mod");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, _Ty&)>(glm::modf)), "modf");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, float)>(glm::min)), "min");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&)>(glm::min)), "min");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, float)>(glm::max)), "max");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&)>(glm::max)), "max");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, float, float)>(glm::clamp)), "clamp");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&, const _Ty&)>(glm::clamp)), "clamp");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&, float)>(glm::mix)), "mix");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&, const _Ty&)>(glm::mix)), "mix");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(float, const _Ty&)>(glm::step)), "step");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&)>(glm::step)), "step");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(float, float, const _Ty&)>(glm::smoothstep)), "smoothstep");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&, const _Ty&)>(glm::smoothstep)), "smoothstep");
    //pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::isnan)), "isnan");
    //pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::isinf)), "isinf");
    //pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&)>(glm::frexp)), "frexp");
    //pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&)>(glm::ldexp)), "ldexp");

    pModule->add(chaiscript::fun(static_cast<float (*)(const _Ty&)>(glm::length)), "length");
    pModule->add(chaiscript::fun(static_cast<float (*)(const _Ty&, const _Ty&)>(glm::distance)), "distance");
    pModule->add(chaiscript::fun(static_cast<float (*)(const _Ty&, const _Ty&)>(glm::dot)), "dot");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::normalize)), "normalize");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&, const _Ty&)>(glm::faceforward)), "faceforward");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&)>(glm::reflect)), "reflect");
    pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&, const _Ty&, float)>(glm::refract)), "refract");

    if constexpr(std::is_same_v<_Ty, glm::vec3>)
        pModule->add(chaiscript::fun(static_cast<glm::vec3 (*)(const glm::vec3&, const glm::vec3&)>(glm::cross)), "cross");
}

template<class _Ty>
constexpr void register_matrix(const std::string& name, chaiscript::ModulePtr& pModule)
{
    register_same<_Ty>(name, pModule);
    pModule->add(chaiscript::constructor<_Ty(const glm::mat2&)>(), name);
    pModule->add(chaiscript::constructor<_Ty(const glm::mat3&)>(), name);
    pModule->add(chaiscript::constructor<_Ty(const glm::mat4&)>(), name);
    pModule->add(chaiscript::constructor<_Ty(const glm::mat2x3&)>(), name);
    pModule->add(chaiscript::constructor<_Ty(const glm::mat3x2&)>(), name);
    pModule->add(chaiscript::constructor<_Ty(const glm::mat2x4&)>(), name);
    pModule->add(chaiscript::constructor<_Ty(const glm::mat4x2&)>(), name);
    pModule->add(chaiscript::constructor<_Ty(const glm::mat3x4&)>(), name);
    pModule->add(chaiscript::constructor<_Ty(const glm::mat4x3&)>(), name);

    if constexpr(is_not_non_standart_matrix<_Ty>::value)
    {
        pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::transpose)), "transpose");
        pModule->add(chaiscript::fun(static_cast<float (*)(const _Ty&)>(glm::determinant)), "determinant");
        pModule->add(chaiscript::fun(static_cast<_Ty (*)(const _Ty&)>(glm::inverse)), "inverse");
    }
}

namespace engine
{
    namespace scripting
    {
        namespace modules
        {
            static chaiscript::ModulePtr glm()
            {
                auto pModule = chaiscript::ModulePtr(new chaiscript::Module());

                //scalar
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::log2)), "log2");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::inversesqrt)), "inversesqrt");
            
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::abs)), "abs");
                pModule->add(chaiscript::fun(static_cast<float (*)(float, float)>(glm::mod)), "mod"); 
                pModule->add(chaiscript::fun(static_cast<float (*)(float, float)>(glm::max)), "max");
                pModule->add(chaiscript::fun(static_cast<float (*)(float, float)>(glm::min)), "min");
                pModule->add(chaiscript::fun(static_cast<float (*)(float, float, float)>(glm::mix)), "mix");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::sign)), "sign");
                pModule->add(chaiscript::fun(static_cast<float (*)(float, float, float)>(glm::clamp)), "clamp");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::radians)), "radians");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::degrees)), "degrees");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::fract)), "fract");
                pModule->add(chaiscript::fun(static_cast<float (*)(float, float&)>(glm::modf)), "modf");
                pModule->add(chaiscript::fun(static_cast<float (*)(float, int&)>(glm::frexp)), "frexp");
                pModule->add(chaiscript::fun(static_cast<float (*)(const float&, const int&)>(glm::ldexp)), "ldexp"); 
                pModule->add(chaiscript::fun(static_cast<float (*)(float, float)>(glm::step)), "step");
                pModule->add(chaiscript::fun(static_cast<float (*)(float, float, float)>(glm::smoothstep)), "smoothstep");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::trunc)), "trunc");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::sin)), "sin");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::cos)), "cos");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::tan)), "tan");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::asin)), "asin");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::acos)), "acos");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::atan)), "atan");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::ceil)), "ceil");
                pModule->add(chaiscript::fun(static_cast<float (*)(float)>(glm::floor)), "floor");
                pModule->add(chaiscript::fun(&glm::pi<float>), "pi");
                pModule->add(chaiscript::fun(&glm::epsilon<float>), "epsilon");
                pModule->add(chaiscript::fun(&glm::cos_one_over_two<float>), "cos_one_over_two");
                pModule->add(chaiscript::fun(&glm::zero<float>), "zero");
                pModule->add(chaiscript::fun(&glm::one<float>), "one");
                pModule->add(chaiscript::fun(&glm::two_pi<float>), "two_pi");
                pModule->add(chaiscript::fun(&glm::root_pi<float>), "root_pi");
                pModule->add(chaiscript::fun(&glm::half_pi<float>), "half_pi");
                pModule->add(chaiscript::fun(&glm::three_over_two_pi<float>), "three_over_two_pi");
                pModule->add(chaiscript::fun(&glm::quarter_pi<float>), "quarter_pi");
                pModule->add(chaiscript::fun(&glm::one_over_pi<float>), "one_over_pi");
                pModule->add(chaiscript::fun(&glm::one_over_two_pi<float>), "one_over_two_pi");
                pModule->add(chaiscript::fun(&glm::two_over_pi<float>), "two_over_pi");
                pModule->add(chaiscript::fun(&glm::four_over_pi<float>), "four_over_pi");
                pModule->add(chaiscript::fun(&glm::two_over_root_pi<float>), "two_over_root_pi");
                pModule->add(chaiscript::fun(&glm::one_over_root_two<float>), "one_over_root_two");
                pModule->add(chaiscript::fun(&glm::root_half_pi<float>), "root_half_pi");
                pModule->add(chaiscript::fun(&glm::root_two_pi<float>), "root_two_pi");
                pModule->add(chaiscript::fun(&glm::root_ln_four<float>), "root_ln_four");
                pModule->add(chaiscript::fun(&glm::e<float>), "e");
                pModule->add(chaiscript::fun(&glm::euler<float>), "euler");
                pModule->add(chaiscript::fun(&glm::root_two<float>), "root_two");
                pModule->add(chaiscript::fun(&glm::root_three<float>), "root_three");
                pModule->add(chaiscript::fun(&glm::root_five<float>), "root_five");
                pModule->add(chaiscript::fun(&glm::ln_two<float>), "ln_two");
                pModule->add(chaiscript::fun(&glm::ln_ten<float>), "ln_ten");
                pModule->add(chaiscript::fun(&glm::ln_ln_two<float>), "ln_ln_two");
                pModule->add(chaiscript::fun(&glm::third<float>), "third");
                pModule->add(chaiscript::fun(&glm::two_thirds<float>), "two_thirds");
                pModule->add(chaiscript::fun(&glm::golden_ratio<float>), "golden_ratio");

                //glm::vec1
                register_vector<glm::vec1>("vec1", pModule);
                pModule->add(chaiscript::fun(&glm::vec1::x), "x");
                pModule->add(chaiscript::fun(
                    [](const glm::vec1& v) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "vec1(" << v.x << ")";
                        return ss.str();
                    }), "to_string");

                //glm::vec2
                register_vector<glm::vec2>("vec2", pModule);
                pModule->add(chaiscript::constructor<glm::vec2(float, float)>(), "vec2");
                pModule->add(chaiscript::fun(&glm::vec2::x), "x");
                pModule->add(chaiscript::fun(&glm::vec2::y), "y");
                
                pModule->add(chaiscript::fun(
                    [](const glm::vec2& v) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "vec2(" << v.x << ", " << v.y << ")";
                        return ss.str();
                    }), "to_string");

                //glm::vec3
                register_vector<glm::vec3>("vec3", pModule);
                pModule->add(chaiscript::constructor<glm::vec3(float, float, float)>(), "vec3");
                pModule->add(chaiscript::constructor<glm::vec3(const glm::vec2&, float)>(), "vec3");
                pModule->add(chaiscript::fun(&glm::vec3::x), "x");
                pModule->add(chaiscript::fun(&glm::vec3::y), "y");
                pModule->add(chaiscript::fun(&glm::vec3::z), "z");
                
                pModule->add(chaiscript::fun(
                    [](const glm::vec3& v) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
                        return ss.str();
                    }), "to_string");

                //glm::vec4
                register_vector<glm::vec4>("vec4", pModule);
                pModule->add(chaiscript::constructor<glm::vec4(float, float, float, float)>(), "vec4");
                pModule->add(chaiscript::constructor<glm::vec4(const glm::vec3&, float)>(), "vec4");
                pModule->add(chaiscript::constructor<glm::vec4(const glm::vec2&, float, float)>(), "vec4");
                pModule->add(chaiscript::fun(&glm::vec4::x), "x");
                pModule->add(chaiscript::fun(&glm::vec4::y), "y");
                pModule->add(chaiscript::fun(&glm::vec4::z), "z");
                pModule->add(chaiscript::fun(&glm::vec4::w), "w");
                
                pModule->add(chaiscript::fun(
                    [](const glm::vec4& v) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
                        return ss.str();
                    }), "to_string");

                //glm::quat
                register_same<glm::quat>("quat", pModule);
                pModule->add(chaiscript::constructor<glm::quat(const glm::vec3&)>(), "quat");
                pModule->add(chaiscript::fun(&glm::quat::w), "w");
                pModule->add(chaiscript::fun(&glm::quat::x), "x");
                pModule->add(chaiscript::fun(&glm::quat::y), "y");
                pModule->add(chaiscript::fun(&glm::quat::z), "z");

                pModule->add(chaiscript::fun(static_cast<glm::quat (*)(const float&, const glm::vec3&)>(glm::angleAxis)), "angleAxis");
                pModule->add(chaiscript::fun(static_cast<glm::vec3 (*)(const glm::quat&)>(glm::eulerAngles)), "eulerAngles");
                pModule->add(chaiscript::fun(static_cast<glm::quat (*)(const glm::quat&)>(glm::conjugate)), "conjugate");
                pModule->add(chaiscript::fun(static_cast<glm::quat (*)(const glm::quat&)>(glm::inverse)), "inverse");
                pModule->add(chaiscript::fun(static_cast<glm::quat (*)(const glm::quat&)>(glm::normalize)), "normalize");
                pModule->add(chaiscript::fun(static_cast<glm::quat (*)(const glm::quat&, const float&, const glm::vec3&)>(glm::rotate)), "rotate");
                glm::vec3(1) * glm::vec3(1);
                pModule->add(chaiscript::fun(
                    [](const glm::quat& q) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "quat(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
                        return ss.str();
                    }), "to_string");

                //Matrix transform
                pModule->add(chaiscript::fun(static_cast<glm::mat4 (*)(const glm::mat4&, const glm::vec3&)>(glm::translate)), "translate");
                pModule->add(chaiscript::fun(static_cast<glm::mat4 (*)(const glm::mat4&, float, const glm::vec3&)>(glm::rotate)), "rotate");
                pModule->add(chaiscript::fun(static_cast<glm::mat4 (*)(const glm::mat4&, float, const glm::vec3&)>(glm::rotate_slow)), "rotate_slow");
                pModule->add(chaiscript::fun(static_cast<glm::mat4 (*)(const glm::mat4&, const glm::vec3&)>(glm::scale)), "scale");
                pModule->add(chaiscript::fun(static_cast<glm::mat4 (*)(const glm::mat4&, const glm::vec3&)>(glm::scale_slow)), "scale_slow");
                pModule->add(chaiscript::fun(static_cast<glm::mat4 (*)(const glm::vec3&, const glm::vec3&, const glm::vec3&)>(glm::lookAtRH)), "lookAtRH");
                pModule->add(chaiscript::fun(static_cast<glm::mat4 (*)(const glm::vec3&, const glm::vec3&, const glm::vec3&)>(glm::lookAtLH)), "lookAtLH");
                pModule->add(chaiscript::fun(static_cast<glm::mat4 (*)(const glm::vec3&, const glm::vec3&, const glm::vec3&)>(glm::lookAt)), "lookAt");

                //glm::mat2
                register_matrix<glm::mat2>("mat2", pModule);
                pModule->add(chaiscript::constructor<glm::mat2(
                    const glm::mat2::value_type&, const glm::mat2::value_type&, 
                    const glm::mat2::value_type&, const glm::mat2::value_type&)>(), "mat2");
                pModule->add(chaiscript::constructor<glm::mat2(
                    const glm::mat2::col_type&, 
                    const glm::mat2::col_type&)>(), "mat2");
                pModule->add(chaiscript::fun(
                    [](const glm::mat2& m) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "mat2((" << m[0].x << ", " << m[0].y << "), (" << m[1].x << ", " << m[1].y << "))";
                        return ss.str();
                    }), "to_string");

                //glm::mat3
                register_matrix<glm::mat3>("mat3", pModule);
                pModule->add(chaiscript::constructor<glm::mat3(
                    glm::mat3::value_type, glm::mat3::value_type, glm::mat3::value_type,
                    glm::mat3::value_type, glm::mat3::value_type, glm::mat3::value_type,
                    glm::mat3::value_type, glm::mat3::value_type, glm::mat3::value_type)>(), "mat3");
                pModule->add(chaiscript::constructor<glm::mat3(
                    const glm::mat3::col_type&, 
                    const glm::mat3::col_type&, 
                    const glm::mat3::col_type&)>(), "mat3");
                pModule->add(chaiscript::fun(
                    [](const glm::mat3& m) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "mat3((" << 
                        m[0].x << ", " << m[0].y << ", " << m[0].z << "), (" << 
                        m[1].x << ", " << m[1].y << ", " << m[1].z << "), (" <<
                        m[2].x << ", " << m[2].y << ", " << m[2].z << "))";
                        return ss.str();
                    }), "to_string");

                //glm::mat4
                register_matrix<glm::mat4>("mat4", pModule);
                pModule->add(chaiscript::constructor<glm::mat4(
                    glm::mat4::value_type, glm::mat4::value_type, glm::mat4::value_type, glm::mat4::value_type, 
                    glm::mat4::value_type, glm::mat4::value_type, glm::mat4::value_type, glm::mat4::value_type, 
                    glm::mat4::value_type, glm::mat4::value_type, glm::mat4::value_type, glm::mat4::value_type,
                    glm::mat4::value_type, glm::mat4::value_type, glm::mat4::value_type, glm::mat4::value_type)>(), "mat4");
                pModule->add(chaiscript::constructor<glm::mat4(
                    const glm::mat4::col_type&, 
                    const glm::mat4::col_type&, 
                    const glm::mat4::col_type&, 
                    const glm::mat4::col_type&)>(), "mat4");
                pModule->add(chaiscript::fun(
                    [](const glm::mat4& m) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "mat4((" << 
                        m[0].x << ", " << m[0].y << ", " << m[0].z << ", " << m[0].w << "), (" << 
                        m[1].x << ", " << m[1].y << ", " << m[1].z << ", " << m[1].w << "), (" <<
                        m[1].x << ", " << m[2].y << ", " << m[2].z << ", " << m[2].w << "), (" <<
                        m[2].x << ", " << m[3].y << ", " << m[3].z << ", " << m[3].w << "))";
                        return ss.str();
                    }), "to_string");

                //glm::mat2x3
                register_matrix<glm::mat2x3>("mat2x3", pModule);
                pModule->add(chaiscript::constructor<glm::mat2x3(
                    glm::mat2x3::value_type, glm::mat2x3::value_type, glm::mat2x3::value_type,
                    glm::mat2x3::value_type, glm::mat2x3::value_type, glm::mat2x3::value_type)>(), "mat2x3");
                pModule->add(chaiscript::constructor<glm::mat2x3(
                    const glm::mat2x3::col_type&, 
                    const glm::mat2x3::col_type&)>(), "mat2x3");
                pModule->add(chaiscript::fun(
                    [](const glm::mat2x3& m) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "mat2x3((" << 
                        m[0].x << ", " << m[0].y << ", " << m[0].z << "), (" << 
                        m[1].x << ", " << m[1].y << ", " << m[1].z << "))";
                        return ss.str();
                    }), "to_string");
                
                //glm::mat3x2
                register_matrix<glm::mat3x2>("mat3x2", pModule);
                pModule->add(chaiscript::constructor<glm::mat3x2(
                    glm::mat3x2::value_type, glm::mat3x2::value_type, 
                    glm::mat3x2::value_type, glm::mat3x2::value_type, 
                    glm::mat3x2::value_type, glm::mat3x2::value_type)>(), "mat3x2");
                pModule->add(chaiscript::constructor<glm::mat3x2(
                    const glm::mat3x2::col_type&, 
                    const glm::mat3x2::col_type&,
                    const glm::mat3x2::col_type&)>(), "mat3x2");
                pModule->add(chaiscript::fun(
                    [](const glm::mat3x2& m) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "mat3x2((" << 
                        m[0].x << ", " << m[0].y << "), (" << 
                        m[1].x << ", " << m[1].y << "), (" <<
                        m[2].x << ", " << m[2].y << "))";
                        return ss.str();
                    }), "to_string");

                //glm::mat2x4
                register_matrix<glm::mat2x4>("mat2x4", pModule);
                pModule->add(chaiscript::constructor<glm::mat2x4(
                    glm::mat2x4::value_type, glm::mat2x4::value_type, glm::mat2x4::value_type, glm::mat2x4::value_type,
                    glm::mat2x4::value_type, glm::mat2x4::value_type, glm::mat2x4::value_type, glm::mat2x4::value_type)>(), "mat2x4");
                pModule->add(chaiscript::constructor<glm::mat2x4(
                    const glm::mat2x4::col_type&, 
                    const glm::mat2x4::col_type&)>(), "mat2x4");
                pModule->add(chaiscript::fun(
                    [](const glm::mat2x4& m) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "mat2x4((" << 
                        m[0].x << ", " << m[0].y << ", " << m[0].z << ", " << m[0].w << "), (" << 
                        m[1].x << ", " << m[1].y << ", " << m[1].z << ", " << m[1].w << "))";
                        return ss.str();
                    }), "to_string");

                //glm::mat4x2
                register_matrix<glm::mat4x2>("mat4x2", pModule);
                pModule->add(chaiscript::constructor<glm::mat4x2(
                    glm::mat4x2::value_type, glm::mat4x2::value_type, 
                    glm::mat4x2::value_type, glm::mat4x2::value_type, 
                    glm::mat4x2::value_type, glm::mat4x2::value_type,
                    glm::mat4x2::value_type, glm::mat4x2::value_type)>(), "mat4x2");
                pModule->add(chaiscript::constructor<glm::mat4x2(
                    const glm::mat4x2::col_type&, 
                    const glm::mat4x2::col_type&,
                    const glm::mat4x2::col_type&,
                    const glm::mat4x2::col_type&)>(), "mat4x2");
                pModule->add(chaiscript::fun(
                    [](const glm::mat4x2& m) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "mat4x2((" << 
                        m[0].x << ", " << m[0].y << "), (" << 
                        m[1].x << ", " << m[1].y << "), (" <<
                        m[2].x << ", " << m[2].y << "), (" <<
                        m[3].x << ", " << m[3].y << "))";
                        return ss.str();
                    }), "to_string");

                //glm::mat3x4
                register_matrix<glm::mat3x4>("mat3x4", pModule);
                pModule->add(chaiscript::constructor<glm::mat3x4(
                    glm::mat3x4::value_type, glm::mat3x4::value_type, glm::mat3x4::value_type, glm::mat3x4::value_type,
                    glm::mat3x4::value_type, glm::mat3x4::value_type, glm::mat3x4::value_type, glm::mat3x4::value_type,
                    glm::mat3x4::value_type, glm::mat3x4::value_type, glm::mat3x4::value_type, glm::mat3x4::value_type)>(), "mat3x4");
                pModule->add(chaiscript::constructor<glm::mat3x4(
                    const glm::mat3x4::col_type&,
                    const glm::mat3x4::col_type&,
                    const glm::mat3x4::col_type&)>(), "mat3x4");
                pModule->add(chaiscript::fun(
                    [](const glm::mat3x4& m) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "mat3x4((" << 
                        m[0].x << ", " << m[0].y << ", " << m[0].z << ", " << m[0].w << "), (" << 
                        m[1].x << ", " << m[1].y << ", " << m[1].z << ", " << m[1].w << "), (" << 
                        m[2].x << ", " << m[2].y << ", " << m[2].z << ", " << m[2].w << "))";
                        return ss.str();
                    }), "to_string");

                //glm::mat4x3
                register_matrix<glm::mat4x3>("mat4x3", pModule);
                pModule->add(chaiscript::constructor<glm::mat4x3(
                    glm::mat4x3::value_type, glm::mat4x3::value_type, glm::mat4x3::value_type, 
                    glm::mat4x3::value_type, glm::mat4x3::value_type, glm::mat4x3::value_type, 
                    glm::mat4x3::value_type, glm::mat4x3::value_type, glm::mat4x3::value_type,
                    glm::mat4x3::value_type, glm::mat4x3::value_type, glm::mat4x3::value_type)>(), "mat4x3");
                pModule->add(chaiscript::constructor<glm::mat4x3(
                    const glm::mat4x3::col_type&, 
                    const glm::mat4x3::col_type&,
                    const glm::mat4x3::col_type&,
                    const glm::mat4x3::col_type&)>(), "mat4x3");
                pModule->add(chaiscript::fun(
                    [](const glm::mat4x3& m) -> std::string 
                    {
                        std::stringstream ss;
                        ss << "mat4x3((" << 
                        m[0].x << ", " << m[0].y << ", " << m[0].z << "), (" << 
                        m[1].x << ", " << m[1].y << ", " << m[1].z << "), (" <<
                        m[2].x << ", " << m[2].y << ", " << m[2].z << "), (" <<
                        m[3].x << ", " << m[3].y << ", " << m[3].z << "))";
                        return ss.str();
                    }), "to_string");

                return pModule;
            }
        }
    }
}