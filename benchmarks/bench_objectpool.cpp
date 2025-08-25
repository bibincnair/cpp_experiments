#include "objectpool/objectpool.h"
#include <benchmark/benchmark.h>

struct BenchmarkObject
{
  long long data[8];
};


static void BM_NewDelete(benchmark::State &state)
{
  for (auto _ : state) {
    BenchmarkObject *obj = new BenchmarkObject();
    benchmark::DoNotOptimize(obj);
    delete obj;
  }
}
BENCHMARK(BM_NewDelete);

static void BM_ObjectPool(benchmark::State &state)
{
  memory::ObjectPool<BenchmarkObject> pool(static_cast<size_t>(state.range(0)));

  for (auto _ : state) {
    BenchmarkObject *obj = pool.construct();
    benchmark::DoNotOptimize(obj);
    pool.destroy(obj);
  }
}

BENCHMARK(BM_ObjectPool)->Range(8, 8 << 10);

BENCHMARK_MAIN();