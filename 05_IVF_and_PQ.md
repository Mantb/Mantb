# Part 5: Inverted File & Product Quantization

This part corresponds to `ivf.hpp` and `ivf.cpp`. It is the most advanced and widely used ANN technique in production today.

## 1. IVF (Inverted File Index)
**Logic:** Instead of searching the whole dataset, divide the dataset into regions. Only search the regions closest to the query.

**Implementation:**
1. **Build:** Run K-means on the dataset to create $C$ coarse clusters.
2. Create an "Inverted List": an array of $C$ lists. Each list contains the indices of the dataset vectors that belong to that cluster.
3. **Search:** For a query, find the $b$ (nprobes) closest coarse centroids. Retrieve the vectors only from those $b$ lists and perform exact distance computations on them.

---

## 2. Product Quantization (PQ)
**Logic:** Even inside the IVF lists, computing exact distances on high-dimensional vectors is slow and storing them takes too much memory. PQ compresses the vectors and allows distances to be computed using lookup tables.

**The Math (Compression):**
1. Take a $D$-dimensional vector and split it into $M$ equal sub-vectors (chunks). (e.g., a 128D vector split into 8 chunks of 16 dimensions).
2. For *each* chunk position (1 to $M$), run K-means to find a "sub-codebook" with $k^*$ (usually 256) sub-centroids.
3. Replace each sub-vector with the ID (an 8-bit integer) of its nearest sub-centroid.
   *Result:* A 128-float vector (512 bytes) is compressed into just $M$ bytes (e.g., 8 bytes).

**The Math (Asymmetric Distance Computation - ADC):**
When a query arrives, we DO NOT compress it. We want the distance between the uncompressed query $q$ and a compressed dataset vector $x$.
1. Split $q$ into $M$ sub-vectors.
2. Build a Look-Up Table (LUT): For each of the $M$ chunks, compute the exact distance between the query sub-vector and all 256 sub-centroids in the sub-codebook.
3. To find the distance to $x$: Look at $x$'s $M$ integer IDs. Look up the pre-computed distances in the LUT and sum them up. This requires 0 floating point math, just $M$ array lookups and additions!

## 3. IVFPQ (Combining IVF and PQ)
**Logic:**
1. Use IVF to find the general neighborhood.
2. Instead of storing exact vectors in the IVF lists, store their **Residuals** (the vector minus its IVF centroid).
3. Compress these Residuals using PQ.
4. During search, compute the query's residual, build the PQ LUT for that residual, and rapidly scan the IVF list using ADC.