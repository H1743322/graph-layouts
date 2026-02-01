#pragma once

struct Camera2D {
    float x = 0.0f;
    float y = 0.0f;
    float zoom = 1.0f;
    float rotation = 0.0f;

    void moveBy(float dx, float dy) {
        x += dx;
        y += dy;
    }
    void zoomBy(float f) { zoom *= f; }
    void rotateBy(float dtheta) { rotation += dtheta; }
};
