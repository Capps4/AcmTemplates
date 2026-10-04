#include "Circle.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
using namespace _geo2;
volatile double checksum = 0;

template <class F>
double measure(int repeats, F f) {
    std::array<double, 5> samples;
    for (auto &ms : samples) {
        auto start = std::chrono::steady_clock::now();
        double result = 0;
        for (int i = 0; i < repeats; ++i) result += f(i);
        checksum = result;
        ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
    }
    std::sort(samples.begin(), samples.end());
    return samples[2];
}

int main() {
    std::vector<Vec<double>> queries;
    for (int i = 0; i < 40000; ++i) {
        double a = i * 2.399963229728653;
        queries.emplace_back(std::cos(a), std::sin(a));
    }
    std::cout << "operation,n,repeats,median_ms\n" << std::fixed << std::setprecision(3);
    for (int n : {1024, 8192, 65536}) {
        std::vector<Point<double>> ps, shifted;
        for (int i = 0; i < n; ++i) {
            double a = 2 * std::acos(-1.) * i / n;
            ps.emplace_back(100 * std::cos(a), 100 * std::sin(a));
            double phase = a + .37 * 2 * std::acos(-1.) / n;
            shifted.emplace_back(100 * std::cos(phase) + 30, 100 * std::sin(phase) + 20);
        }
        auto h = Convex<double>::fromBoundary(ps), b = Convex<double>::fromBoundary(shifted);
        auto row = [&](const char *name, int repeats, auto f) {
            std::cout << name << ',' << n << ',' << repeats << ',' << measure(repeats, f) << '\n';
        };
        row("boundary_dense", 20, [&](int) { return Convex<double>::fromBoundary(ps).area2(); });
        std::vector<Point<double>> sparse;
        for (int i = 0; i < n / 4; ++i) sparse.push_back({400. * i / n, 0});
        for (int i = 0; i < n / 4; ++i) sparse.push_back({100, 400. * i / n});
        for (int i = 0; i < n / 4; ++i) sparse.push_back({100 - 400. * i / n, 100});
        for (int i = 0; i < n / 4; ++i) sparse.push_back({0, 100 - 400. * i / n});
        row("boundary_sparse", 20, [&](int) { return Convex<double>::fromBoundary(sparse).area2(); });
        auto compact = Convex<double>::fromBoundary(sparse);
        std::cerr << "sparse_capacity n=" << n << " vertices=" << compact.size() << " capacity=" << compact.vertices().capacity() << '\n';
        row("inter_predicate", 40000, [&](int i) { return double(inter(h, Line<double>::fromVec({0, 0}, queries[i]), nullptr)); });
        row("near_line", 40000, [&](int i) { auto q = near(h, Line<double>::fromVec(Point<double>::O + queries[i].rot90() * 200, queries[i])); return q.first.dist2(q.second); });
        row("tangents", 40000, [&](int i) { return double(h.tangents(Point<double>::O + queries[i] * 200)[0]); });
        row("calipers", 20, [&](int) { return diameter(h) + minWidth(h) + minBoundingRect(h)->area; });
        row("near_convex", 20, [&](int) { auto q = near(h, b); return q.first.dist2(q.second); });
        row("inter_convex_predicate", 20, [&](int) { return double(inter(h, b, nullptr)); });
        row("minkowski", 20, [&](int) { return minkowskiSum(h, b).area(); });
        row("convex_intersection", 20, [&](int) { return inter(h, b).area(); });
        row("circle_polygon_area", 20, [&](int) { return intersectionArea(Circle<double>({50, 0}, 70), h); });
    }
    std::cerr << "checksum=" << checksum << '\n';
}
