#include "tsnl/intstr.hpp"

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

TEST_F(intstr_test, idempotence) {
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

TEST_F(intstr_test, default_is_falsy) {
    intstr s;
    EXPECT_FALSE(s);
    EXPECT_EQ(std::string(s), "");
}

} // namespace tsnl
