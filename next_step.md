# Next Step: Extracting SIFT Descriptors and Preparing Data

Now that you are refining your K-Means algorithm to handle 128-dimensional vectors, you need actual data to feed into it. Your next major milestone is to parse the dataset directories and extract SIFT descriptors using OpenCV.

## The Logic & Requirements

According to the assignment, you have two datasets:
1. **HPatches**: Used for training the visual vocabulary (K-Means) and serving as the actual queries and ground truth.
2. **MIRFlickr-25K**: Used purely as distractors to scale up the database size $D \in \{1000, 5000, 10000, 25000\}$.

### Step 1: Iterate Through the Dataset Directories
You need a function that can traverse a folder, find all the image files (e.g., `.jpg`, `.png`), and load them.
*   **C++ Tool:** Use the `<filesystem>` library (introduced in C++17).
*   **Action:** Write a function `std::vector<std::string> get_image_paths(const std::string& directory)` that iterates through a directory and returns a list of file paths.

### Step 2: Load Images and Extract SIFT
For each image path, you must load it as grayscale and extract its SIFT features.
*   **OpenCV Tools:**
    *   `cv::imread(path, cv::IMREAD_GRAYSCALE)`: Loads the image.
    *   `cv::Ptr<cv::SIFT> sift = cv::SIFT::create();`: Initializes the SIFT detector.
    *   `sift->detectAndCompute(image, cv::noArray(), keypoints, descriptors);`: Extracts the features. The `descriptors` matrix is what we care about. It will be an $N \times 128$ matrix of floats, where $N$ is the number of keypoints found in that specific image.

### Step 3: Cap the Features for Training (Crucial Rule)
The assignment explicitly states: **"K-means training samples a maximum of S (default 500) descriptors per training image."**
If an image has 2000 SIFT descriptors, you must randomly select (or just take the top) 500 of them. If it has less than 500, take all of them.

### Step 4: Flattening for K-Means
Your K-Means algorithm expects a massive 1D pool of 128-dimensional vectors (e.g., `std::vector<std::vector<float>> all_training_descriptors`).
*   **Action:** As you loop through the training images and extract their capped (max 500) descriptors, convert the OpenCV `cv::Mat` rows into `std::vector<float>` and append them to `all_training_descriptors`.

### Summary of Your Next Code Architecture
You should probably create these functions in your `include/image_utils.hpp` and implement them in `src/image_utils.cpp`:

1.  `load_images_from_directory(...)` -> Gets paths.
2.  `extract_sift_descriptors(image_path, max_features)` -> Returns a `std::vector<std::vector<float>>` of size up to $S \times 128$.
3.  `build_training_set(directory, max_features)` -> Loops over paths, calls `extract_sift`, and concatenates them all into one giant dataset to pass to your `kmeans(...)` function.
---

## Appendix: K-Means Optimization Snippet

As requested, here is a small structural snippet to help you fix the memory and performance issues in your `kmeans.cpp` template.

Notice how we **do not** copy the vectors into physical buckets (like `vector<vector<vector<float>>> clusters`). Instead, we keep a simple integer array `assignments` where `assignments[i] = 3` means that `points[i]` belongs to `centroids[3]`. We also remove `std::sqrt` for a massive speed boost.

```cpp
#include <vector>
#include <limits>

// A helper function to compute Squared Euclidean distance.
// We use squared distance because it is much faster than std::sqrt,
// and the relative ordering of distances remains exactly the same!
float squared_distance(const std::vector<float>& a, const std::vector<float>& b) {
    float dist = 0.0f;
    // SIFT vectors are 128D
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        dist += diff * diff;
    }
    return dist;
}

// ... inside your K-Means loop ...
// Assume 'points' is vector<vector<float>> of size N
// Assume 'centroids' is vector<vector<float>> of size K

// 1. assignments[i] will store the index of the centroid that points[i] belongs to.
std::vector<int> assignments(points.size(), -1);

// 2. The Assignment Step:
for (size_t p = 0; p < points.size(); ++p) {
    float min_dist = std::numeric_limits<float>::max();
    int best_centroid = -1;

    for (size_t c = 0; c < centroids.size(); ++c) {
        float dist = squared_distance(points[p], centroids[c]);
        if (dist < min_dist) {
            min_dist = dist;
            best_centroid = c;
        }
    }
    assignments[p] = best_centroid;
}

// 3. The Update Step:
// We need to calculate the new mean for each centroid.
// Since we don't have physical buckets, we accumulate the sums in a temporary structure.
std::vector<std::vector<float>> new_centroids(k, std::vector<float>(128, 0.0f));
std::vector<int> counts(k, 0);

for (size_t p = 0; p < points.size(); ++p) {
    int c = assignments[p];
    counts[c]++;
    for (size_t d = 0; d < 128; ++d) {
        new_centroids[c][d] += points[p][d];
    }
}

// Divide by count to get the average
for (size_t c = 0; c < k; ++c) {
    if (counts[c] > 0) {
        for (size_t d = 0; d < 128; ++d) {
            centroids[c][d] = new_centroids[c][d] / counts[c];
        }
    } else {
        // Edge case: A cluster became empty!
        // You should re-initialize centroids[c] to a random point here.
    }
}
```
