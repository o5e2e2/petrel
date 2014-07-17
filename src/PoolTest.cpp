#include "catch.hpp"

#include <vector>
#include "Pool.hpp"

typedef Pool<int> PoolTest;

TEST_CASE( "Pool is working", "[Pool]" ) {
    PoolTest pool;
    CHECK (pool.total() == 0);
    CHECK (pool.used() == 0);
    CHECK (pool.free() == 0);

    auto first = pool.acquire();
    CHECK (pool.total() == 1);
    CHECK (pool.used() == 1);
    CHECK (pool.free() == 0);

    pool.release(std::move(first));
    CHECK (pool.total() == 1);
    CHECK (pool.used() == 0);
    CHECK (pool.free() == 1);

    auto second = pool.acquire();
    CHECK (first == second);
    CHECK (pool.total() == 1);
    CHECK (pool.used() == 1);
    CHECK (pool.free() == 0);

    auto third = pool.acquire();
    CHECK (pool.total() == 2);
    CHECK (pool.used() == 2);
    CHECK (pool.free() == 0);

    pool.release(std::move(third));
    CHECK (pool.total() == 2);
    CHECK (pool.used() == 1);
    CHECK (pool.free() == 1);

    auto forth = pool.acquire();
    CHECK (third == forth);
    CHECK (pool.total() == 2);
    CHECK (pool.used() == 2);
    CHECK (pool.free() == 0);

    pool.release(std::move(forth));
    CHECK (pool.total() == 2);
    CHECK (pool.used() == 1);
    CHECK (pool.free() == 1);

    pool.release(std::move(second));
    CHECK (pool.total() == 2);
    CHECK (pool.used() == 0);
    CHECK (pool.free() == 2);

    auto fifth= pool.acquire();
    CHECK( fifth == second );
    CHECK (pool.total() == 2);
    CHECK (pool.used() == 1);
    CHECK (pool.free() == 1);

}
