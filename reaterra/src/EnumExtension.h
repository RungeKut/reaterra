#ifndef ENUMEXTENSION_H_
#define ENUMEXTENSION_H_

#include  <type_traits>

#define ENABLE_BITMASK_OPERATORS(x)  \
template<>                           \
struct EnableBitMaskOperators<x> {   \
    static const bool enable = true; \
};

template<typename Enum>
struct EnableBitMaskOperators {
    static const bool enable = false;
};

template <typename Enum>
constexpr typename std::enable_if<std::is_enum<Enum>::value, typename std::underlying_type<Enum>::type>::type to_base(Enum e) noexcept {
    return static_cast<typename std::underlying_type<Enum>::type>(e);
}

template <typename Enum>
constexpr typename std::enable_if<std::is_enum<Enum>::value, Enum>::type to_enum(int v) noexcept {
    return static_cast<Enum>(v);
}

template<typename Enum>
constexpr typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator|(Enum lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template<typename Enum>
constexpr typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator&(Enum lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename Enum>
constexpr typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator^(Enum lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}

template<typename Enum>
constexpr typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator~(Enum lhs) noexcept {
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(~static_cast<underlying>(lhs));
}

template<typename Enum>
constexpr typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum&>::type
operator|=(Enum &lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type<Enum>::type;
    lhs = static_cast<Enum>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
    return lhs;
}

template<typename Enum>
constexpr typename std::enable_if<EnableBitMaskOperators<Enum&>::enable, Enum>::type
operator&=(Enum &lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type<Enum>::type;
    lhs = static_cast<Enum>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
    return lhs;
}

template<typename Enum>
constexpr typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum&>::type
operator^=(Enum &lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type<Enum>::type;
    lhs = static_cast<Enum>(static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
    return lhs;
}

template<typename Enum>
constexpr typename std::enable_if<EnableBitMaskOperators<Enum>::enable, bool>::type
operator&&(Enum lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<bool>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename Enum>
constexpr typename std::enable_if<EnableBitMaskOperators<Enum>::enable, bool>::type
to_bool(Enum lhs) noexcept {
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<bool>(static_cast<underlying>(lhs));
}

#endif /* ENUMEXTENSION_H_ */
