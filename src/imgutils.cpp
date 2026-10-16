#include "imgutils.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>

using namespace std;

vector<vector<float>> extract_sift_descriptors(const string& image_path, int max_features) {
    vector<vector<float>> descriptors_vec;

    // TODO:
    // 1. Load the image in grayscale using cv::imread
    // 2. Initialize SIFT detector: cv::SIFT::create()
    // 3. Detect and compute keypoints and descriptors
    // 4. Sample a maximum of 'max_features' descriptors (e.g., take the first N, or randomly shuffle)
    // 5. Convert the cv::Mat rows into std::vector<float> and push to descriptors_vec

    return descriptors_vec;
}

vector<float> compute_bovw_histogram(const vector<vector<float>>& image_descriptors,
                                     const vector<vector<float>>& vocabulary) {
    // The size of the histogram must match the vocabulary size (K)
    vector<float> histogram(vocabulary.size(), 0.0f);

    // TODO:
    // 1. Loop over every descriptor in 'image_descriptors'
    // 2. Find the index of the closest centroid in 'vocabulary'
    // 3. Increment histogram[index]
    // 4. Normalize the histogram (divide all elements by the total number of descriptors)

    return histogram;
}
