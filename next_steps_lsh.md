# Next Steps: Locality Sensitive Hashing (LSH)

With Exact Search completed, you now have a baseline to ensure your distance metrics and evaluation functions are correct. However, Exact Search is $O(N)$, which becomes too slow for massive datasets. The assignment requires implementing approximate nearest neighbor search methods, starting with **Locality Sensitive Hashing (LSH)**.

## 1. Mathematical Foundation of LSH (Euclidean)

The goal of LSH is to hash vectors such that similar vectors map to the same "bucket" with high probability. For Euclidean distance, the standard approach is using **Random Projections**.

A single hash function $h(x)$ is defined as:
$$ h(x) = \left\lfloor \frac{v \cdot x + t}{w} \right\rfloor $$

Where:
* $x$ is the input vector (dimension $D$).
* $v$ is a random vector where each component is drawn from a standard normal distribution $\mathcal{N}(0, 1)$. This vector defines a random projection line.
* $w$ is a scalar hyperparameter that controls the bucket width (bin size).
* $t$ is a random scalar offset drawn uniformly from $[0, w)$.

### Amplifying the Probability

A single hash function is not enough. We want to increase the probability that *only* close vectors collide, and decrease the chance of distant vectors colliding.

To do this, we use $L$ independent Hash Tables.
For each table, we generate $k$ independent hash functions.

The combined hash for a vector $x$ in table $j$ is the concatenation of the $k$ individual hash values:
$$ g_j(x) = (h_{j,1}(x), h_{j,2}(x), \dots, h_{j,k}(x)) $$

In C++, $g_j(x)$ can be represented as an `std::vector<int>`, or combined into a single 32-bit/64-bit integer hash if $k$ is small.

## 2. Implementation Logic (`src/lsh.cpp`)

### Building the Index
1.  **Initialize Parameters:** Based on CLI args, set the number of tables $L$, the number of functions per table $k$, and the bucket width $w$.
2.  **Generate Random Vectors:** For each of the $L$ tables, generate $k$ random projection vectors $v$ and offsets $t$. Store these.
3.  **Populate Hash Tables:**
    *   Iterate through every vector $x_i$ in the dataset.
    *   For each table $j$ (from $1$ to $L$):
        *   Compute the combined hash $g_j(x_i)$.
        *   Store the `image_id` (or index $i$) in the bucket corresponding to $g_j(x_i)$ in table $j$. (A `std::unordered_map` is perfect for this).

### Querying (Search)
1.  **Hash the Query:** Given a query vector $q$, compute its combined hash $g_j(q)$ for all $L$ tables.
2.  **Retrieve Candidates:** For each table $j$, look up the bucket $g_j(q)$. Collect all the `image_id`s found in that bucket.
3.  **Remove Duplicates:** An image might be found in the same bucket across multiple tables. Use a `std::unordered_set` or sort/unique to get a distinct list of candidate IDs.
4.  **Filter with Exact Search:** Calculate the actual Euclidean distance between $q$ and only the candidate vectors.
5.  **Return Top K:** Sort the candidates by exact distance and return the top 10.

---

## Teaser: Tying it all together in `main.cpp`

Eventually, everything will come together in your `main.cpp`. The flow will look like this:

1.  **Parse Arguments:** Use your `args.cpp` to understand what dataset to load and what search method the user requested (e.g., `-lsh -k 4 -L 5 -w 0.05`).
2.  **Load Data:** Use `loader.cpp` and `split.cpp` to load the HPatches sequences and MIRFlickr distractors.
3.  **Build Vocabulary:** Extract SIFT from training images, run `kmeans()`, and get $K$ centroids.
4.  **Compute BoVW:** Convert all images (database and queries) into normalized $K$-dimensional frequency vectors.
5.  **Initialize Search:**
    ```cpp
    SearchMethod* searcher = nullptr;
    if (use_lsh) {
        searcher = new LSHSearch(L, k_lsh, w);
    } else {
        searcher = new ExactSearch();
    }
    searcher->build_index(database_bovw_vectors, database_ids);
    ```
6.  **Evaluate:** Loop over query vectors, call `searcher->search(query)`, calculate `Recall@10` and `AP`, and average the results!

---

## Action Plan
I have created `include/imgutils/lsh.hpp` and `src/lsh.cpp` containing the `LSHSearch` class structure. Your task is to implement the logic described above inside the `TODO` blocks!
