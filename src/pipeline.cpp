#include "imgutils/pipeline.hpp"
#include "imgutils/imgutils.hpp"
#include "kmeans.hpp"
#include "search.hpp"
#include <iostream>

using namespace std;

vector<vector<float>> build_vocabulary(const vector<Image>& train_images, int k_vocab, int max_sift_features) {
    vector<vector<float>> all_descriptors;
    cout << "Extracting SIFT descriptors for vocabulary training..." << endl;

    // TODO:
    // 1. Loop over every image in `train_images`.
    // 2. Call `extract_sift_descriptors(image.path, max_sift_features)`.
    // 3. Append the returned descriptors to `all_descriptors`.
    for (const auto& image : train_images) {
        vector<vector<float>> descriptors = extract_sift_descriptors(image.path, max_sift_features);
        all_descriptors.insert(all_descriptors.end(), descriptors.begin(), descriptors.end());
    }

    cout << "Running K-Means to cluster " << all_descriptors.size() << " descriptors into " << k_vocab << " visual words." << endl;
    // 4. Return the result of `kmeans(all_descriptors, k_vocab)`
    return kmeans(all_descriptors, k_vocab);
}

vector<vector<float>> compute_dataset_bovw(const vector<Image>& images, const vector<vector<float>>& vocabulary, int max_features) {
    vector<vector<float>> bovw_dataset;
    // TODO:
    // 1. Loop over every image in `images`.
    // 2. Extract SIFT descriptors for the image (use max_features or a high number so you don't artificially cap test queries).
    // 3. Call `compute_bovw_histogram(descriptors, vocabulary)`.
    // 4. Append the histogram to `bovw_dataset`.
    for (const auto& image : images) {
        vector<vector<float>> descriptors = extract_sift_descriptors(image.path, max_features);
        vector<float> histogram = compute_bovw_histogram(descriptors, vocabulary);
        bovw_dataset.push_back(histogram);
    }
    return bovw_dataset;
}

SearchMethod* initialize_search_method(const Args& args) {
    if (args.method == Method::LSH) {
        return new LSHSearch(args.L, args.k, args.w);
    } else if (args.method == Method::Hypercube) {
        return new HypercubeSearch(args.kproj, args.probes, args.M, args.w, args.seed);
    } else if (args.method == Method::IVFFlat) {
        return new IVFFlatSearch(args.kclusters, args.nprobe);
    } else if (args.method == Method::IVFPQ) {
        return new IVFPQSearch(args.kclusters, args.nprobe, args.M, args.nbits);
    } else {
        return new ExactSearch();
    }
}

#include <chrono>
#include <fstream>

void evaluate_queries(SearchMethod* searcher, const vector<Image>& query_images, const vector<vector<float>>& query_bovw, const vector<vector<string>>& ground_truth, const string& output_file) {
    float total_recall_5 = 0.0f;
    float total_recall_10 = 0.0f;
    float total_ap_10 = 0.0f;
    double total_time_ms = 0.0;
    int num_queries = query_images.size();

    // TODO:
    // 1. Loop through each query (i from 0 to num_queries-1).
    // 2. Call `searcher->search(query_bovw[i], 10)` to get top 10 results.
    // 3. Calculate `Recall@10` and `AP` using the functions in `search.hpp`.
    // 4. Accumulate the totals.
    // 5. Print the average Recall (total_recall / num_queries) and Mean Average Precision (mAP = total_ap / num_queries).
    for (int i = 0; i < num_queries; ++i) {
        auto start_time = chrono::high_resolution_clock::now();
        vector<SearchResult> results = searcher->search(query_bovw[i], 10);
        auto end_time = chrono::high_resolution_clock::now();

        chrono::duration<double, milli> duration = end_time - start_time;
        total_time_ms += duration.count();

        total_recall_5 += calculate_recall(results, ground_truth[i], 5);
        total_recall_10 += calculate_recall(results, ground_truth[i], 10);
        total_ap_10 += calculate_ap(results, ground_truth[i], 10);
    }

    if (num_queries > 0) {
        float avg_recall_5 = total_recall_5 / num_queries;
        float avg_recall_10 = total_recall_10 / num_queries;
        float map = total_ap_10 / num_queries;
        double avg_time_ms = total_time_ms / num_queries;

        cout << "Evaluation Results over " << num_queries << " queries:" << endl;
        cout << "Average Recall@5: " << avg_recall_5 << endl;
        cout << "Average Recall@10: " << avg_recall_10 << endl;
        cout << "Mean Average Precision (mAP@10): " << map << endl;
        cout << "Average Query Time (ms): " << avg_time_ms << endl;

        if (!output_file.empty()) {
            ofstream out(output_file);
            if (out.is_open()) {
                out << "Average Recall@5: " << avg_recall_5 << endl;
                out << "Average Recall@10: " << avg_recall_10 << endl;
                out << "Mean Average Precision (mAP@10): " << map << endl;
                out << "Average Query Time (ms): " << avg_time_ms << endl;
                out.close();
                cout << "Results saved to " << output_file << endl;
            } else {
                cerr << "Warning: Could not open output file " << output_file << endl;
            }
        }
    } else {
        cout << "No queries to evaluate." << endl;
    }
}
