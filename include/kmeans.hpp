#pragma once
#include <vector>

// Runs Lloyd's Algorithm on a dataset of high-dimensional vectors.
// Returns K centroids (the Visual Vocabulary).
std::vector<std::vector<float>> kmeans(const std::vector<std::vector<float>>& points, int k, int max_iterations = 100);
