
/* Benchmark program for profiling put API call. */

#include "kvstore.h"
#include "testing_helpers.h"

#include <chrono>

using Clock = std::chrono::steady_clock;

constexpr uint64_t N = 100000; /* Benchmarked puts. */

int main() {

    std::array<std::string, N> keys;
    std::array<std::string, N> values;

    TempDirGuard guard(TestHelpers::create_tmp_dir());

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

    const double elapsedSec = std::chrono::duration<double>(end - start).count();

    std::cout << "Operations: " << N << '\n';
    std::printf("Elapsed: %.2fs\n%.2f Mops/s\n%.1f ns/op\n",
                elapsedSec,
                (N / elapsedSec) / 1e6,
                elapsedSec * (1e9 / N));

    return 0;
}