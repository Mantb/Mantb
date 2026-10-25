# Next Steps: Finalizing the Main Pipeline

You have successfully implemented the core algorithms for extracting features (`imgutils.cpp`), building a vocabulary (`kmeans.cpp`), and conducting sophisticated nearest-neighbor searches (`lsh.cpp`, `hypercube.cpp`, `ivf.cpp`).

The final step is to **wire them all together**. To keep `main.cpp` clean, I have abstracted the repetitive looping logic into a new file: `pipeline.cpp`.

## The Pipeline Flow

Here is the exact sequence of events that needs to happen to perform a full Image Search evaluation:

### 1. Data Loading
*   Read `split.txt` using `load_split()`.
*   Iterate through the `train` sequence names. Use `load_dataset()` to load the images for training.
*   Iterate through the test sequences. For each HPatches sequence (which contains 6 images), the *first* image (`1.ppm`) is the **Query Image**, and the remaining 5 are the **Relevant Database Images** (the Ground Truth).
*   Load the MIRFlickr distractor dataset up to size $D$. These images are simply added to the database but are never considered relevant to any query.

### 2. Vocabulary Construction (`build_vocabulary`)
*   Pass the training images to this function.
*   For each image, extract a maximum of $S$ (default 500) SIFT descriptors.
*   Aggregate all descriptors into a massive pool of data.
*   Run `kmeans()` on this pool with $k = \text{vocab}$. The resulting centroids form your Visual Vocabulary.

### 3. Feature Extraction (`compute_dataset_bovw`)
*   Pass your database images to this function to convert them from raw images into $K$-dimensional Bag of Visual Words histograms.
*   Pass your query images through the same function.
*   **Crucial Rule:** Do *not* aggressively cap the descriptors on the query images (or if you do, use a large number). Capping at 500 is only for K-Means training performance.

### 4. Indexing and Searching (`evaluate_queries`)
*   Initialize your search method (Exact, LSH, etc.).
*   Call `build_index()` with your database BoVW vectors. For Exact Search, this does nothing. For LSH, it hashes them. For IVF, it runs K-Means again to build coarse clusters.
*   Loop over your queries. Call `search()` to retrieve the top 10 ranked image IDs.
*   Compare these IDs against the 5 relevant IDs for that specific query sequence to calculate Recall@10 and Average Precision.

---

## Action Plan
I have created `include/imgutils/pipeline.hpp` and `src/pipeline.cpp`.
Your final major task is to fill in the `TODO` blocks in `src/pipeline.cpp` and tie it all together in `src/main.cpp`!
