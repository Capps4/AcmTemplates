# Benchmark references

Snapshots downloaded from [KACTL](https://github.com/kth-competitive-programming/kactl/tree/main/content/geometry) on 2026-10-02, used only by BenchmarkCompare.cpp.

Point.h, OnSegment.h, SegmentIntersection.h: CC0, as declared in each source.
LineHullIntersection.h, HullDiameter.h: Boost Software License 1.0; see ../LICENSE.

Reference headers are unchanged. The benchmark additionally measures a const-reference adaptation of hullDiameter to separate its kernel cost from the public function's input copy, and an extrVertex-based collision-only adaptation to match the boolean query. Their original public entry points are also measured.
