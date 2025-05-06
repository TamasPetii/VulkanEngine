#pragma once
#include <functional>
#include <concepts>

template <typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
    { a == a } -> std::convertible_to<bool>;
};