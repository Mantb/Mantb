#include "kmeans.hpp"
#include <limits>
#include <random>

using namespace std;

float squared_distance(const vector<float>& a, const vector<float>& b) {
    float dist = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        dist += diff * diff;
    }
    return dist;
    //cause we have 128 dimensions 
}

vector<vector<float>> kmeans(const vector<vector<float>>& points, int k, int max_iterations) {
    vector<vector<float>> centroids(k);

    // Edge case: if k > number of points, just return the points
    if (points.size() <= static_cast<size_t>(k)) {
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
        const static int sift_dim=k;
        std::vector<std::vector<float>> sum_centroids(k);
        std::vector<int> count_centroids(sift_dim, 0);
        for (int j = 0; j < k; ++j) {
            if (count_centroids[j] > 0) {
                for (size_t d = 0; d < centroids[j].size(); ++d) {
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
