# Part 3: Base Index and Exact Search

This part corresponds to `search_index.hpp` and `search_index.cpp`.

## The `SearchIndex` Base Class
Because you are building multiple search algorithms (Exact, LSH, Hypercube, IVF), you should use Object-Oriented polymorphism.

Create a base class with a virtual interface:
```cpp
class SearchIndex {
public:
    virtual ~SearchIndex() = default;

    // Insert all dataset vectors into the index
    virtual void build(const std::vector<std::vector<float>>& dataset) = 0;

    // Given a query vector, return the indices of the top-N nearest neighbors
    virtual std::vector<int> search(const std::vector<float>& query, int top_n) = 0;
};
```
Every specific search method will inherit from this base class.

## Exact Search (Brute Force)
**Logic:** To find the closest vector to a query $q$, compare $q$ against *every single vector* in the dataset.

**The Math (Euclidean Distance / L2):**
For two vectors $p$ and $q$ of dimension $d$ (in our case, $d=K$ from BoVW):
$Distance(p, q) = \sqrt{\sum_{i=1}^{d} (p_i - q_i)^2}$
*Optimization tip:* You do not need to compute the square root (`std::sqrt`) when comparing distances to find the minimum. Comparing the squared distances is mathematically equivalent and much faster.

**Implementation details:**
1. Iterate through all $N$ vectors in the dataset.
2. Compute the squared Euclidean distance between the query and dataset vector $i$.
3. Maintain a data structure of the "Top 10" closest vectors seen so far.
   * *Best Practice:* Use a Max-Heap (`std::priority_queue` in C++) of size 10. If the queue has 10 items and you find a distance *smaller* than the maximum element in the heap, pop the max and push the new one.
