#ifndef imgutils_hpp
#define imgutils_hpp
#include <vector>
#include <string>

// Extracts up to max_features SIFT descriptors from an image.
// Returns a matrix (vector of vectors) where each inner vector is a 128D float vector.
std::vector<std::vector<float>> extract_sift_descriptors(const std::string& image_path, int max_features = 500);

// Given an image's descriptors and the pre-computed K-Means vocabulary,
// computes the normalized Bag of Visual Words (BoVW) histogram.
std::vector<float> compute_bovw_histogram(const std::vector<std::vector<float>>& image_descriptors,
                                          const std::vector<std::vector<float>>& vocabulary);
#endif