#pragma once
#include "../../../src/models/models.hpp"

namespace math_lib {

inline auto one = new model::Int(1);

inline auto init_module = [](model::Object* self, const model::List* args) -> model::Object* {
    auto mod = new model::Module(
        "math",
        nullptr
    );

    mod->attrs.insert("one", one);
    
    return mod;
};

}