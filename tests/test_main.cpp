#include "test_runner.hpp"
#include "kmeans.hpp"
#include "imgutils/search.hpp"
#include "imgutils/imgutils.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

bool test_kmeans() {
    vector<vector<float>> points = {{1.0, 1.0}, {1.0, 1.0}, {5.0, 5.0}, {5.0, 5.0}};
    auto centroids = kmeans(points, 2, 100);
    ASSERT_EQ(centroids.size(), 2);
    float dist = squared_distance(centroids[0], centroids[1]);
    ASSERT_APPROX(dist, 32.0f, 0.1f);
    return true;
}

bool test_exact_search() {
    ExactSearch searcher;
    vector<vector<float>> dataset = {{1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0}};
    vector<string> ids = {"1", "2", "3"};
    searcher.build_index(dataset, ids);

    vector<float> query = {2.1, 2.1};
    auto results = searcher.search(query, 2);

    ASSERT_EQ(results.size(), 2);
    ASSERT_EQ(results[0].image_id, "2");
    ASSERT_EQ(results[1].image_id, "3");
    return true;
}

bool test_lsh_search() {
    LSHSearch searcher(4, 5, 0.5f); // L, k, w
    vector<vector<float>> dataset = {{1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0}, {100.0, 100.0}};
    vector<string> ids = {"1", "2", "3", "4"};
    searcher.build_index(dataset, ids);

    vector<float> query = {2.1, 2.1};
    auto results = searcher.search(query, 2);

    // Might not find everything due to hashing, but should find something and not crash
    ASSERT_TRUE(results.size() <= 2, "Size is bounded by top_k");
    if(!results.empty()) {
        ASSERT_TRUE(results[0].distance < 100.0, "Should find reasonable distance");
    }
    return true;
}

bool test_hypercube_search() {
    HypercubeSearch searcher(3, 10, 100, 1.0f); // kproj, probes, M, w
    vector<vector<float>> dataset = {{1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0}, {100.0, 100.0}};
    vector<string> ids = {"1", "2", "3", "4"};
    searcher.build_index(dataset, ids);

    vector<float> query = {2.1, 2.1};
    auto results = searcher.search(query, 2);

    ASSERT_TRUE(results.size() <= 2, "Size is bounded by top_k");
    if(!results.empty()) {
        ASSERT_TRUE(results[0].distance < 100.0, "Should find reasonable distance");
    }
    return true;
}

bool test_ivfflat_search() {
    IVFFlatSearch searcher(2, 2); // kclusters, nprobe
    vector<vector<float>> dataset = {{1.0, 1.0}, {1.1, 1.1}, {10.0, 10.0}, {10.1, 10.1}};
    vector<string> ids = {"1", "2", "3", "4"};
    searcher.build_index(dataset, ids);

    vector<float> query = {1.05, 1.05};
    auto results = searcher.search(query, 2);

    ASSERT_EQ(results.size(), 2);
    ASSERT_TRUE(results[0].image_id == "1" || results[0].image_id == "2", "Should be cluster 1");
    return true;
}

bool test_ivfpq_search() {
    IVFPQSearch searcher(2, 2, 2, 1); // kclusters, nprobe, M, nbits (1 bit = 2 clusters per segment)
    vector<vector<float>> dataset = {
        {1.0, 1.0, 1.0, 1.0},
        {1.1, 1.1, 1.1, 1.1},
        {10.0, 10.0, 10.0, 10.0},
        {10.1, 10.1, 10.1, 10.1},
        {2.0, 2.0, 2.0, 2.0},
        {2.1, 2.1, 2.1, 2.1}
    };
    vector<string> ids = {"1", "2", "3", "4", "5", "6"};
    searcher.build_index(dataset, ids);

    vector<float> query = {1.05, 1.05, 1.05, 1.05};
    auto results = searcher.search(query, 2);

    ASSERT_EQ(results.size(), 2);
    return true;
}

bool test_evaluation_metrics() {
    vector<SearchResult> results = {
        {"1", 0.1f}, {"2", 0.2f}, {"3", 0.3f}, {"4", 0.4f}, {"5", 0.5f}
    };
    vector<string> gt = {"1", "3", "5", "7"}; // 3 relevant out of 4 total possible relevant

    // Recall@5 = 3 found / 4 total = 0.75
    float recall = calculate_recall(results, gt, 5);
    ASSERT_APPROX(recall, 0.75f, 0.001f);

    // AP@5:
    // rank 1 (id 1): relevant, prec = 1/1 = 1.0
    // rank 2 (id 2): not relevant
    // rank 3 (id 3): relevant, prec = 2/3 = 0.666...
    // rank 4 (id 4): not relevant
    // rank 5 (id 5): relevant, prec = 3/5 = 0.6
    // sum_prec = 1.0 + 0.666... + 0.6 = 2.2666...
    // AP = 2.2666... / 4 = 0.5666...
    float ap = calculate_ap(results, gt, 5);
    ASSERT_APPROX(ap, 0.5666f, 0.001f);

    return true;
}

int main() {
    TestRunner runner;
    runner.add_test("KMeans", test_kmeans);
    runner.add_test("ExactSearch", test_exact_search);
    runner.add_test("LSHSearch", test_lsh_search);
    runner.add_test("HypercubeSearch", test_hypercube_search);
    runner.add_test("IVFFlatSearch", test_ivfflat_search);
    runner.add_test("IVFPQSearch", test_ivfpq_search);
    runner.add_test("EvaluationMetrics", test_evaluation_metrics);
    return runner.run_all() ? 0 : 1;
}
