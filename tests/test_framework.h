#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>

struct TestCase {
    std::string name;
    std::function<void()> func;
};

inline std::vector<TestCase>& test_registry() {
    static std::vector<TestCase> r;
    return r;
}

struct TestRegistrar {
    TestRegistrar(const char* n, std::function<void()> f) {
        test_registry().push_back({n, f});
    }
};

#define TEST(name) \
    void test_##name(); \
    static TestRegistrar reg_##name(#name, test_##name); \
    void test_##name()

#define ASSERT_TRUE(x) \
    do { \
        if(!(x)) { \
            std::cerr << "FAIL: " << #x << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::exit(1); \
        } \
    } while(0)

#define ASSERT_FALSE(x) ASSERT_TRUE(!(x))

#define ASSERT_EQ(a,b) \
    do { \
        if((a)!=(b)) { \
            std::cerr << "FAIL: " << #a << " != " << #b << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::exit(1); \
        } \
    } while(0)

#define ASSERT_NE(a,b) \
    do { \
        if((a)==(b)) { \
            std::cerr << "FAIL: " << #a << " == " << #b << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::exit(1); \
        } \
    } while(0)

#define RUN_ALL_TESTS() \
    int main() { \
        for (auto& t : test_registry()) { \
            std::cout << "Running " << t.name << "..."; \
            t.func(); \
            std::cout << " OK" << std::endl; \
        } \
        std::cout << "All tests passed." << std::endl; \
        return 0; \
    }
