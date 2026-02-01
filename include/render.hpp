#pragma once
#include "camera.hpp"
#include "graph.hpp"

class Render {
  public:
    explicit Render(Graph* g);

    void setWindowSize(float w, float h);
    void renderGraph(const Camera2D& camera, float nodeSize);

  private:
    void applyCameraTransform(const Camera2D& camera);
    void renderEdges();
    void renderNodes(float nodeSize, const Camera2D& cam);
    void renderAxes(float extent);

  private:
    Graph* graph_;
    float w_ = 1.0f;
    float h_ = 1.0f;
};
