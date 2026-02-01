#include "render.hpp"
#include <GL/gl.h>
#include <cassert>

Render::Render(Graph* g) : graph_(g) {}

void Render::setWindowSize(float w, float h) {
    w_ = w;
    h_ = h;
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void Render::applyCameraTransform(const Camera2D& camera) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, w_, 0.0f, h_, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(w_ * 0.5f, h_ * 0.5f, 0.0f);
    glScalef(camera.zoom, camera.zoom, 1.0f);
    glTranslatef(-camera.x, -camera.y, 0.0f);
}

void Render::renderEdges() {
    assert(graph_->nodes.size() == graph_->adj.size());

    glColor3f(1.f, 1.f, 1.f);
    glBegin(GL_LINES);

    for (size_t i = 0; i < graph_->adj.size(); ++i) {
        const auto& src = graph_->nodes[i];
        for (const auto& e : graph_->adj[i]) {
            if (i <= e.dst) {
                const auto& dst = graph_->nodes[e.dst];
                glVertex2f(src.x, src.y);
                glVertex2f(dst.x, dst.y);
            }
        }
    }

    glEnd();
}
void Render::renderAxes(float max) {
    glLineWidth(1.5f);
    glBegin(GL_LINES);

    glColor3f(1.f, 0.f, 0.f);
    glVertex2f(-max, 0.f);
    glVertex2f(max, 0.f);

    glColor3f(0.f, 1.f, 0.f);
    glVertex2f(0.f, -max);
    glVertex2f(0.f, max);

    glEnd();
}
void Render::renderNodes(float nodeSize, const Camera2D& camera) {
    if (nodeSize < 0.1f)
        return;
    glPointSize(nodeSize * camera.zoom);
    glColor3f(1.f, 0.f, 0.f);

    glBegin(GL_POINTS);
    for (const auto& n : graph_->nodes)
        glVertex2f(n.x, n.y);
    glEnd();
}

void Render::renderGraph(const Camera2D& camera, float nodeSize) {
    glDisable(GL_DEPTH_TEST);
    applyCameraTransform(camera);
    renderAxes(10'000.0f);
    renderEdges();
    renderNodes(nodeSize, camera);
}
