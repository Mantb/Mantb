#include "include/imgutils/search.hpp"
#include "include/imgutils/imgutils.hpp"

using namespace std;

//map a point to its hypercube vertex: bit i = f_i(h_i(p))
uint64_t HypercubeSearch::vertex(const vector<float>& point) {
    uint64_t v = 0;
    for (int i = 0; i < kproj_; ++i) {
        //h_i(p) = floor((v_i . p + t_i) / w)
        float dot = 0.0f;
        for (size_t d = 0; d < point.size(); ++d) {
            dot += v_[i][d] * point[d];
        }
        long long h = (long long)floor((dot + t_[i]) / w_);

        //f_i assigns each h value a random bit, remembered so the same h always gives the same bit
        auto it = f_[i].find(h);
        if (it == f_[i].end()) {
            it = f_[i].emplace(h, uniform_int_distribution<int>(0, 1)(gen_)).first;
        }
        if (it->second) v |= (1ULL << i);
    }
    return v;
}

void HypercubeSearch::build_index(const vector<vector<float>>& dataset, const vector<string>& image_ids) {
    dataset_ = dataset;
    image_ids_ = image_ids;
    buckets_.clear();
    if (dataset_.empty()) return;
    if (kproj_ < 1 || kproj_ > 63) throw runtime_error("-kproj must be between 1 and 63");

    //random projections v_i ~ N(0,1)^d and offsets t_i ~ U[0,w)
    int dimensions = dataset_[0].size();
    normal_distribution<float> normal_dist(0.0f, 1.0f);
    uniform_real_distribution<float> uniform_dist(0.0f, w_);
    v_.assign(kproj_, vector<float>(dimensions));
    t_.assign(kproj_, 0.0f);
    f_.assign(kproj_, {});
    for (int i = 0; i < kproj_; ++i) {
        for (float& x : v_[i]) x = normal_dist(gen_);
        t_[i] = uniform_dist(gen_);
    }

    //put every point in the bucket of its vertex
    for (size_t i = 0; i < dataset_.size(); ++i) {
        buckets_[vertex(dataset_[i])].push_back(i);
    }
}

vector<SearchResult> HypercubeSearch::search(const vector<float>& query, int top_k) {
    vector<SearchResult> results;
    if (dataset_.empty()) return results;

    uint64_t q = vertex(query);
    int visited = 0, checked = 0;

    //visit vertices in increasing Hamming distance from q,
    //stop after `probes` vertices or `M` candidate points
    for (int dist = 0; dist <= kproj_ && visited < probes_ && checked < M_; ++dist) {
        //enumerate all kproj-bit masks with exactly `dist` bits set (Gosper's hack)
        uint64_t mask = (dist == 0) ? 0 : (1ULL << dist) - 1;
        while (visited < probes_ && checked < M_ && mask < (1ULL << kproj_)) {
            ++visited;
            auto it = buckets_.find(q ^ mask);
            if (it != buckets_.end()) {
                for (int i : it->second) {
                    if (checked >= M_) break;
                    results.push_back({image_ids_[i], sqrt(squared_distance(query, dataset_[i]))});
                    ++checked;
                }
            }
            if (mask == 0) break;
            uint64_t c = mask & -mask, r = mask + c;
            mask = (((r ^ mask) >> 2) / c) | r;
        }
    }

    //keep the top_k closest
    sort(results.begin(), results.end(), [](const SearchResult& a, const SearchResult& b) {
        return a.distance < b.distance;
    });
    results.resize(min(top_k, (int)results.size()));
    return results;
}
