#include <catch2/catch_test_macros.hpp>
#include "objectpool/objectpool.h"


struct TestObject{
    int x;
    double y;
    TestObject(): x(0), y(0.0){}
    TestObject(int a, double b): x(a), y(b){}
};

TEST_CASE("ObjectPool Core Functionality", "[objectpool]"){
    memory::ObjectPool<TestObject> pool(2);

    SECTION("Construction and Destruction"){
        TestObject* obj1 = pool.construct(10, 20.5);
        REQUIRE(obj1 != nullptr);
        REQUIRE(obj1->x == 10);
        REQUIRE(obj1->y == 20.5);
    }

    SECTION("Pool Exhaustion"){
        TestObject* obj1 = pool.construct();
        TestObject* obj2 = pool.construct();

        REQUIRE_THROWS_AS(pool.construct(), std::bad_alloc);
        pool.destroy(obj1);
        pool.destroy(obj2);
    }

    SECTION("Slot reuse"){
        TestObject* ptr1 = pool.construct();
        pool.destroy(ptr1);

        TestObject* ptr2 = pool.construct();
        REQUIRE(ptr1 == ptr2);
        
        pool.destroy(ptr2);

    }
}

