#include <vector>
#include <array>
#include <cmath>
using namespace std;
vector<vector<std::array<double, 2>>> kmeans(vector<std::array<double, 2>> &points, int k, int max_iterations) {
    vector<std::array<double, 2>> centroids(k);
    for (int i = 0; i < k; ++i) {
        centroids[i] = points[i];
    }
    vector<vector<std::array<double, 2>>> clusters(k);
    for (int iteration = 0; iteration < max_iterations; ++iteration) {
        for (auto &cluster : clusters) {
            cluster.clear();
        }
        for (const auto &point : points) {
            double min_distance = std::numeric_limits<double>::max();
            int closest_centroid_index = -1;
            for (int i = 0; i < k; ++i) {
                double distance = std::sqrt(std::pow(point[0] - centroids[i][0], 2) + std::pow(point[1] - centroids[i][1], 2));
                if (distance < min_distance) {
                    min_distance = distance;
                    closest_centroid_index = i;
                }
            }
            clusters[closest_centroid_index].push_back(point);
        }
        for (int i = 0; i < k; ++i) {
            if (!clusters[i].empty()) {
                double sum_x = 0;
                double sum_y = 0;
                for (const auto &point : clusters[i]) {
                    sum_x += point[0];
                    sum_y += point[1];
                }
                centroids[i][0] = sum_x / clusters[i].size();
                centroids[i][1] = sum_y / clusters[i].size();
            }
        }
    }
    return clusters;
}
vector<std::array<double, 2>> initcentroids(int num_points, double min_x, double max_x, double min_y, double max_y) {

}
double goal(vector<vector<std::array<double, 2>>> &clusters, vector<std::array<double, 2>> &centroids) {
    double loss = 0;
    for (const auto &cluster : clusters) {
        for (const auto &point : cluster) {
            double min_distance = std::numeric_limits<double>::max();
            for (const auto &centroid : centroids) {
                double distance = std::sqrt(std::pow(point[0] - centroid[0], 2) + std::pow(point[1] - centroid[1], 2));
                if (distance < min_distance) {
                    min_distance = distance;
                }
            }
            loss += min_distance;
        }
    }
    return loss;
}