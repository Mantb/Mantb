#include "kmeans.hpp"
#include "imgutils/imgutils.hpp"
#include <limits>
#include <random>

using namespace std;

vector<vector<float>> kmeans(const vector<vector<float>>& points, int k, int max_iterations) {
    vector<vector<float>> centroids(k);

    // Edge case: if k > number of points, just return the points
    if (points.size() < static_cast<size_t>(k)) {
        vector<vector<float>> padded = points;
        if(points.empty()) return padded;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, points.size() - 1);
        while(padded.size() < static_cast<size_t>(k)) {
            padded.push_back(points[dis(gen)]);
        }
        return padded;
    } else if (points.size() == static_cast<size_t>(k)) {
        return points;
    }

    // TODO: Phase 1: Initialization
    // Pick K random vectors from 'points' and assign them to 'centroids'.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, points.size() - 1);
    for (int i = 0; i < k; ++i) {
        centroids[i] = points[dis(gen)];
    } 
    // This array will hold the index of the centroid that each point is assigned to.
    vector<int> assignments(points.size(), -1);

    for (int iteration = 0; iteration < max_iterations; ++iteration) {

        bool changed = false;

        // TODO: Phase 2: Expectation (Assignment Step)
        // For every point in 'points', find the closest centroid using squared_distance.
        // Update 'assignments[i]'. If the assignment changes, set changed = true.
        for(id_t i = 0; i < points.size(); ++i) {
            float min_dist = std::numeric_limits<float>::max();
            int closest_centroid = -1;
            for(int j = 0; j < k; ++j) {
                float dist = squared_distance(points[i], centroids[j]);
                if(dist < min_dist) {
                    min_dist = dist;
                    closest_centroid = j;
                }
            }
            if(assignments[i] != closest_centroid) {
                assignments[i] = closest_centroid;
                changed = true;
            }
        }
        // If no points changed clusters,break early.
        if(!changed)break;
        // TODO: Phase 3: Maximization (Update Step)
        // Calculate the new means for each centroid.
        // You'll need temporary vectors to sum the dimensions and an array to count the items per cluster.
        size_t dim = points[0].size();
        std::vector<std::vector<float>> sum_centroids(k, std::vector<float>(dim, 0.0f));
        std::vector<int> count_centroids(k, 0);

        for (size_t i = 0; i < points.size(); ++i) {
            int c = assignments[i];
            count_centroids[c]++;
            for (size_t d = 0; d < dim; ++d) {
                sum_centroids[c][d] += points[i][d];
            }
        }

        for (int j = 0; j < k; ++j) {
            if (count_centroids[j] > 0) {
                for (size_t d = 0; d < dim; ++d) {
                    centroids[j][d] = sum_centroids[j][d] / count_centroids[j];
                }
            } else {
                // Re-initialize empty clusters to a random point
                centroids[j] = points[dis(gen)];
            }
        }
       
    }

    return centroids;
}
