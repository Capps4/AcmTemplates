#include "../../../../../src/GraphTheory/Connectivity/VertexBiconnectedComponent/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
#include <pthread.h>
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<std::vector<int>> g(n);
    for (int i = 1; i < n; ++i) {
        int p = input.shape ? 0 : i - 1;
        g[p].push_back(i);
        g[i].push_back(p);
    }
    (void)random;
    int result = 0;
    auto work = [&] {
        result = measure(input, "chain / star; build and traversal; 256 MiB stack",
                         [&]() -> std::uint64_t {
                             VertexBC a(g);
                             std::uint64_t sum = 0;
                             for (auto &r : a.csqt)
                                 sum += r.size();
                             return sum;
                         });
    };
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) != 0)
        return 2;
    if (pthread_attr_setstacksize(&attr, 256ULL << 20) != 0) {
        pthread_attr_destroy(&attr);
        return 2;
    }
    pthread_t thread;
    int error = pthread_create(
        &thread, &attr,
        [](void *data) -> void * {
            (*static_cast<decltype(work) *>(data))();
            return nullptr;
        },
        &work);
    pthread_attr_destroy(&attr);
    if (error != 0)
        return 2;
    if (pthread_join(thread, nullptr) != 0)
        return 2;
    return result;
}
