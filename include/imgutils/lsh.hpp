#pragma once
#include "search.hpp"
#include <vector>
#include <string>
#include <unordered_map>

// Helper structure to represent a single LSH Hash Function: h(x) = floor((v*x + t) / w)
struct LSHFunction {
    std::vector<float> v; // Random projection vector
    float t;              // Random offset
    float w;              // Bucket width
};

// Represents a single Hash Table containing 'k' functions
struct HashTable {
    std::vector<LSHFunction> functions;

    // Maps a combined hash (represented as a string for simplicity) to a list of image indices
    std::unordered_map<std::string, std::vector<int>> buckets;
};

class LSHSearch : public SearchMethod {
private:
    int L_; // Number of Hash Tables
    int k_; // Number of Hash Functions per table
    float w_; // Bucket width

    std::vector<HashTable> tables_;

    // We store the dataset to perform the final exact distance calculation on candidates
    std::vector<std::vector<float>> dataset_;
    std::vector<std::string> image_ids_;

    // Helper method to compute the combined string hash for a vector in a specific table
    std::string compute_hash(const std::vector<float>& vec, const HashTable& table);

public:
    LSHSearch(int L, int k, float w);

    void build_index(const std::vector<std::vector<float>>& dataset, const std::vector<std::string>& image_ids) override;
    std::vector<SearchResult> search(const std::vector<float>& query, int top_k = 10) override;
};
