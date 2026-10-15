
/* Benchmark program for profiling KVStore::put(). */

#include "kvstore.h"
#include "testing_helpers.h"

#include <chrono>

using Clock = std::chrono::steady_clock;

constexpr uint64_t N = 100000; /* Benchmarked puts. */

int main() {

    TempDirGuard guard(TestHelpers::create_tmp_dir());

    std::array<std::string, N> keys;
    std::array<std::string, N> values;

    // Initialise array.
    for (uint64_t i = 0; i < N; i++) {
        keys.at(i) = ("key_" + std::to_string(i));
        values.at(i) = ("value_" + std::to_string(i));
    }

    KVStore kvs(guard.dir);
    StoreFlags flags;

    KVExpected open = kvs.open(flags);
    if (!open.has_value())
        std::cout << "Open failed." << std::endl;

    KVStoreHandle stH = open.value();

    auto start = Clock::now(); /* Benchmark start. */

    for (uint64_t i = 0; i < N; i++) {
        KVResult put = kvs.put(stH, keys.at(i), values.at(i));
    }

    auto end = Clock::now(); /* Benchmark End. */

    const double bSeconds = std::chrono::duration<double>(end - start).count();

    double mOps = (static_cast<double>(N) / bSeconds) / 1e6;
    double nsPerOp = bSeconds * (1e9 / N);

    std::printf("---- Benchmark Summary ----\n");
    std::printf(" Total Ops : %ld\n", N);
    std::printf(" Elapsed   : %.2f s\n", bSeconds);
    std::printf(" Throughput: %.2f MOps/s\n", mOps);
    std::printf(" Latency   : %.1f ns/Op\n", nsPerOp);    
    
    return 0;
}