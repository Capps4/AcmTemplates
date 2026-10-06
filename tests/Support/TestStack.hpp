#pragma once
#include "TestSupport.hpp"
#include <exception>
#include <pthread.h>
#include <utility>

// This is test infrastructure, not an algorithm workaround. Recursive Tarjan
// deliberately needs a larger stack for 200K-chain stress tests on macOS.
template <class F>
int withTestStack(F work) {
    struct Context {
        F work;
        std::exception_ptr error{};
        explicit Context(F f) : work(std::move(f)) {}
    } context(std::move(work));
    pthread_attr_t attributes;
    CHECK(pthread_attr_init(&attributes) == 0);
    constexpr std::size_t bytes = 256ULL << 20;
    CHECK(pthread_attr_setstacksize(&attributes, bytes) == 0);
    pthread_t thread;
    CHECK(pthread_create(
              &thread, &attributes,
              [](void *data) -> void * {
                  auto &ctx = *static_cast<Context *>(data);
                  try {
                      ctx.work();
                  } catch (...) {
                      ctx.error = std::current_exception();
                  }
                  return nullptr;
              },
              &context) == 0);
    CHECK(pthread_attr_destroy(&attributes) == 0);
    CHECK(pthread_join(thread, nullptr) == 0);
    if (context.error)
        std::rethrow_exception(context.error);
    std::cout << "TEST_STACK_BYTES=" << bytes << '\n';
    return 0;
}
