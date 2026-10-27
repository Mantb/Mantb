#include "imgutils/search.hpp"
#include "imgutils/imgutils.hpp"
#include "kmeans.hpp"
#include <stdexcept>

using namespace std;

void IVFFlatSearch::build_index(const vector<vector<float>>& dataset, const vector<string>& image_ids) {
    dataset_ = dataset;
    image_ids_ = image_ids;
    if (dataset_.empty()) return;

    //quantizer(run k-means on the database vectors)
    centroids_ = kmeans(dataset_, kclusters_);

    //assign every point to its closest centroid (inverted lists)
    inverted_lists_.assign(centroids_.size(), {});
    for (size_t i = 0; i < dataset_.size(); ++i) {
        int closest = 0;
        float min_dist = numeric_limits<float>::max();
        for (size_t c = 0; c < centroids_.size(); ++c) {
            float dist = squared_distance(dataset_[i], centroids_[c]);
            if (dist < min_dist) {
                min_dist = dist;
                closest = c;
            }
        }
        inverted_lists_[closest].push_back(i);
    }
}

vector<SearchResult> IVFFlatSearch::search(const vector<float>& query, int top_k) {
    vector<SearchResult> results;
    if (centroids_.empty()) return results;

    //rank centroids by distance to the query
    vector<pair<float, int>> centroid_dists;
    for (size_t c = 0; c < centroids_.size(); ++c) {
        centroid_dists.push_back({squared_distance(query, centroids_[c]), (int)c});
    }
    int probes = min(nprobe_, (int)centroid_dists.size());
    partial_sort(centroid_dists.begin(), centroid_dists.begin() + probes, centroid_dists.end());

    //Scan the points of the nprobe closest clusters exhaustively
    for (int p = 0; p < probes; ++p) {
        for (int i : inverted_lists_[centroid_dists[p].second]) {
            results.push_back({image_ids_[i], sqrt(squared_distance(query, dataset_[i]))});
        }
    }

    //Keep the top_k closest
    sort(results.begin(), results.end(), [](const SearchResult& a, const SearchResult& b) {
        return a.distance < b.distance;
    });
    results.resize(min(top_k, (int)results.size()));
    return results;
}

// --- IVFPQ Implementation ---

void IVFPQSearch::build_index(const vector<vector<float>>& dataset, const vector<string>& image_ids) {
    dataset_ = dataset;
    image_ids_ = image_ids;
    if (dataset_.empty()) return;

    int D = dataset_[0].size();
    if (D % M_ != 0) {
        throw runtime_error("Vector dimension must be divisible by M for IVFPQ");
    }
    d_sub_ = D / M_;

    // 1. Coarse Quantization: Train k-means on the original dataset
    coarse_centroids_ = kmeans(dataset_, kclusters_);

    // 2. Assign each point to the closest coarse centroid and compute residuals
    inverted_lists_.assign(coarse_centroids_.size(), {});
    vector<vector<float>> residuals(dataset_.size(), vector<float>(D));
    for (size_t i = 0; i < dataset_.size(); ++i) {
        int closest = 0;
        float min_dist = numeric_limits<float>::max();
        for (size_t c = 0; c < coarse_centroids_.size(); ++c) {
            float dist = squared_distance(dataset_[i], coarse_centroids_[c]);
            if (dist < min_dist) {
                min_dist = dist;
                closest = c;
            }
        }
        inverted_lists_[closest].push_back(i);
        // Compute residual: r = x - q_c(x)
        for (int j = 0; j < D; ++j) {
            residuals[i][j] = dataset_[i][j] - coarse_centroids_[closest][j];
        }
    }

    // 3. Product Quantization: Train k-means on each segment of the residuals
    sub_centroids_.resize(M_);
    for (int m = 0; m < M_; ++m) {
        // Extract the m-th segment for all residuals
        vector<vector<float>> segment_data(residuals.size(), vector<float>(d_sub_));
        for (size_t i = 0; i < residuals.size(); ++i) {
            for (int j = 0; j < d_sub_; ++j) {
                segment_data[i][j] = residuals[i][m * d_sub_ + j];
            }
        }
        // Train k-means for this segment
        sub_centroids_[m] = kmeans(segment_data, k_sub_);
    }

    // 4. Encode each residual vector using the sub-centroids
    pq_codes_.assign(dataset_.size(), vector<int>(M_));
    for (size_t i = 0; i < dataset_.size(); ++i) {
        for (int m = 0; m < M_; ++m) {
            // Find closest sub-centroid for the m-th segment
            int closest_sub = 0;
            float min_dist = numeric_limits<float>::max();

            // Build the segment vector
            vector<float> segment(d_sub_);
            for (int j = 0; j < d_sub_; ++j) {
                segment[j] = residuals[i][m * d_sub_ + j];
            }

            for (int k = 0; k < k_sub_; ++k) {
                float dist = squared_distance(segment, sub_centroids_[m][k]);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_sub = k;
                }
            }
            pq_codes_[i][m] = closest_sub;
        }
    }
}

vector<SearchResult> IVFPQSearch::search(const vector<float>& query, int top_k) {
    vector<SearchResult> results;
    if (coarse_centroids_.empty()) return results;

    // 1. Rank coarse centroids by distance to the query
    vector<pair<float, int>> centroid_dists;
    for (size_t c = 0; c < coarse_centroids_.size(); ++c) {
        centroid_dists.push_back({squared_distance(query, coarse_centroids_[c]), (int)c});
    }
    int probes = min(nprobe_, (int)centroid_dists.size());
    partial_sort(centroid_dists.begin(), centroid_dists.begin() + probes, centroid_dists.end());

    // Compute asymmetric distances using precomputed distance tables
    for (int p = 0; p < probes; ++p) {
        int c = centroid_dists[p].second;

        // Calculate the residual query: r_q = query - q_c
        vector<float> query_residual(query.size());
        for (size_t j = 0; j < query.size(); ++j) {
            query_residual[j] = query[j] - coarse_centroids_[c][j];
        }

        // Precompute distance table for this residual query
        // dist_table[m][k] = squared distance between m-th segment of query_residual and k-th sub-centroid of m-th PQ
        vector<vector<float>> dist_table(M_, vector<float>(k_sub_));
        for (int m = 0; m < M_; ++m) {
            vector<float> segment(d_sub_);
            for (int j = 0; j < d_sub_; ++j) {
                segment[j] = query_residual[m * d_sub_ + j];
            }
            for (int k = 0; k < k_sub_; ++k) {
                dist_table[m][k] = squared_distance(segment, sub_centroids_[m][k]);
            }
        }

        // Scan points in the inverted list for cluster c using the PQ codes
        for (int i : inverted_lists_[c]) {
            float dist_sq = 0;
            for (int m = 0; m < M_; ++m) {
                dist_sq += dist_table[m][pq_codes_[i][m]];
            }
            results.push_back({image_ids_[i], sqrt(dist_sq)});
        }
    }

    // Keep the top_k closest
    sort(results.begin(), results.end(), [](const SearchResult& a, const SearchResult& b) {
        return a.distance < b.distance;
    });
    results.resize(min(top_k, (int)results.size()));
    return results;
}
