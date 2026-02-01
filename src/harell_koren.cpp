#include "harell_koren.hpp"
#include "bin_heap.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

void HarellKoren::apply(Graph& g) {

    std::clog << ">> Computing HarellKoren\n";
    auto start = std::chrono::high_resolution_clock::now();

    float L0 = std::max(cfg_.mx, cfg_.my) / 2;

    // PERF: not memory optimized
    std::clog << "Computing Shortest Paths\n";
    dist_ = g.computeAllPairsShortestPaths();

    std::clog << "Computing Spring lengths and strenghts\n";
    computeLAndK(dist_, L0, cfg_.K);

    size_t V = g.nodes.size();
    if (V == 0)
        return;

    size_t curr_size_ = cfg_.min_size;
    std::clog << "Computing Layout\n";
    while (curr_size_ <= V) {

        std::clog << "K: " << curr_size_ << "\n";
        std::clog << "Computing KCenters\n";
        auto centers = kCenters(g, dist_, curr_size_);

        std::clog << "Computing Radius\n";
        float radius = computeRadius(centers, dist_, cfg_.rad);

        std::clog << "Local Layout\n";
        localLayout(g, dist_, radius);

        // std::clog << "Add random noise\n";
        // noise(g, centers, dist_, V);

        curr_size_ *= cfg_.ratio;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::clog << "Elapsed time: " << elapsed.count() << " seconds\n";
}

void HarellKoren::noise(Graph& g, const std::vector<int>& centers,
                        const std::vector<std::vector<float>>& dist, const size_t V) {
    std::random_device rd;

    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> rand(0.0f, 1.0f);

    for (size_t v = 0; v < V; ++v) {
        float minDist = std::numeric_limits<float>::max();
        int bestCenter = -1;
        for (int c : centers) {
            if (dist[v][c] < minDist) {
                minDist = dist[v][c];
                bestCenter = c;
            }
        }
        g.nodes[v].x = g.nodes[bestCenter].x + rand(gen);
        g.nodes[v].y = g.nodes[bestCenter].y + rand(gen);
    }
}
void HarellKoren::localLayout(Graph& g, const std::vector<std::vector<float>>& dist, float radius) {
    int V = g.nodes.size();
    auto neighborhoods = computeKNeighborhoods(g, dist, radius);

    BinHeap<NodeEnergy> heap(V);

    for (int v = 0; v < V; ++v) {
        auto delta_en = computeDeltaK(g, v, dist, neighborhoods[v]);
        heap.push({delta_en.energy, delta_en.dx, delta_en.dy, v});
    }

    for (int iter = 0; iter < cfg_.max_iter * V; ++iter) {

        if (heap.empty())
            break;

        auto top = heap.top();
        int m = top.node;
        float old_x = g.nodes[m].x;
        float old_y = g.nodes[m].y;

        float hxx = 0.0f, hyy = 0.0f, hxy = 0.0f;
        float dx_e = top.dx;
        float dy_e = top.dy;

        for (int i : neighborhoods[m]) {
            if (i == m)
                continue;

            float dx = g.nodes[m].x - g.nodes[i].x;
            float dy = g.nodes[m].y - g.nodes[i].y;
            float dx2 = dx * dx;
            float dy2 = dy * dy;
            float d = sqrtf(dx2 + dy2);
            d = std::max(d, EPSILON);

            float l_mi = L_[m][i];
            float k_mi = K_[m][i];
            float d_mi = dist[m][i];

            float dist3 = d * d * d;
            hxx += 2 * k_mi * (1 - (l_mi * d_mi * dy2) / dist3);
            hyy += 2 * k_mi * (1 - (l_mi * d_mi * dx2) / dist3);
            hxy += 2 * k_mi * l_mi * d_mi * dx * dy / dist3;
        }

        float det = hxx * hyy - (hxy * hxy);
        det = std::max(std::abs(det), EPSILON);
        float delta_x = (-dx_e * hyy + dy_e * hxy) / det;
        float delta_y = (dx_e * hxy - dy_e * hxx) / det;

        g.nodes[m].x += delta_x;
        g.nodes[m].y += delta_y;

        auto node_m = heap.get(m);
        node_m.dx = 0.0f;
        node_m.dy = 0.0f;

        auto computeContribution = [&](float dx, float dy, float k, float l,
                                       float d) -> std::pair<float, float> {
            float dist = std::sqrt(dx * dx + dy * dy);
            dist = std::max(dist, EPSILON);
            float factor = 2.0f * k * (1.0f - (l * d) / dist);
            return {factor * dx, factor * dy};
        };

        for (int u : neighborhoods[m]) {
            if (u == m)
                continue;

            auto node_u = heap.get(u);
            if (node_u.node == -1)
                throw std::runtime_error("Energy node invalid");

            float k_um = K_[u][m];
            float l_um = L_[u][m];
            float d_um = dist[u][m];

            auto [dx_old_u, dy_old_u] =
                computeContribution(g.nodes[u].x - old_x, g.nodes[u].y - old_y, k_um, l_um, d_um);
            node_u.dx -= dx_old_u;
            node_u.dy -= dy_old_u;

            auto [dx_new_u, dy_new_u] = computeContribution(g.nodes[u].x - g.nodes[m].x,
                                                            g.nodes[u].y - g.nodes[m].y, k_um, l_um, d_um);
            node_u.dx += dx_new_u;
            node_u.dy += dy_new_u;

            node_u.energy = sqrtf(node_u.dx * node_u.dx + node_u.dy * node_u.dy);
            node_u.node = u;
            heap.update(u, node_u);

            auto [dx_m, dy_m] = computeContribution(g.nodes[m].x - g.nodes[u].x, g.nodes[m].y - g.nodes[u].y,
                                                    K_[m][u], L_[m][u], dist[m][u]);
            node_m.dx += dx_m;
            node_m.dy += dy_m;
        }

        node_m.energy = sqrtf(node_m.dx * node_m.dx + node_m.dy * node_m.dy);
        heap.update(m, node_m);
    }
}
NodeEnergy HarellKoren::computeDeltaK(const Graph& g, int v, const std::vector<std::vector<float>>& dist,
                                      const std::vector<int>& neighborhood) {

    float dx_e = 0.0f;
    float dy_e = 0.0f;
    for (int u : neighborhood) {
        if (u == v)
            continue;

        float dx = g.nodes[v].x - g.nodes[u].x;
        float dy = g.nodes[v].y - g.nodes[u].y;
        float d = sqrtf(dx * dx + dy * dy);
        d = std::max(d, EPSILON);

        float l_vu = L_[v][u];
        float k_vu = K_[v][u];
        float d_vu = dist[v][u];

        dx_e += 2 * k_vu * dx * (1 - (l_vu * d_vu) / d);
        dy_e += 2 * k_vu * dy * (1 - (l_vu * d_vu) / d);
    }
    auto delta = sqrtf(dx_e * dx_e + dy_e * dy_e);
    auto node_en = NodeEnergy{delta, dx_e, dy_e};
    return node_en;
}

std::vector<std::vector<int>>
HarellKoren::computeKNeighborhoods(const Graph& g, const std::vector<std::vector<float>>& dist, int k) {
    size_t V = g.nodes.size();
    std::vector<std::vector<int>> neighborhoods(V);

    for (size_t v = 0; v < V; ++v) {
        for (size_t u = 0; u < V; ++u) {
            if (v != u && dist[v][u] < k) {
                neighborhoods[v].push_back(u);
            }
        }
    }
    return neighborhoods;
}
std::vector<int> HarellKoren::kCenters(const Graph& g, const std::vector<std::vector<float>>& dist,
                                       size_t k) {
    size_t n = g.nodes.size();
    std::vector<int> centers;
    centers.reserve(k);

    std::vector<float> dist_min(n, std::numeric_limits<float>::max());

    centers.push_back(0);
    for (size_t i = 0; i < n; ++i)
        dist_min[i] = dist[i][0];

    while (centers.size() < k) {
        int n_far = -1;
        float dist_max = -1.0f;

        for (size_t i = 0; i < n; ++i) {
            if (std::find(centers.begin(), centers.end(), i) != centers.end())
                continue;

            dist_min[i] = std::min(dist_min[i], dist[i][centers.back()]);

            if (dist_min[i] > dist_max) {
                dist_max = dist_min[i];
                n_far = i;
            }
        }

        centers.push_back(n_far);
    }

    return centers;
}
void HarellKoren::computeLAndK(const std::vector<std::vector<float>>& dist, float L0, float k) {
    size_t V = dist.size();
    L_.assign(V, std::vector<float>(V, 0.0f));
    K_.assign(V, std::vector<float>(V, 0.0f));

    float maxDist = 0.0f;
    for (size_t i = 0; i < V; ++i)
        for (size_t j = 0; j < V; ++j)
            if (dist[i][j] < std::numeric_limits<float>::infinity())
                maxDist = std::max(maxDist, dist[i][j]);

    for (size_t i = 0; i < V; ++i)
        for (size_t j = 0; j < V; ++j) {
            if (i == j)
                continue;
            if (!std::isfinite(dist[i][j])) {
                L_[i][j] = L0;
                K_[i][j] = 0.0f;
            } else {
                L_[i][j] = L0 * dist[i][j] / maxDist;
                K_[i][j] = k / (dist[i][j] * dist[i][j]);
            }
        }
    // Debug print
    // std::clog << "L_ matrix:\n";
    // for (size_t i = 0; i < V; ++i) {
    //     for (size_t j = 0; j < V; ++j) {
    //         std::clog << L_[i][j] << " ";
    //     }
    //     std::clog << "\n";
    // }
    //
    // std::clog << "\nK_ matrix:\n";
    // for (size_t i = 0; i < V; ++i) {
    //     for (size_t j = 0; j < V; ++j) {
    //         std::clog << K_[i][j] << " ";
    //     }
    //     std::clog << "\n";
    // }
}

float HarellKoren::computeRadius(const std::vector<int>& centers, const std::vector<std::vector<float>>& dist,
                                 float Rad) {
    float radius = 0.0f;
    for (size_t i = 0; i < centers.size(); ++i) {
        float minDist = std::numeric_limits<float>::max();
        for (size_t j = 0; j < centers.size(); ++j) {
            if (i == j)
                continue;
            minDist = std::min(minDist, dist[centers[i]][centers[j]]);
        }
        // FIX
        radius = std::max(radius, float(minDist) * Rad);
    }
    return radius;
}

std::vector<float>
HarellKoren::computeEnergyDeltaAllNodes(Graph& g, const std::vector<std::vector<float>>& d,
                                        const std::vector<std::vector<int>>& neighborhoods) {
    int V = g.nodes.size();
    std::vector<float> nodeEnergy(V);
    for (int m = 0; m < V; ++m) {
        float em = 0.0f;
        float dx_e = 0.0f;
        float dy_e = 0.0f;
        for (int i : neighborhoods[m]) {
            if (m == i) {
                continue;
            }
            float dx = g.nodes[m].x - g.nodes[i].x;
            float dy = g.nodes[m].y - g.nodes[i].y;

            float dist = sqrtf(dx * dx + dy * dy);
            dist = std::max(dist, EPSILON);

            float l_mi = L_[m][i];
            float k_mi = K_[m][i];
            float d_mi = d[m][i];

            dx_e += 2 * k_mi * dx * (1 - (l_mi * d_mi) / dist);
            dy_e += 2 * k_mi * dy * (1 - (l_mi * d_mi) / dist);
        }
        em = sqrtf(dx_e * dx_e + dy_e * dy_e);
        nodeEnergy[m] = em;
    }
    return nodeEnergy;
}
