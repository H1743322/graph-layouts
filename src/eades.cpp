
#include "eades.hpp"
#include "graph.hpp"
#include <algorithm>
#include <cmath>

void Eades::apply(Graph& g) {
    auto V = g.nodes.size();

    for (int iter = 0; iter < cfg_.max_iter; ++iter) {
        g.resetForces();

        // Repulsive forces
        for (size_t i = 0; i < V; ++i) {
            for (size_t j = i + 1; j < V; ++j) {
                Node& vi = g.nodes[i];
                Node& vj = g.nodes[j];

                float dx = vi.x - vj.x;
                float dy = vi.y - vj.y;
                float dist2 = dx * dx + dy * dy;
                float dist = std::sqrt(std::max(dist2, EPSILON));
                dist = std::max(dist, EPSILON);

                float force = repelForce(dist);
                float fx = (dx / dist) * force;
                float fy = (dy / dist) * force;

                vi.dx += fx;
                vi.dy += fy;
                vj.dx -= fx;
                vj.dy -= fy;
            }

            // Attractive forces
            for (const auto& e : g.adj[i]) {
                Node& src = g.nodes[i];
                Node& dst = g.nodes[e.dst];

                float dx = src.x - dst.x;
                float dy = src.y - dst.y;
                float dist2 = dx * dx + dy * dy;
                float dist = std::sqrt(std::max(dist2, EPSILON));
                dist = std::max(dist, EPSILON);

                float force = attractForce(dist);
                float fx = (dx / dist) * force;
                float fy = (dy / dist) * force;

                src.dx -= fx;
                src.dy -= fy;
                if (!g.directed) {
                    dst.dx += fx;
                    dst.dy += fy;
                }
            }
        }

        // Update positions
        for (auto& node : g.nodes) {
            node.x += cfg_.c4 * node.dx;
            node.y += cfg_.c4 * node.dy;
        }
    }
}

float Eades::repelForce(float d) {
    d = std::max(d, EPSILON);
    return cfg_.c3 / (d * d);
}

float Eades::attractForce(float d) {
    d = std::max(d, EPSILON);
    return cfg_.c1 * std::log(d / cfg_.c2);
}
