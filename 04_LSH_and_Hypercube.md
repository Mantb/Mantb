# Part 4: Approximate Search (LSH & Hypercube)

Exact search suffers from the "Curse of Dimensionality" and is extremely slow for large datasets. We trade a little accuracy for a lot of speed using Approximate Nearest Neighbor (ANN) methods.

## 1. Locality Sensitive Hashing (LSH) (`lsh.cpp`)
**Logic:** Map high-dimensional vectors to hash values (buckets). The hash function is designed so that *similar vectors are highly likely to get the same hash*. During a query, only search vectors in the same bucket as the query.

**The Math (Random Projections):**
For Euclidean space, we use random lines (hyperplanes).
1. Generate a random vector $v$ of dimension $d$, where each component is drawn from a Normal distribution $\mathcal{N}(0, 1)$.
2. Choose a random shift $t$ uniformly from $[0, w)$, where $w$ is a window size parameter.
3. The hash function for a vector $x$ is:
   $h(x) = \lfloor \frac{x \cdot v + t}{w} \rfloor$ (where $\cdot$ is the dot product).

**Amplification:**
A single hash function isn't discriminative enough.
1. We combine $k$ such hash functions $g(x) = [h_1(x), h_2(x), \dots, h_k(x)]$.
2. To increase recall (chance of finding the neighbor), we use $L$ independent hash tables, each with its own $g(x)$.

**Implementation:**
* During `build()`: Compute the $L$ hash values for every vector. Store the vector index in a hash map (`std::unordered_map<HashValue, std::vector<int>>`).
* During `search()`: Hash the query $L$ times. Retrieve all vectors from those $L$ buckets. Perform exact distance computation *only* on these retrieved vectors.

---

## 2. Randomized Projections (Hypercube) (`hypercube.cpp`)
**Logic:** Project vectors onto the vertices of a hypercube $\{0, 1\}^{d'}$.

**The Math:**
1. Generate random vectors $v_i$ as in LSH.
2. The hash function is binary:
   $h_i(x) = 1$ if $x \cdot v_i \ge 0$, else $0$.
3. Combining $d'$ such functions gives a $d'$-bit string (a vertex on the hypercube).

**Implementation:**
* During `build()`: Hash vectors to hypercube vertices.
* During `search()`: Hash the query. Check the exact bucket. If we haven't found enough neighbors, expand the search to neighboring vertices (vertices with a Hamming distance of 1, then 2, etc.) until a threshold of points or checked vertices is reached.