#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>

#define ASSERT_TRUE(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion failed: (" << #condition << "), function " << __FUNCTION__ \
                      << ", file " << __FILE__ << ", line " << __LINE__ << ".\n" \
                      << "Message: " << message << std::endl; \
            return false; \
        } \
    } while (0)

#define ASSERT_FALSE(condition, message) \
    do { \
        if ((condition)) { \
            std::cerr << "Assertion failed: (" << #condition << " should be false), function " << __FUNCTION__ \
                      << ", file " << __FILE__ << ", line " << __LINE__ << ".\n" \
                      << "Message: " << message << std::endl; \
            return false; \
        } \
    } while (0)

#define ASSERT_EQ(val1, val2) \
    do { \
        if ((val1) != (val2)) { \
            std::cerr << "Assertion failed: (" << #val1 << " == " << #val2 << "), function " << __FUNCTION__ \
                      << ", file " << __FILE__ << ", line " << __LINE__ << ".\n" \
                      << "Expected: " << (val2) << " but got: " << (val1) << std::endl; \
            return false; \
        } \
    } while (0)

#define ASSERT_APPROX(val1, val2, epsilon) \
    do { \
        if (std::abs((val1) - (val2)) > (epsilon)) { \
            std::cerr << "Assertion failed: (" << #val1 << " approx " << #val2 << "), function " << __FUNCTION__ \
                      << ", file " << __FILE__ << ", line " << __LINE__ << ".\n" \
                      << "Expected approx: " << (val2) << " but got: " << (val1) << std::endl; \
            return false; \
        } \
    } while (0)

class TestRunner {
public:
    using TestFunc = std::function<bool()>;

    void add_test(const std::string& name, TestFunc test) {
        tests_.push_back({name, test});
    }

    bool run_all() {
        int passed = 0;
        int failed = 0;
        for (const auto& test : tests_) {
            std::cout << "[RUN      ] " << test.name << std::endl;
            if (test.func()) {
                std::cout << "[       OK] " << test.name << std::endl;
                passed++;
            } else {
                std::cout << "[  FAILED ] " << test.name << std::endl;
                failed++;
            }
        }
        std::cout << "[==========] " << tests_.size() << " tests from " << tests_.size() << " test cases ran." << std::endl;
        std::cout << "[  PASSED  ] " << passed << " tests." << std::endl;
        if (failed > 0) {
            std::cout << "[  FAILED  ] " << failed << " tests." << std::endl;
        }
        return failed == 0;
    }

private:
    struct TestCase {
        std::string name;
        TestFunc func;
    };
    std::vector<TestCase> tests_;
};
