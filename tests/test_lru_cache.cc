#include "tests/test_framework.h"
#include "vde/common/lru_cache.h"

TEST(lru_cache_put_get) {
    vde::LruCache cache(2);
    cache.put(1, 100);
    cache.put(2, 200);

    uint32_t v = 0;
    ASSERT_TRUE(cache.get(1, &v));
    ASSERT_EQ(v, 100u);

    cache.put(3, 300); // Evicts key 2
    ASSERT_FALSE(cache.get(2, &v));
}

RUN_ALL_TESTS()
