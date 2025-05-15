#pragma once

#include <deque>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>

namespace tsnl {

class intstr {
public:
    intstr() = default;
    intstr(intstr const&) = default;
    intstr(intstr&&) = default;

    auto operator=(intstr const&) -> intstr& = default;
    auto operator=(intstr&&) -> intstr& = default;

    inline explicit intstr(uint32_t id);
    inline explicit operator uint32_t() const;

    inline intstr(std::string_view str);
    inline operator std::string() const;
    inline operator std::string_view() const;

    [[nodiscard]] auto operator<=>(intstr const&) const = default;
    [[nodiscard]] inline operator bool() const;

private:
    static std::shared_mutex mutex_;
    static std::unordered_map<std::string_view, uint32_t> s2i_map_;
    static std::deque<std::string> i2s_tab_;

    static auto intern(std::string_view str) -> uint32_t;

    uint32_t index_ = 0;
};

namespace literals {
    inline auto operator""_is(char const* str, size_t _) -> intstr {
        return {std::string_view(str)};
    }
} // namespace literals

inline intstr::intstr(uint32_t id)
: index_(id) {
}

inline intstr::operator uint32_t() const {
    return index_;
}

inline intstr::intstr(std::string_view str)
: index_(intern(str)) {
}

inline intstr::operator std::string() const {
    return std::string(std::string_view(*this));
}

inline intstr::operator std::string_view() const {
    if (index_ == 0) {
        return {};
    }
    std::shared_lock lock(mutex_);
    return i2s_tab_[index_ - 1];
}

[[nodiscard]] inline intstr::operator bool() const {
    return index_ != 0;
}

} // namespace tsnl
