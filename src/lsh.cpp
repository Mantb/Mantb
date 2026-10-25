#include "imgutils/search.hpp"
#include "imgutils/imgutils.hpp"
using namespace std;

LSHSearch::LSHSearch(int L, int k, float w) : L_(L), k_(k), w_(w) {
    tables_.resize(L_);
}

string LSHSearch::compute_hash(const vector<float>& vec, const HashTable& table) {
    string combined_hash = "";
    // TODO:
    // 1. Loop through all `k` functions in the `table`.
    // 2. For each function, compute the dot product of `vec` and `function.v`.
    // 3. Add `function.t`, divide by `function.w`, and take the floor.
    // 4. Append the resulting integer to `combined_hash` (e.g., using std::to_string and a delimiter).
    for (const auto& function : table.functions) {
        float dot_product = 0.0f;
        for (size_t i = 0; i < vec.size(); ++i) {
            dot_product += vec[i] * function.v[i];
        }
        int hash_value = static_cast<int>(floor((dot_product + function.t) / function.w));
        combined_hash += to_string(hash_value) + "_"; // Using '_' as a delimiter
    }
    return combined_hash;
}

void LSHSearch::build_index(const vector<vector<float>>& dataset, const vector<string>& image_ids) {
    dataset_ = dataset;
    image_ids_ = image_ids;

    if (dataset.empty()) return;
    int dimensions = dataset[0].size();

    // Setup Random Generators for Normal and Uniform distributions
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<float> normal_dist(0.0f, 1.0f);
    uniform_real_distribution<float> uniform_dist(0.0f, w_);

    // TODO: Phase 1 - Initialize Hash Functions
    // 1. Loop through each of the `L_` tables.
    // 2. Inside, loop `k_` times to create `k_` LSHFunctions.
    // 3. Fill `function.v` with `dimensions` random numbers from `normal_dist`.
    // 4. Set `function.t` from `uniform_dist`.
    // 5. Set `function.w = w_`.
    // 6. Push the function into `table.functions`.
    for (int l = 0; l < L_; ++l) {
        for (int k = 0; k < k_; ++k) {
            LSHFunction function;
            function.v.resize(dimensions);
            for (int d = 0; d < dimensions; ++d) {
                function.v[d] = normal_dist(gen);
            }
            function.t = uniform_dist(gen);
            function.w = w_;
            tables_[l].functions.push_back(function);
        }
    }
    // TODO: Phase 2 - Hash the Dataset
    // 1. Loop through every vector in `dataset_` (keep track of index `i`).
    // 2. Loop through every `table` in `tables_`.
    // 3. Call `compute_hash` for the current vector and table.
    // 4. Push index `i` into `table.buckets[hash_string]`.
    for (size_t i = 0; i < dataset_.size(); ++i) {
        for (auto& table : tables_) {
            string hash_string = compute_hash(dataset_[i], table);
            table.buckets[hash_string].push_back(i);
        }
    }
}

vector<SearchResult> LSHSearch::search(const vector<float>& query, int top_k) {
    vector<SearchResult> results;
    unordered_set<int> candidate_indices;

    // TODO: Phase 1 - Candidate Generation
    // 1. Loop through all `L_` tables.
    // 2. Compute the hash for the `query` in the current table.
    // 3. Look up that hash in `table.buckets`.
    // 4. If the bucket exists, insert all indices in that bucket into `candidate_indices`.
    //    (Using an unordered_set automatically removes duplicates).
    for (const auto& table : tables_) {
        string hash_string = compute_hash(query, table);
        auto it = table.buckets.find(hash_string);
        if (it != table.buckets.end()) {
            candidate_indices.insert(it->second.begin(), it->second.end());
        }
    }
    // TODO: Phase 2 - Exact Distance Calculation
    // 1. Iterate over every index in `candidate_indices`.
    // 2. Retrieve the corresponding vector from `dataset_`.
    // 3. Compute the Euclidean distance between `query` and the dataset vector.
    // 4. Create a SearchResult pair and add it to `results`.
    for (int idx : candidate_indices) {
        float dist = sqrt(squared_distance(query, dataset_[idx]));
        results.push_back({image_ids_[idx], dist});
    }
    // TODO: Phase 3 - Sorting
    // 1. Sort the `results` by distance in ascending order.
    // 2. Return up to `top_k` elements.
    sort(results.begin(), results.end(), [](const SearchResult& a, const SearchResult& b) {
        return a.distance < b.distance;
    });
    if (results.size() > static_cast<size_t>(top_k)) {
        results.resize(top_k);
    }
    return results;
}
