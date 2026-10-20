#include "imgutils/imgutils.hpp"
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
    cv::Mat image = cv::imread(image_path, cv::IMREAD_GRAYSCALE);
    if (image.empty()) {
        throw std::runtime_error("Could not open or find the image: " + image_path);
    }
    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
    vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    sift->detectAndCompute(image, cv::noArray(), keypoints, descriptors);

    descriptors_vec.reserve(min(max_features, descriptors.rows));
    for (int i = 0; i < max_features && i < descriptors.rows; ++i) {
        vector<float> descriptor_row(descriptors.cols);
        for (int j = 0; j < descriptors.cols; ++j) {
            descriptor_row[j] = descriptors.at<float>(i, j);
        }
        descriptors_vec.push_back(descriptor_row);
    }
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
    // 4. Normalize the histogram (divide all elements by the total number of descriptors
    for(int i=0;i<image_descriptors.size();++i){
        int j=0;
        bool flag=0;
        for(j=0;j<vocabulary.size();++j){
            if(image_descriptors[i]==vocabulary[j]){
                flag=1;
                break;
            }
        }
        if(flag==1){
            histogram[j]++;
        }
    }
    // Normalize the histogram
    float total = 0.0f;
    for (float val : histogram) {
        total += val;
    }
    if (total > 0) {
        for (float& val : histogram) {
            val /= total;
        }
    }
    return histogram;
}
