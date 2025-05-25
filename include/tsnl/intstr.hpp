#pragma once

#include <concepts>
#include <deque>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>

namespace tsnl {

template <std::unsigned_integral Integer>
class basic_intstr {
public:
    using integer = Integer;

    basic_intstr() = default;
    basic_intstr(basic_intstr const&) = default;
    basic_intstr(basic_intstr&&) = default;

    auto operator=(basic_intstr const&) -> basic_intstr& = default;
    auto operator=(basic_intstr&&) -> basic_intstr& = default;

    inline explicit basic_intstr(integer id);
    inline basic_intstr(std::string_view str);

    [[nodiscard]] inline auto id() const -> integer;

    [[nodiscard]] inline operator std::string() const;
    [[nodiscard]] inline operator std::string_view() const;
    [[nodiscard]] inline explicit operator integer() const;
    [[nodiscard]] inline operator bool() const;

    auto operator<=>(basic_intstr const&) const = default;

private:
    inline static std::shared_mutex mutex_{};
    inline static std::unordered_map<std::string_view, integer> s2i_map_{};
    inline static std::deque<std::string> i2s_tab_{};

    inline static auto intern(std::string_view str) -> integer;

    integer id_ = 0;
};

// Edit/patch the below line to change the size of 'intstr', e.g. to uint16_t, uint64_t, etc.
class intstr : public basic_intstr<uint32_t> {
public:
    using basic_intstr::basic_intstr;

    auto operator<=>(intstr const&) const = default;
};

template <std::unsigned_integral Integer>
inline basic_intstr<Integer>::basic_intstr(integer id)
: id_(id) {
}

template <std::unsigned_integral Integer>
inline auto basic_intstr<Integer>::id() const -> integer {
    return id_;
}

template <std::unsigned_integral Integer>
inline basic_intstr<Integer>::basic_intstr(std::string_view str)
: id_(intern(str)) {
}

template <std::unsigned_integral Integer>
inline basic_intstr<Integer>::operator integer() const {
    return id_;
}

template <std::unsigned_integral Integer>
inline basic_intstr<Integer>::operator std::string() const {
    return std::string(std::string_view(*this));
}

template <std::unsigned_integral Integer>
inline basic_intstr<Integer>::operator std::string_view() const {
    if (id_ == 0) {
        return {};
    }
    std::shared_lock lock(mutex_);
    return i2s_tab_[id_ - 1];
}

template <std::unsigned_integral Integer>
inline basic_intstr<Integer>::operator bool() const {
    return id_ != 0;
}

template <std::unsigned_integral Integer>
inline auto basic_intstr<Integer>::intern(std::string_view str) -> integer {
    // If the string is empty, return the special ID 0, representing the falsy empty string:
    {
        if (str.empty()) {
            return 0;
        }
    }

    // Try looking up all interned strings with a read lock first:
    {
        std::shared_lock lock(mutex_);
        auto it = s2i_map_.find(str);
        if (it != s2i_map_.end()) {
            return it->second;
        }
    }

    // If not found, acquire a write lock to add the new string:
    {
        std::unique_lock lock(mutex_);
        integer id = 1 + i2s_tab_.size();
        i2s_tab_.emplace_back(str);
        s2i_map_.emplace(i2s_tab_.back(), id);
        return id;
    }
}

} // namespace tsnl

namespace tsnl::literals {
inline auto operator""_is(char const* str, size_t n) -> intstr {
    return {std::string_view(str, n)};
}
} // namespace tsnl::literals

template <>
struct ::std::hash<::tsnl::intstr> {
    auto operator()(::tsnl::intstr intstr) const {
        auto intstr_id = intstr.id();
        return ::std::hash<decltype(intstr_id)>{}(intstr_id);
    }
};
