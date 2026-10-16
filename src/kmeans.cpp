#include "kmeans.hpp"
#include <limits>
#include <random>

using namespace std;

float squared_distance(const vector<float>& a, const vector<float>& b) {
    float dist = 0.0f;
    // TODO: Implement squared Euclidean distance calculation here
    return dist;
}

vector<vector<float>> kmeans(const vector<vector<float>>& points, int k, int max_iterations) {
    vector<vector<float>> centroids(k);

    // Edge case: if k > number of points, just return the points
    if (points.size() <= static_cast<size_t>(k)) {
        return points;
    }

    // TODO: Phase 1: Initialization
    // Pick K random vectors from 'points' and assign them to 'centroids'.

    // This array will hold the index of the centroid that each point is assigned to.
    vector<int> assignments(points.size(), -1);

    for (int iteration = 0; iteration < max_iterations; ++iteration) {

        bool changed = false;

        // TODO: Phase 2: Expectation (Assignment Step)
        // For every point in 'points', find the closest centroid using squared_distance.
        // Update 'assignments[i]'. If the assignment changes, set changed = true.


        // If no points changed clusters, we have converged! Break early.
        // if (!changed) break;


        // TODO: Phase 3: Maximization (Update Step)
        // Calculate the new means for each centroid.
        // You'll need temporary vectors to sum the dimensions and an array to count the items per cluster.

    }

    return centroids;
}
