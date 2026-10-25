#include "imgutils/search.hpp"
#include "imgutils/imgutils.hpp"
#include "kmeans.hpp"

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
