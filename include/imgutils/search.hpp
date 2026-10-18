#pragma once
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <cmath>

// Structure to hold search results
struct SearchResult {
    std::string image_id; // Identifier for the found image
    float distance;       // Distance from the query image
};

// Base interface for all search methods (Exact, LSH, Hypercube, IVF, etc.)
class SearchMethod {
public:
    virtual ~SearchMethod() = default;

    // Train or build the index for the search method (if needed)
    virtual void build_index(const std::vector<std::vector<float>>& dataset, const std::vector<std::string>& image_ids) = 0;

    // Search for the top_k nearest neighbors of the query vector
    virtual std::vector<SearchResult> search(const std::vector<float>& query, int top_k = 10) = 0;
};

// Exact Search (Brute Force) Implementation
class ExactSearch : public SearchMethod {
private:
    std::vector<std::vector<float>> dataset_;
    std::vector<std::string> image_ids_;

public:
    void build_index(const std::vector<std::vector<float>>& dataset, const std::vector<std::string>& image_ids) override;
    std::vector<SearchResult> search(const std::vector<float>& query, int top_k = 10) override;
};

// Evaluation Metrics
// Calculate Recall@K given the retrieved results and the ground truth relevant images
float calculate_recall(const std::vector<SearchResult>& results, const std::vector<std::string>& ground_truth);

// Calculate Average Precision (AP@K) given the retrieved results and the ground truth relevant images
float calculate_ap(const std::vector<SearchResult>& results, const std::vector<std::string>& ground_truth);
