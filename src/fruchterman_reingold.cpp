#include "fruchterman_reingold.hpp"
#include "graph.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

void FruchtermanReingold::computeRepulsiveForces(Graph& g) {

    const size_t V = g.nodes.size();
    for (size_t i = 0; i < V; ++i) {
        Node& vi = g.nodes[i];
        for (size_t j = 0; j < V; ++j) {
            if (i == j) {
                continue;
            }
            Node& vj = g.nodes[j];

            float dx = vi.x - vj.x;
            float dy = vi.y - vj.y;
            float dist2 = dx * dx + dy * dy;
            float dist = std::sqrt(std::max(dist2, EPSILON));

            float force = fr(dist, K_);
            float fx = (dx / dist) * force;
            float fy = (dy / dist) * force;

            vi.dx += fx;
            vi.dy += fy;
        }
    }
}
void FruchtermanReingold::computeAttractiveForces(Graph& g) {
    const size_t V = g.nodes.size();
    for (size_t i = 0; i < V; ++i) {
        Node& src = g.nodes[i];
        for (const auto& e : g.adj[i]) {
            Node& dst = g.nodes[e.dst];

            float dx = src.x - dst.x;
            float dy = src.y - dst.y;
            float dist2 = dx * dx + dy * dy;
            float dist = std::sqrt(std::max(dist2, EPSILON));

            float force = fa(dist, K_);
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
}
void FruchtermanReingold::updatePositions(Graph& g) {
    for (auto& n : g.nodes) {
        float dx = n.dx;
        float dy = n.dy;
        float disp2 = dx * dx + dy * dy;
        float disp = std::sqrt(disp2);

        if (disp > EPSILON) {
            float move = std::min(disp, T_);
            n.x += (dx / disp) * move;
            n.y += (dy / disp) * move;
        }

        // n.x = std::clamp(n.x, -mx_ / 2.0f, mx_ / 2.0f);
        // n.y = std::clamp(n.y, -my_ / 2.0f, my_ / 2.0f);
    }
}

void FruchtermanReingold::apply(Graph& g) {

    std::clog << ">> Computing Fruchterman Reingold\n";
    const size_t V = g.nodes.size();
    if (V == 0) {
        return;
    }
    A_ = cfg_.mx * cfg_.my;
    K_ = cfg_.C * std::sqrt(A_ / static_cast<float>(V));
    T_ = cfg_.mx / 10.0f;
    I_ = 0;

    std::clog << "T initial: " << T_ << '\n';

    for (int iter = 0; iter < cfg_.max_iter; ++iter) {
        g.resetForces();
        if (iter % 100 == 0) {
            std::clog << "Iteration: " << iter << '\n';
        }

        computeRepulsiveForces(g);
        computeAttractiveForces(g);
        updatePositions(g);

        T_ = cool(T_);
    }
}
