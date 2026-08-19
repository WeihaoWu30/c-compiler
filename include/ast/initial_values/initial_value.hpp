#pragma once
#include <variant>

namespace ast {
    struct Tentative {
    };

    struct Initial {
        int value;
        Initial(int value_) : value(value_) {}
    };

    struct No_Initializer {
    };
    using Initial_Value = std::variant<No_Initializer, Tentative, Initial>;
}