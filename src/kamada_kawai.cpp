#include "kamada_kawai.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

void KamadaKawai::apply(Graph& g) {
    std::clog << ">> Computing KamadaKawai\n";
    size_t V = g.nodes.size();
    if (V == 0)
        return;

    float L0 = std::max(cfg_.mx, cfg_.my) / 2;
    auto dist = g.computeAllPairsShortestPaths();
    computeLAndK(dist, L0, cfg_.K);
    auto en_i = computeEnergy(g);
    std::clog << "Initial Energy: " << en_i << "\n";

    for (int iter = 0; iter < cfg_.max_iter; ++iter) {

        // PERF: dont always copy
        // just return value and index
        auto nodeEnergy = computeEnergyAllNodes(g);

        auto it = std::max_element(nodeEnergy.begin(), nodeEnergy.end());
        size_t m = std::distance(nodeEnergy.begin(), it);

        if (*it < EPSILON) {
            break;
        }

        for (int iter_2 = 0; iter_2 < cfg_.max_iter_2; ++iter_2) {
            float hxx = 0.0f;
            float hyy = 0.0f;
            float hxy = 0.0f;
            float dx_e = 0.0f;
            float dy_e = 0.0f;
            for (size_t i = 0; i < V; ++i) {
                if (i == m)
                    continue;

                float dx = g.nodes[m].x - g.nodes[i].x;
                float dy = g.nodes[m].y - g.nodes[i].y;
                float dx2 = dx * dx;
                float dy2 = dy * dy;
                float dist = std::sqrt(dx2 + dy2);
                dist = std::max(dist, EPSILON);

                float l_mi = L_[m][i];
                float k_mi = K_[m][i];

                float dist3 = dist * dist * dist;
                hxx += k_mi * (1 - (l_mi * dy2) / dist3);
                hyy += k_mi * (1 - (l_mi * dx2) / dist3);
                hxy += k_mi * ((l_mi * dx * dy) / dist3);

                dx_e += k_mi * (dx - ((l_mi * dx) / dist));
                dy_e += k_mi * (dy - ((l_mi * dy) / dist));
            }

            float det = hxx * hyy - (hxy * hxy);
            float delta_x = (-dx_e * hyy + dy_e * hxy) / det;
            float delta_y = (dx_e * hxy - dy_e * hxx) / det;

            float delta = std::sqrt(delta_x * delta_x + delta_y * delta_y);

            g.nodes[m].x += delta_x;
            g.nodes[m].y += delta_y;

            if (delta < EPSILON) {
                break;
            }
        }
    }
    auto en_f = computeEnergy(g);
    std::clog << "Final Energy: " << en_f << "\n";
}

float KamadaKawai::computeEnergy(Graph& g) {
    size_t V = g.nodes.size();
    std::vector<float> nodeEnergy(V);
    float energy = 0.0f;
    for (size_t m = 0; m < V; ++m) {
        for (size_t i = 0; i < V; ++i) {
            if (m <= i) {
                continue;
            }
            float dx = g.nodes[m].x - g.nodes[i].x;
            float dy = g.nodes[m].y - g.nodes[i].y;

            float dist = std::sqrt(dx * dx + dy * dy);
            dist = std::max(dist, EPSILON);

            float delta = dist - L_[m][i];
            energy += 0.5f * K_[m][i] * delta * delta;
        }
    }
    return energy;
}
std::vector<float> KamadaKawai::computeEnergyAllNodes(Graph& g) {
    size_t V = g.nodes.size();
    std::vector<float> nodeEnergy(V);
    for (size_t m = 0; m < V; ++m) {
        float em = 0.0f;
        float dx_e = 0.0f;
        float dy_e = 0.0f;
        for (size_t i = 0; i < V; ++i) {
            if (m == i) {
                continue;
            }
            float dx = g.nodes[m].x - g.nodes[i].x;
            float dy = g.nodes[m].y - g.nodes[i].y;

            float dist = std::sqrt(dx * dx + dy * dy);
            dist = std::max(dist, EPSILON);

            float l_mi = L_[m][i];
            float k_mi = K_[m][i];
            dx_e += k_mi * (dx - ((l_mi * dx) / dist));
            dy_e += k_mi * (dy - ((l_mi * dy) / dist));
        }
        em = std::sqrt(dx_e * dx_e + dy_e * dy_e);
        nodeEnergy[m] = em;
    }
    return nodeEnergy;
}

void KamadaKawai::computeLAndK(const std::vector<std::vector<float>>& dist, float L0, float k) {
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
                L_[i][j] *= cfg_.multL;
                K_[i][j] = k / (dist[i][j] * dist[i][j]);
            }
        }
}
