# 1. Math
## MathPackage
### Sieve
<!-- @code Sieve -->

### ModuloInteger
<!-- @code ModuloInteger -->

### Combinatorics
<!-- @code Combinatorics -->

### FloatPointNumber
<!-- @code FloatPointNumber -->

### ExGcd 
<!-- @code ExGcd -->

For the equation$ ax+by=c $, invoking exgcd, get $ x_0 $and$ y_0 $ such that $ ax_0+by_0=\gcd(a, b) $.

When $ \gcd(a,b)\mid c $ the general solution is :

$ x = x_0 \times \frac{c}{\gcd(a, b)}+k\times \frac{b}{\gcd(a, b)}
\\ \ 
\\  \\
y = y_0 \times \frac{c}{\gcd(a, b)}-k\times \frac{a}{\gcd(a, b)}
 $

## Polynomial
### FastFourierTransform
<!-- @code FastFourierTransform -->

### NumberTheoreticTransform
<!-- @code NumberTheoreticTransform -->

## Linear Algebra
### GaussianElimination
<!-- @code GaussianElimination -->

### LinearBasis
<!-- @code LinearBasis -->

## Random Number Algorithm
### RandomNumber
<!-- @code RandomNumber -->

### MillerRabin
<!-- @code MillerRabin -->

### PollardRho
<!-- @code PollardRho -->

If$ n $is a prime number (by MillerRabbin) , return$ n $,

otherwise return a random factor of$ n $in$ [2,n-1] $.

Time complexity:$ O(n^{\frac{1}{4}}\log n) $, but fast as$ O(n^{\frac{1}{4}}) $.

# 2. Dynamic Programming
## MultipleBackpacks
<!-- @code MultipleBackpacks -->

# 3. Sorting
## CountingSortOrder
<!-- @code CountingSortOrder -->

## Discreter
<!-- @code Discreter -->

## MoSort
<!-- @code MoSort -->

In mo processing, use #define add(x) & del(x) for atomic operator.

# 4. String
## String F4
### StringHash
<!-- @code StringHash -->

### Kmp
<!-- @code Kmp -->

### Z-Function
<!-- @code ZFunction -->

### Manacher
<!-- @code Manacher -->

### MinimalString
<!-- @code MinimalString -->

## Advanced Strings
### AcAutomaton
<!-- @code AcAutomaton -->

### SuffixAutomaton
<!-- @code SuffixAutomaton -->

### ExSuffixAutomaton
<!-- @code ExSuffixAutomaton -->

### PalindromeAutomaton
<!-- @code PalindromeAutomaton -->

### SuffixArray
<!-- @code SuffixArray -->



# 5. DataStructures
## BaseDataStructures
### HashMap
<!-- @code HashMap -->

### DisjointSetUnion
<!-- @code DisjointSetUnion -->

### RMQ
<!-- @code RMQ -->

### DeletableHeap
<!-- @code DeletableHeap -->

## TreeDataStructures
### TreeMap
<!-- @code TreeMap -->

### SegTree
<!-- @code SegTree -->

### Trie
<!-- @code Trie -->

### FenwickTree
<!-- @code FenwickTree -->

### SparseSegTree
<!-- @code SparseSegTree -->

# 6. Tree Theory
## Tree
<!-- @code Tree -->

## CentroidDecomposition
<!-- @code CentroidDecomposition -->

# 7. Graph Theory
## GridUtil
<!-- @code GridUtil -->

## Dijkstra
<!-- @code Dijkstra -->

## RingTree
<!-- @code RingTree -->

## TopSort
<!-- @code TopSort -->

## Connectivity
### StronglyConnectedComponent
<!-- @code StronglyConnectedComponent -->

### TwoSat
<!-- @code TwoSat -->

### VertexBiconnectedComponent
<!-- @code VertexBiconnectedComponent -->

### EdgeBiconnectedComponent
<!-- @code EdgeBiconnectedComponent -->

## Flow
### MaxFlow
<!-- @code MaxFlow -->

In a bipartite graph with $ n $ vertices and $ m $ edges, let $ L $ and $ R $ be the left and right partitions:

+ Maximum matching $ = $ Minimum vertex cover
+ Maximum independent set $ = $ $ n $ - Minimum vertex cover

After running the max flow work:

1. $ S $ is the set that can be reached from start point in the residual network
2. All unmatched vertices in the left partition $ \in S $, and all unmatched vertices in the right partition $ \notin S $.
3. For a matched edge, either both endpoints are $ \in S $, or both are $ \notin S $.

**Construct the minimum cut:**

+ **The minimum cut** $ = \{x \rightarrow y \mid x \in S \wedge y \notin S\} $.

**Construct the minimum vertex cover:**

+ **The minimum vertex cover** $ = \{x \in L \mid x \notin S\} \cup \{y \in R \mid y \in S\} $.

**Construct the maximum independent set:**

+ **The maximum independent set** $ = $ **all vertices** $ - $ **the minimum vertex cover**.

**Construct the longest antichain in a DAG** (a maximum set of mutually unreachable vertices; minimum non-overlapping chain cover):

+ First, split the vertices of the DAG into a bipartite graph. If $ x \rightarrow y $ in the DAG, let $ x_l \rightarrow y_r $ in the bipartite graph, then run maximum matching (MaxFlow).
+ **The longest antichain** consists of the vertices $ x $ for which **both** $ x_l $ and $ x_r $ belong to the maximum independent set.

### CostFlow
<!-- @code CostFlow -->

This minimum cost flow doesn't support negative cost **cycles**. However, meeting negative cost cycles are extremely rare, and it can handle negative **edge** weights.

# 8. Geometry
## Geo2
### PointVec
<!-- @code Geo2/PointVec -->

### SegLine
<!-- @code Geo2/SegLine -->

### PolygonConvex
<!-- @code Geo2/PolygonConvex -->

### Circle
<!-- @code Geo2/Circle -->

## Geo3
<!-- @code Geo3 -->

# 9. Clarketech
### ListHelper
<!-- @code ListHelper -->

### Debuger
<!-- @code Debuger -->

### DebugerCore
Copy it to the tail of `bits/stdc++.h`.

<!-- @code DebugerHeadCopyToBitsStdC++ -->

### FastInputOutput
<!-- @code FastInputOutput -->

# last. CTL

### SnippetInstaller

Save the code below as `install.py` and run it.

Usage tutorial: [Bilibili](https://www.bilibili.com/video/BV1FudhYeE7E/).

<!-- @tool SnippetInstaller -->
