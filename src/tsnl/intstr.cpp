#include "tsnl/intstr.hpp"

namespace tsnl {

std::shared_mutex intstr::mutex_;
std::unordered_map<std::string_view, uint32_t> intstr::s2i_map_;
std::deque<std::string> intstr::i2s_tab_;

auto intstr::intern(std::string_view str) -> uint32_t {
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
        uint32_t index = i2s_tab_.size();
        i2s_tab_.emplace_back(str);
        s2i_map_[str] = index;
        return index;
    }
}

} // namespace tsnl
