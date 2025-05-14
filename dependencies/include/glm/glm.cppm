#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp>          

namespace glm {
    // Base types
    using glm::qualifier;
    using glm::precision;
    using glm::vec;
    using glm::mat;
    using glm::qua;

#    if GLM_HAS_TEMPLATE_ALIASES
    using glm::tvec1;
    using glm::tvec2;
    using glm::tvec3;
    using glm::tvec4;
    using glm::tmat2x2;
    using glm::tmat2x3;
    using glm::tmat2x4;
    using glm::tmat3x2;
    using glm::tmat3x3;
    using glm::tmat3x4;
    using glm::tmat4x2;
    using glm::tmat4x3;
    using glm::tmat4x4;
    using glm::tquat;
#    endif

    using glm::int8;
    using glm::int16;
    using glm::int32;
    using glm::int64;
    using glm::uint8;
    using glm::uint16;
    using glm::uint32;
    using glm::uint64;

    //Precision types for integers
    using glm::lowp_i8;
    using glm::mediump_i8;
    using glm::highp_i8;
    using glm::lowp_i16;
    using glm::mediump_i16;
    using glm::highp_i16;
    using glm::lowp_i32;
    using glm::mediump_i32;
    using glm::highp_i32;
    using glm::lowp_i64;
    using glm::mediump_i64;
    using glm::highp_i64;

    //Precision types for unsigned integers
    using glm::lowp_u8;
    using glm::mediump_u8;
    using glm::highp_u8;
    using glm::lowp_u16;
    using glm::mediump_u16;
    using glm::highp_u16;
    using glm::lowp_u32;
    using glm::mediump_u32;
    using glm::highp_u32;
    using glm::lowp_u64;
    using glm::mediump_u64;
    using glm::highp_u64;

    //Precision types for floating-point numbers
    using glm::lowp_f32;
    using glm::mediump_f32;
    using glm::highp_f32;
    using glm::lowp_f64;
    using glm::mediump_f64;
    using glm::highp_f64;

    //Special types for 8-bit integers and floating-point
    using glm::i8;
    using glm::u8;
    using glm::i16;
    using glm::u16;
    using glm::i32;
    using glm::u32;
    using glm::i64;
    using glm::u64;
    using glm::f32;
    using glm::f64;
}
