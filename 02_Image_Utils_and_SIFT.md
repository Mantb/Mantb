# Part 2: Image Processing (SIFT & BoVW)

This part corresponds to `image_utils.hpp` and `image_utils.cpp`.

## 1. SIFT Extraction
**Logic:** Images vary in scale, rotation, and lighting. SIFT (Scale-Invariant Feature Transform) finds "keypoints" (stable features like corners) and describes each with a 128D vector of gradients.

**Implementation detail:**
You must strictly use OpenCV for this.
```cpp
cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
std::vector<cv::KeyPoint> keypoints;
cv::Mat descriptors;
sift->detectAndCompute(image, cv::noArray(), keypoints, descriptors);
```
Limit extraction to a maximum of $S$ (default 500) descriptors per training image to avoid memory exhaustion during K-Means.

## 2. K-Means Clustering (Lloyd's Algorithm)
**Goal:** Partition $N$ vectors (all our collected SIFT descriptors) into $K$ clusters.

**The Math (Lloyd's Algorithm):**
1. **Initialize:** Choose $K$ random vectors from the dataset as initial centroids $C_1, C_2, \dots, C_K$. (Use K-means++ for better spreading).
2. **Assign (Expectation):** For every vector $x_i$, assign it to the centroid $C_j$ that minimizes the Euclidean distance $||x_i - C_j||_2$.
3. **Update (Maximization):** For each cluster $j$, compute the new centroid $C_j$ by taking the mean of all vectors assigned to it:
   $C_j = \frac{1}{|S_j|} \sum_{x \in S_j} x$
4. **Repeat:** Steps 2 and 3 until centroids stop moving (or a max iteration limit is reached).

**Implementation detail:**
Write a custom C++ class/function for this. Do not use OpenCV's k-means. The output is a matrix of $K$ centroids (the visual vocabulary).

## 3. Bag of Visual Words (BoVW)
**Logic:** Convert an image with varying numbers of SIFT descriptors into a fixed-size $K$-dimensional vector.

**Steps:**
1. Extract SIFT descriptors for an image.
2. For each descriptor, find the nearest "Visual Word" (centroid from step 2).
3. Increment the corresponding bucket in a $K$-dimensional histogram.
4. Normalize the histogram (e.g., L1 or L2 normalization) so the vector represents frequency/probabilities.
   $x_I = (h_1/N, h_2/N, \dots, h_K/N)$ where $N$ is the total number of descriptors in the image.
