#include <catch2/catch_test_macros.hpp>
#include <string>

// This is the main test file that will be linked with Catch2WithMain
// Individual test files will be linked separately through CMake

// Additional test cases can be added here
TEST_CASE("Basic project test", "[basic]") {
    REQUIRE(true);
    CHECK(1 + 1 == 2);
}

TEST_CASE("String operations", "[basic][string]") {
    std::string test_string = "Hello, World!";
    
    SECTION("Length check") {
        REQUIRE(test_string.length() == 13);
    }
    
    SECTION("Substring check") {
        REQUIRE(test_string.substr(0, 5) == "Hello");
    }
    
    SECTION("Find operation") {
        REQUIRE(test_string.find("World") != std::string::npos);
    }
}
