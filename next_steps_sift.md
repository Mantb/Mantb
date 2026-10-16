# Next Steps: SIFT Extraction and Bag of Visual Words

Now that your CLI parser (`args`) and dataset loader (`loader`, `split`) are working, it's time to process the actual images.

Your goal is to transition from **raw image paths** to **high-dimensional vectors** that your search algorithms can use.

## 1. Extracting SIFT Descriptors (`imgutils.cpp`)

You have a list of image paths. You need to write a function that takes an image path, opens it using OpenCV, and extracts its SIFT descriptors.

**Logic & Rules:**
*   **Load Image:** Use `cv::imread(path, cv::IMREAD_GRAYSCALE)`. Check if it's empty!
*   **Extract:** Use OpenCV's `cv::SIFT::create()`.
*   **Cap the Features:** The assignment states: *"K-means training samples a maximum of S (default 500) descriptors per training image."* If an image yields 2000 keypoints, you must select 500 of them.
*   **Format:** OpenCV returns descriptors as a `cv::Mat` (rows = keypoints, columns = 128 floats). You need to convert these rows into `std::vector<float>` so they are easier to use in your K-Means implementation.

## 2. Upgrading K-Means (`kmeans.cpp`)

Your previous K-Means implementation was designed for `std::array<double, 2>`. You need to upgrade it to handle $D$-dimensional data (`std::vector<float>`).

**Logic & Rules:**
*   **Distance Metric:** Use squared Euclidean distance to avoid slow `std::sqrt` calls.
*   **Memory Efficiency:** Do not move physical vectors into cluster buckets (e.g., avoid `vector<vector<vector<float>>>`). Instead, create an `assignments` array of size $N$, where `assignments[i]` holds an integer representing which cluster center `data[i]` belongs to.

## 3. Bag of Visual Words (BoVW) (`imgutils.cpp`)

Once K-Means is finished, you have $K$ centroids (your "Visual Vocabulary"). Now you need to represent *every single image in the dataset* as a single, fixed-size vector of length $K$.

**Logic & Rules:**
1.  Extract ALL SIFT descriptors for a given image (do NOT cap at 500 this time!).
2.  Create a histogram `std::vector<float> hist(K, 0.0f)`.
3.  For every descriptor in the image, find the nearest "Visual Word" (centroid) from your vocabulary.
4.  Increment that bucket in the histogram: `hist[nearest_word_index] += 1.0f`.
5.  **Normalize:** Divide every element in the histogram by the total number of descriptors in that image so the vector sums to 1 (L1 Normalization) or has a magnitude of 1 (L2 Normalization).

---

## Action Plan
I have populated `include/imgutils.hpp`, `src/imgutils.cpp`, `include/kmeans.hpp`, and `src/kmeans.cpp` with **empty template functions**.
Your next task is to fill in the implementation for these functions!