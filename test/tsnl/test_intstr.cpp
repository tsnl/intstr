#include "tsnl/intstr.hpp"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace tsnl {

class intstr_test : public ::testing::Test {
protected:
    intstr_test() = default;
    ~intstr_test() override = default;
};

TEST_F(intstr_test, strong_ordering) {
    intstr s1("Hello");
    intstr s2("World");

    EXPECT_EQ(s1, s1);
    EXPECT_NE(s1, s2);
    EXPECT_LT(s1, s2);
}

TEST_F(intstr_test, operator_integer) {
    intstr s1("Hello");
    intstr s2("World");
    EXPECT_EQ(s1.id(), intstr::integer(s1));
    EXPECT_EQ(s2.id(), intstr::integer(s2));
}

TEST_F(intstr_test, operator_string) {
    using namespace std::literals;
    intstr s("Hello");
    EXPECT_EQ(std::string(s), "Hello"s);
}

TEST_F(intstr_test, operator_string_view) {
    using namespace std::literals;
    intstr s("");
    EXPECT_FALSE(s);
    EXPECT_EQ(std::string_view(s), ""sv);
}

TEST_F(intstr_test, idempotence_1) {
    intstr s1("Hello");
    intstr s2("Hello");
    intstr s3("World");

    EXPECT_EQ(uint32_t(s1), uint32_t(s2));
    EXPECT_NE(uint32_t(s1), uint32_t(s3));

    EXPECT_EQ(std::string(s1), std::string(s2));
    EXPECT_NE(std::string(s1), std::string(s3));

    EXPECT_EQ(std::string(s1), "Hello");
    EXPECT_EQ(std::string(s3), "World");
}

TEST_F(intstr_test, idempotence_2) {
    constexpr size_t iter_count = 128;

    std::vector<std::pair<intstr, uint32_t>> name_to_value_map;

    for (size_t n = 0; n < iter_count; n++) {
        auto s = "v_" + std::to_string(n);
        auto i = intstr(s);
        name_to_value_map.emplace_back(i, n);
    }

    for (size_t n = 0; n < iter_count; n++) {
        auto s = "v_" + std::to_string(n);
        auto i = intstr(s);
        auto p = name_to_value_map.begin() + n;
        EXPECT_EQ(p->first.id(), i.id());
        EXPECT_EQ(p->second, n);
    }
}

TEST_F(intstr_test, default_is_falsy) {
    intstr s;
    EXPECT_FALSE(s);
    EXPECT_EQ(std::string_view(s), "");
    EXPECT_EQ(s.id(), 0);
}

TEST_F(intstr_test, literal) {
    using namespace tsnl::literals;
    EXPECT_EQ("Hello"_is, intstr("Hello"));
}

TEST_F(intstr_test, hashable) {
    constexpr size_t iter_count = 128;

    std::unordered_map<intstr, uint32_t> name_to_value_map;

    for (size_t n = 0; n < iter_count; n++) {
        auto s = "v_" + std::to_string(n);
        auto i = intstr(s);
        name_to_value_map.emplace(i, n);
    }

    for (size_t n = 0; n < iter_count; n++) {
        auto s = "v_" + std::to_string(n);
        auto i = intstr(s);
        auto p = name_to_value_map.find(i);
        EXPECT_NE(p, name_to_value_map.end());
        EXPECT_EQ(p->first, i);
        EXPECT_EQ(p->second, n);
    }
}

TEST_F(intstr_test, comparable) {
    constexpr size_t iter_count = 128;

    std::map<intstr, uint32_t> name_to_value_map;

    for (size_t n = 0; n < iter_count; n++) {
        auto s = "v_" + std::to_string(n);
        auto i = intstr(s);
        name_to_value_map.emplace(i, n);
    }

    for (size_t n = 0; n < iter_count; n++) {
        auto s = "v_" + std::to_string(n);
        auto i = intstr(s);
        auto p = name_to_value_map.find(i);
        EXPECT_NE(p, name_to_value_map.end());
        EXPECT_EQ(p->first, i);
        EXPECT_EQ(p->second, n);
    }
}

TEST_F(intstr_test, readme_demo) {
    using namespace tsnl::literals;

    EXPECT_EQ("Hello"_is, "Hello"_is);
    EXPECT_NE("Hello"_is, "World"_is);

    static_assert(sizeof("Hello"_is) == 4);
    EXPECT_EQ(uint32_t("Hello"_is), "Hello"_is.id());
    EXPECT_EQ(std::string_view(intstr("Hello"_is.id())), std::string_view("Hello"));

    EXPECT_EQ(uint32_t(""_is), 0);
}

} // namespace tsnl
