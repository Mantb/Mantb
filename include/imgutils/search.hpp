#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_set>
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

// --- LSH Search Implementation ---

// Helper structure to represent a single LSH Hash Function: h(x) = floor((v*x + t) / w)
class LSHSearch : public SearchMethod {
private:
    int L_; // Number of hash tables
    int k_; // Number of hash functions per table
    float w_; // Width of the hash bins
    std::vector<std::vector<std::vector<float>>> hash_tables_; // L hash tables
    std::vector<std::unordered_map<std::string, std::vector<int>>> buckets_; // L hash maps
    std::vector<std::vector<float>> dataset_;
    std::vector<std::string> image_ids_;
    std::string hash(const std::vector<float>& point, int table_index) const;
public:
    LSHSearch(int L, int k, float w) : L_(L), k_(k), w_(w) {}
    void build_index(const std::vector<std::vector<float>>& dataset, const std::vector<std::string>& image_ids) override;
    std::vector<SearchResult> search(const std::vector<float>& query, int top_k = 10) override;
};
//------------IVFFlat Implementation ----------

class IVFFlatSearch : public SearchMethod {
private:
    int kclusters_; // Number of coarse clusters (k-means centroids)
    int nprobe_;    // Number of closest clusters to scan at query time
    std::vector<std::vector<float>> centroids_;
    std::vector<std::vector<int>> inverted_lists_; // inverted_lists_[c] = indices of the points in cluster c
    std::vector<std::vector<float>> dataset_;
    std::vector<std::string> image_ids_;
public:
    IVFFlatSearch(int kclusters, int nprobe) : kclusters_(kclusters), nprobe_(nprobe) {}
    void build_index(const std::vector<std::vector<float>>& dataset, const std::vector<std::string>& image_ids) override;
    std::vector<SearchResult> search(const std::vector<float>& query, int top_k = 10) override;
};

//------------IVFPQ Implementation ----------

class IVFPQSearch : public SearchMethod {
private:
    int kclusters_; // Number of coarse clusters (k-means centroids)
    int nprobe_;    // Number of closest clusters to scan at query time
    int M_;         // Number of segments for Product Quantization
    int nbits_;     // Number of bits per segment (2^nbits_ sub-centroids per segment)

    std::vector<std::vector<float>> coarse_centroids_;
    std::vector<std::vector<int>> inverted_lists_; // inverted_lists_[c] = indices of points in cluster c

    // PQ structures
    int d_sub_; // Dimension of each segment (D / M)
    int k_sub_; // Number of sub-centroids per segment (2^nbits_)

    // sub_centroids_[m][k] gives the sub-centroid k for segment m
    std::vector<std::vector<std::vector<float>>> sub_centroids_;

    // pq_codes_[i][m] gives the sub-centroid index for segment m of the i-th database vector
    std::vector<std::vector<int>> pq_codes_;

    std::vector<std::vector<float>> dataset_;
    std::vector<std::string> image_ids_;

public:
    IVFPQSearch(int kclusters, int nprobe, int M, int nbits)
        : kclusters_(kclusters), nprobe_(nprobe), M_(M), nbits_(nbits) {
        k_sub_ = 1 << nbits_; // 2^nbits_
    }
    void build_index(const std::vector<std::vector<float>>& dataset, const std::vector<std::string>& image_ids) override;
    std::vector<SearchResult> search(const std::vector<float>& query, int top_k = 10) override;
};

//------------Hypercube Implementation ----------

class HypercubeSearch : public SearchMethod {
private:
    int kproj_;  // Dimension of the hypercube (bits per vertex)
    int probes_; // Max number of vertices to visit per query
    int M_;      // Max number of candidate points to check per query
    float w_;    // Width of the LSH bins
    std::mt19937 gen_;
    std::vector<std::vector<float>> v_; // kproj random projection vectors
    std::vector<float> t_;              // kproj random offsets in [0, w)
    std::vector<std::unordered_map<long long, int>> f_; // f_i: h_i value -> random bit {0,1}
    std::unordered_map<uint64_t, std::vector<int>> buckets_; // vertex -> point indices
    std::vector<std::vector<float>> dataset_;
    std::vector<std::string> image_ids_;
    uint64_t vertex(const std::vector<float>& point);
public:
    HypercubeSearch(int kproj, int probes, int M, float w, int seed = 1)
        : kproj_(kproj), probes_(probes), M_(M), w_(w), gen_(seed) {}
    void build_index(const std::vector<std::vector<float>>& dataset, const std::vector<std::string>& image_ids) override;
    std::vector<SearchResult> search(const std::vector<float>& query, int top_k = 10) override;
};
// Evaluation Metrics
// Calculate Recall@K given the retrieved results and the ground truth relevant images
float calculate_recall(const std::vector<SearchResult>& results, const std::vector<std::string>& ground_truth, int k);

// Calculate Average Precision (AP@K) given the retrieved results and the ground truth relevant images
float calculate_ap(const std::vector<SearchResult>& results, const std::vector<std::string>& ground_truth, int k);
