#include "walshaw.hpp"
#include <iostream>

// TODO: coarsening
void Walshaw::apply(Graph& g) {

    std::clog << ">> Computing Wallshaw\n";
    const size_t V = g.nodes.size();
    if (V == 0) {
        return;
    }
    float A = cfg_.mx * cfg_.my;
    float K = cfg_.C * std::sqrt(A / static_cast<float>(V));
    float T = K;
    R_ = 20 * K;

    bool converged = 0;
    int iter = 0;

    while (!converged && iter < cfg_.max_iter) {

        if (iter % 100 == 0) {
            std::clog << "Iter: " << iter << '\n';
            std::clog << "T: " << T << '\n';
        }

        converged = 1;
        for (size_t i = 0; i < V; i++) {
            Node& v = g.nodes[i];
            float thetaX = 0.0f;
            float thetaY = 0.0f;

            // Repulsive
            for (size_t j = 0; j < V; j++) {
                if (i == j)
                    continue;

                const Node& u = g.nodes[j];

                float dx = u.x - v.x;
                float dy = u.y - v.y;
                float dist = std::sqrt(dx * dx + dy * dy + EPSILON);
                float w = 1.0f;
                float force = fr(dist, w, K, cfg_.C);
                thetaX += (dx / dist) * force;
                thetaY += (dy / dist) * force;
            }

            // Attractive
            for (const auto& n_a : g.adj[i]) {
                const Node& u = g.nodes[n_a.dst];
                float dx = u.x - v.x;
                float dy = u.y - v.y;
                float dist = std::sqrt(dx * dx + dy * dy + EPSILON);
                float force = fa(dist, K);
                thetaX += (dx / dist) * force;
                thetaY += (dy / dist) * force;
            }

            float oldPosX = v.x;
            float oldPosY = v.y;

            float disp = std::sqrt(thetaX * thetaX + thetaY * thetaY + EPSILON);
            float step = std::min(disp, T);
            v.x += (thetaX / disp) * step;
            v.y += (thetaY / disp) * step;

            float dx = oldPosX - v.x;
            float dy = oldPosY - v.y;
            float dist2 = dx * dx + dy * dy;
            float dist = std::sqrt(std::max(dist2, EPSILON));

            if (dist > K * cfg_.tol)
                converged = 0;
        }
        iter++;
        T = cool(T);
    }
    std::clog << "Final T: " << T << '\n';
    std::clog << "Final Iter: " << iter << '\n';
}
