#pragma once
#include "args.hpp"
#include "loader.hpp"
#include "imgutils/search.hpp"
#include <vector>
#include <string>

// Build the Vocabulary (K-Means) using a subset of SIFT descriptors from the training images
std::vector<std::vector<float>> build_vocabulary(const std::vector<Image>& train_images, int k_vocab, int max_sift_features);

// Convert a set of raw images into BoVW normalized histograms using the vocabulary
std::vector<std::vector<float>> compute_dataset_bovw(const std::vector<Image>& images, const std::vector<std::vector<float>>& vocabulary, int max_features = 500);

// Initialize the correct SearchMethod dynamically based on the parsed Arguments
SearchMethod* initialize_search_method(const Args& args);

// Evaluate all queries against the search index and compute average Recall and mAP
void evaluate_queries(SearchMethod* searcher, const std::vector<Image>& query_images, const std::vector<std::vector<float>>& query_bovw, const std::vector<std::vector<std::string>>& ground_truth, const std::string& output_file = "");
