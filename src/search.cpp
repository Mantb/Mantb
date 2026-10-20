#include "imgutils/search.hpp"
#include <algorithm>
#include <cmath>

using namespace std;

// --- Exact Search Implementation ---

void ExactSearch::build_index(const vector<vector<float>>& dataset, const vector<string>& image_ids) {
    // TODO: For Exact Search, "building the index" just means storing the dataset and IDs
    // so they can be iterated over during the search phase.
}

vector<SearchResult> ExactSearch::search(const vector<float>& query, int top_k) {
    vector<SearchResult> results;

    // TODO: Implement Brute-Force Search
    // 1. Iterate over every vector in `dataset_`.
    // 2. Calculate the Euclidean distance (or squared Euclidean) between `query` and the current dataset vector.
    // 3. Store the (distance, image_id) pair.
    // 4. Sort all pairs by distance in ascending order.
    // 5. Return the first `top_k` results.

    return results;
}

// --- Evaluation Metrics ---

float calculate_recall(const vector<SearchResult>& results, const vector<string>& ground_truth) {
    // TODO: Implement Recall calculation
    // Recall = (Number of relevant images retrieved) / (Total number of relevant images)
    // 1. Count how many image_ids in `results` exist in `ground_truth`.
    // 2. Divide that count by ground_truth.size().

    return 0.0f;
}

float calculate_ap(const vector<SearchResult>& results, const vector<string>& ground_truth) {
    // TODO: Implement Average Precision (AP)
    // AP is the average of Precision values calculated at each relevant document retrieved.
    // 1. Iterate through `results`.
    // 2. If a result is relevant (exists in ground_truth):
    //    a. Calculate precision at this rank: (number of relevant found so far) / (current rank).
    //    b. Add to a running sum of precisions.
    // 3. Divide the sum of precisions by the total number of relevant images (ground_truth.size()).

    return 0.0f;
}
