#include <algorithm>
#include "imgutils/search.hpp"

using namespace std;


void ExactSearch::build_index(const vector<vector<float>>& dataset, const vector<string>& image_ids) {
    // TODO: For Exact Search, "building the index" just means storing the dataset and IDs
    // so they can be iterated over during the search phase.
    dataset_ = dataset;
    image_ids_ = image_ids;
}

vector<SearchResult> ExactSearch::search(const vector<float>& query, int top_k) {
    vector<SearchResult> results;

    // TODO: Implement Brute-Force Search
    // 1. Iterate over every vector in `dataset_`.
    // 2. Calculate the Euclidean distance (or squared Euclidean) between `query` and the current dataset vector.
    // 3. Store the (distance, image_id) pair.
    // 4. Sort all pairs by distance in ascending order.
    // 5. Return the first `top_k` results.
    for (size_t i = 0; i < dataset_.size(); ++i) {
        float distance = 0.0f;
        for (size_t j = 0; j < query.size(); ++j) {
            float diff = query[j] - dataset_[i][j];
            distance += diff * diff;
        }
        results.push_back({image_ids_[i], distance});
    }
    sort(results.begin(), results.end(), [](const SearchResult& a, const SearchResult& b) {
        return a.distance < b.distance;
    });
    results.resize(min(top_k, static_cast<int>(results.size())));
    return results;
}

// --- Evaluation Metrics ---

float calculate_recall(const vector<SearchResult>& results, const vector<string>& ground_truth) {
    // TODO: Implement Recall calculation
    // Recall = (Number of relevant images retrieved) / (Total number of relevant images)
    // 1. Count how many image_ids in `results` exist in `ground_truth`.
    // 2. Divide that count by ground_truth.size().
    int relevant_count = 0;
    for (const auto& result : results) {
        if (std::find(ground_truth.begin(), ground_truth.end(), result.image_id) != ground_truth.end()) {
            relevant_count++;
        }
    }
    return ground_truth.empty() ? 0.0f : static_cast<float>(relevant_count) / static_cast<float>(ground_truth.size());
}

float calculate_ap(const vector<SearchResult>& results, const vector<string>& ground_truth) {
    // TODO: Implement Average Precision (AP)
    // AP is the average of Precision values calculated at each relevant document retrieved.
    // 1. Iterate through `results`.
    // 2. If a result is relevant (exists in ground_truth):
    //    a. Calculate precision at this rank: (number of relevant found so far) / (current rank).
    //    b. Add to a running sum of precisions.
    // 3. Divide the sum of precisions by the total number of relevant images (ground_truth.size()).
    int relevant_found = 0;
    float precision_sum = 0.0f;
    for (size_t i = 0; i < results.size(); ++i) {
        if (std::find(ground_truth.begin(), ground_truth.end(), results[i].image_id) != ground_truth.end()) {
            relevant_found++;
            precision_sum += static_cast<float>(relevant_found) / static_cast<float>(i + 1);
        }
    }
    return ground_truth.empty() ? 0.0f : precision_sum / static_cast<float>(ground_truth.size());
}
