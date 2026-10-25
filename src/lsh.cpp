#include "imgutils/search.hpp"
#include "imgutils/imgutils.hpp"
#include <random>
#include <cmath>
#include <unordered_set>
#include <algorithm>

using namespace std;

// Compute hash for a given point in a specific table
string LSHSearch::hash(const vector<float>& point, int table_index) const {
    string combined_hash = "";
    for (int j = 0; j < k_; ++j) {
        float dot_product = 0.0f;
        const auto& proj = hash_tables_[table_index][j];

        for (size_t d = 0; d < point.size(); ++d) {
            dot_product += point[d] * proj[d];
        }

        float t = proj.back();
        int h = static_cast<int>(floor((dot_product + t) / w_));
        combined_hash += to_string(h) + "_";
    }
    return combined_hash;
}

void LSHSearch::build_index(const vector<vector<float>>& dataset, const vector<string>& image_ids) {
    dataset_ = dataset;
    image_ids_ = image_ids;

    if (dataset.empty()) return;
    int dimensions = dataset[0].size();

    random_device rd;
    mt19937 gen(rd());
    normal_distribution<float> normal_dist(0.0f, 1.0f);
    uniform_real_distribution<float> uniform_dist(0.0f, w_);

    hash_tables_.assign(L_, vector<vector<float>>(k_, vector<float>(dimensions + 1)));
    buckets_.assign(L_, {});

    for (int l = 0; l < L_; ++l) {
        for (int k = 0; k < k_; ++k) {
            for (int d = 0; d < dimensions; ++d) {
                hash_tables_[l][k][d] = normal_dist(gen);
            }
            hash_tables_[l][k][dimensions] = uniform_dist(gen);
        }
    }

    for (size_t i = 0; i < dataset_.size(); ++i) {
        for (int l = 0; l < L_; ++l) {
            string h = hash(dataset_[i], l);
            buckets_[l][h].push_back(i);
        }
    }
}

vector<SearchResult> LSHSearch::search(const vector<float>& query, int top_k) {
    vector<SearchResult> results;
    if (dataset_.empty()) return results;

    unordered_set<int> candidate_indices;

    for (int l = 0; l < L_; ++l) {
        string h = hash(query, l);
        auto it = buckets_[l].find(h);
        if (it != buckets_[l].end()) {
            candidate_indices.insert(it->second.begin(), it->second.end());
        }
    }

    for (int idx : candidate_indices) {
        float dist = sqrt(squared_distance(query, dataset_[idx]));
        results.push_back({image_ids_[idx], dist});
    }

    sort(results.begin(), results.end(), [](const SearchResult& a, const SearchResult& b) {
        return a.distance < b.distance;
    });

    if (results.size() > static_cast<size_t>(top_k)) {
        results.resize(top_k);
    }

    return results;
}
