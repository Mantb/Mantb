# C++ Image Search Project: Implementation Breakdown

## 1. High-Level Architecture (Pipeline Flow)
The `image_search` project implements a Content-Based Image Retrieval (CBIR) system based on the Bag of Visual Words (BoVW) model.
The pipeline follows these main steps:
1. **Feature Extraction:** Extracts SIFT (Scale-Invariant Feature Transform) descriptors (128-dimensional vectors) from images using OpenCV.
2. **Vocabulary Building (K-Means):** Groups a sample of these SIFT descriptors into visual words (clusters) using the K-Means clustering algorithm. This forms the visual vocabulary.
3. **Image Representation (BoVW Histograms):** Each image is represented as a histogram of visual words by assigning its SIFT descriptors to the nearest cluster centers.
4. **Indexing and Search:** The BoVW histograms are indexed using various Nearest Neighbor search algorithms. When a query image is provided, its BoVW representation is extracted, and the system searches for similar images in the database using:
   - **Exact Search (Exhaustive):** Computes distance to all database images.
   - **LSH (Locality-Sensitive Hashing):** Uses hashing for fast approximate search.
   - **Hypercube:** A random projection method onto a hypercube structure.
   - **IVFFlat (Inverted File Flat):** Uses K-Means to cluster the database for faster search.
   - **IVFPQ (Inverted File Product Quantization):** Combines IVFFlat with product quantization for memory-efficient and fast approximate search.

## 2. Utilities & Pipeline (`loader.cpp`, `imgutils.cpp`, `kmeans.cpp`, `pipeline.cpp`)
- **`loader.cpp` / `imgutils.cpp`:** Handle loading datasets, reading images, and extracting SIFT features using OpenCV.
- **`kmeans.cpp`:** Implements the Lloyd's algorithm for K-Means clustering. It optimizes calculations using the squared Euclidean distance to avoid costly square root operations, grouping SIFT descriptors into `K` visual words.
- **`pipeline.cpp`:** Orchestrates the flow of data. It calls the feature extractors, builds the vocabulary, computes the BoVW histograms for the database and query images, and evaluates the search performance (Recall@5, Recall@10, mAP@10, average query time) through `evaluate_queries`.

## 3. Hashing Methods (`search.cpp`, `lsh.cpp`, `hypercube.cpp`)
- **`search.cpp`:** Contains the base classes and the exact nearest neighbor search. The `ExactSearch` method computes the exact squared distance between the query and all database vectors.
- **`lsh.cpp`:** Locality-Sensitive Hashing. Uses multiple hash tables with `k` hash functions per table. Vectors are hashed into buckets. During search, only vectors in the same bucket (or nearby buckets) are compared to the query, providing a fast approximate result.
- **`hypercube.cpp`:** Projects vectors onto a hypercube of dimension `k`. Points are assigned to vertices of the hypercube. The search explores the closest vertices in Hamming space using Gosper's Hack to efficiently generate bit combinations.

## 4. Inverted File Indexing (`ivf.cpp`)
- **IVFFlat:** Clusters the database vectors into `k_clusters` using K-Means (acting as coarse quantizer). Each database vector is assigned to the nearest cluster. During search, the query is compared against the cluster centers, and only the vectors in the `nprobe` nearest clusters are evaluated exactly.
- **IVFPQ:** Enhances IVFFlat with Product Quantization. After the coarse quantization (assignment to a cluster), the residual vector (difference between vector and cluster center) is split into `M` sub-vectors. Each sub-vector is quantized using a secondary K-Means (fine quantizer). This drastically reduces the memory footprint. During search, asymmetric distance computation (ADC) uses precomputed distance tables to quickly approximate the distance between the query and the quantized database vectors.
