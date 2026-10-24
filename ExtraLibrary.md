# Math

## FastFourierTransform

<!-- @code FastFourierTransform -->

## GaussianElimination

<!-- @code GaussianElimination -->

## RandomNumber

<!-- @code RandomNumber -->

## ExCrt

<!-- @code ExCrt -->

## EnumComb

<!-- @code EnumComb -->

## RangePrimeCount

<!-- @code RangePrimeCount -->

## FloorSum

<!-- @code FloorSum -->

## BitwiseTransform

<!-- @code BitwiseTransform -->

## LinearRecurrence

<!-- @code LinearRecurrence -->

## ModSqrt

<!-- @code ModSqrt -->

## DiscreteLog

<!-- @code DiscreteLog -->

## LagrangeInterpolation

<!-- @code LagrangeInterpolation -->

# DynamicProgramming

## SlopeTrick

<!-- @code SlopeTrick -->

# DataStructures

## DeletableHeap

<!-- @code DeletableHeap -->

## MonotonyStack

<!-- @code MonotonyStack -->

## WeightedDisjointSetUnion

<!-- @code WeightedDisjointSetUnion -->

## BitRMQ

<!-- @code BitRMQ -->

## RangeBIT

<!-- @code RangeBIT -->

## MatBIT

<!-- @code MatBIT -->

## RangeMatBIT

<!-- @code RangeMatBIT -->

## LiChaoTree

<!-- @code LiChaoTree -->

## RollbackDSU

<!-- @code RollbackDSU -->

## SlidingWindowAggregation

<!-- @code SlidingWindowAggregation -->

## SparseMatBIT

<!-- @code SparseMatBIT -->

## SegmentTreeBeats

<!-- @code SegmentTreeBeats -->

# String

## ExSuffixAutomaton

<!-- @code ExSuffixAutomaton -->

## MinimalString

<!-- @code MinimalString -->

## PalindromeAutomaton

<!-- @code PalindromeAutomaton -->

## LyndonFactorization

<!-- @code LyndonFactorization -->

## RunEnumeration

<!-- @code RunEnumeration -->

# Tree

## CentroidDecomposition

<!-- @code CentroidDecomposition -->

## DecareTree

<!-- @code DecareTree -->

## TreeDiameter

<!-- @code TreeDiameter -->

## TreeHash

<!-- @code TreeHash -->

## Rerooting

<!-- @code Rerooting -->

## LinkCutTree

<!-- @code LinkCutTree -->

# Graph

## BiGraphColor

<!-- @code BiGraphColor -->

## DirectedEulerRoad

<!-- @code DirectedEulerRoad -->

## UndirectedEulerRoad

<!-- @code UndirectedEulerRoad -->

## BipartiteMatching

<!-- @code BipartiteMatching -->

## LowerBoundFlow

<!-- @code LowerBoundFlow -->

## BellmanFord

<!-- @code BellmanFord -->

## OfflineDynamicConnectivity

<!-- @code OfflineDynamicConnectivity -->

## GeneralMatching

<!-- @code GeneralMatching -->

## DirectedMST

<!-- @code DirectedMST -->

# Geometry

## Geo3

<!-- @code Geo3 -->

## PolarTwoPointer

<!-- @code PolarTwoPointer -->

## ClosestPair

<!-- @code ClosestPair -->

## DelaunayTriangulation

<!-- @code DelaunayTriangulation -->

## MinimumEnclosingCircle

<!-- @code MinimumEnclosingCircle -->

# Sequence

## CountingSortOrder

<!-- @code CountingSortOrder -->

## MoSort

<!-- @code MoSort -->

In mo processing, use #define add(x) & del(x) for atomic operator.

## PartialOrder3D

<!-- @code PartialOrder3D -->

## ImplicitTreap

<!-- @code ImplicitTreap -->

# Examples and Notes

Problem-specific sketches.

## DigitDP

```latex
// 求[0, X] 的方案数/xx和
// function calc(X)
//     if X == 0 then
//         return something

//     const B = 进制
//     dig = {}
//     while X > 0 do
//         dig.push(X % B)
//         X /= B

//     w = (int)dig.size() - 1;

//     save = {{{}}}  有时候还需要一个 vis

//     /**
//      * 准备填第 p 位
//      * 之前是否贴上界
//      * ...
//      * 某个数之前满足上述条件, 后面随便填,合法的方案数/xx和
//      */
//     dfs = function(int p, bool limit, ...) 
//         if p == -1 then
//             return something

//         if !limit and !lead and save[][] != -1 then
//             return save[][]

//         answer = 0
//         down = p == w ? 1 : 0  只有数位固定时才会用这句
//         up = limit ? dig[p] : B - 1

//         for i in [down, up] do
//             process something
//             answer += dfs(p - 1, limit and i == up, ...)

//         if !limit then
// 			   vis[][] = true
//             save[][] = answer

//         return answer
// 	   end

//     return dfs(w, true, ...)
// end
```

## Block

```latex
template <class T>
class Block {
    const int n;
    int B;

    std::vector<T> a; // 每个元素的值

    std::vector<T> b{}; // 每个区块的和
    std::vector<T> t{}; // 每个区块lazy值

public:
    Block(int n) :
        n(n), B(sqrt(2 * n) + 1), a(n) {
            // B = 1 or n;
            const int blockCnt = (n - 1) / B + 1;
            b.assign(blockCnt, T{});
            t.assign(blockCnt, T{});
        }

private:
    // [l, r)
    template <class PointFunc, class BlockFunc>
    void leftToRightWork(int l, int r, PointFunc &&f, BlockFunc &&g) {
        auto edgeBlock = [&] {
            int limit = std::min(r, l / B * B + B);
            if (l <= limit - B or l >= limit) {
                return;
            }
            down(l / B);
            for (; l < limit; l++) {
                f(l); // 散块操作
            }
        };

        edgeBlock();
        for (int y = l / B; y < r / B; y++, l = y * B) {
            g(y); // 整块操作
        }
        edgeBlock();
    }

    int right(int y) {
        return std::min(n, y * B + B);
    }

    void down(int y) {
        (void)y;
        // if (t[y] == 0) {
        //     return;
        // }
        // for (int x = y * B; x < right(y); x++) {
        //     a[x] += t[y];
        // }

        // t[y] = 0;
    }

public:
    // x是点号，y是块号
    void modify(int l, int r, T v) {
        auto point = [&](int x) {
            int y = x / B;
            // a[x] += v;
            // b[y] += v;
        };
        auto block = [&](int y) {
            int len = right(y) - y * B;
            // b[y] += v * len;
            // t[y] += v;
        };
        leftToRightWork(l, r, point, block);
    }

    T query(int l, int r) {
        T sum = 0;
        auto point = [&](int x) {
            int y = x / B;
            // sum += t[y];
            // sum += a[x];
        };
        auto block = [&](int y) {
            int len = right(y) - y * B;
            // sum += b[y];
        };
        leftToRightWork(l, r, point, block);
        return sum;
    }
};
```

## DsuOnTree

```latex
//	先递归轻子树
    
//	if(当前节点有重子树:big!=-1){
//		递归重子树 
//	}
    
//	加上所有轻子树贡献 
    
//	加上当前节点贡献 
    
//	统计当前节点答案 
    
//	if(当前节点是自己父节点的重儿子)
//    	return   
//	else
//    	删除当前这棵树的贡献
```

## Some Formulas

1. $ \sum_{i = 1}^n \gcd(i, m) = \sum_{d | m} \varphi(d) \lfloor \frac{n}{d} \rfloor $
2. $ \sum_{i = 1}^n \sum_{j = 1}^m [\gcd(i, j) = 1] = \sum_{d = 1}^{\min(n, m)} \mu(d) \lfloor \frac{n}{d} \rfloor    \lfloor \frac{m}{d} \rfloor $
3. $ \sum_{i = 1}^n \sum_{j = 1}^m \gcd(i, j) = \sum_{d = 1}^{\min(n, m)} \varphi(d) \lfloor \frac{n}{d} \rfloor    \lfloor \frac{m}{d} \rfloor $
4. $ \sum_{d = 1}^{n} \varphi(d) \lfloor \frac{n}{d} \rfloor = \frac{1}{2}n(n + 1) $

Core templates: [Core Library](https://www.yuque.com/capps/template/library).
