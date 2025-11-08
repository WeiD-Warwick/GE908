#pragma once
#include "../Interfaces/IRenderable.h"
#include "../../Foundation/GEUtility.h"
#include "../../Foundation/GEModel.h"
#include "../../../ThirdParty/GamesEngineeringBase.h"
#include "../../Foundation/GEDebug.h"

using namespace GamesEngineeringBase;

class RenderableComponent : public IRenderable {
private:
    const Image* _image = nullptr;
    GEPoint _origin{ 0,0 };
    GEColliderType _type = GEColliderType::None;

    // ====== HP ======
    float _hpRatio = 1.0f;
    bool _showHP = true;

    // ====== Hurt ======
    GEColor _flashColor{ 255, 60, 60 };
    float _flashTimer = 0.0f;
    float _flashDuration = 0.15f;

public:
    RenderableComponent() = default;

    // ========== Setters ==========
    void setImage(const Image* img) { _image = img; }
    void setOrigin(GEPoint p) { _origin = p; }
    void setType(GEColliderType t) { _type = t; }
    void setHpRatio(float ratio) { _hpRatio = clamp(ratio, 0.0f, 1.0f); }
    void showHP(bool show) { _showHP = show; }

    // ========== Hurt ==========
    void triggerFlash(GEColor color = GEColor(255, 60, 60), float duration = 0.15f) {
        _flashColor = color;
        _flashDuration = duration;
        _flashTimer = duration;
    }

    bool isFlashing() const { return _flashTimer > 0.0f; }

    // ========== Impl ==========

    void updateRender(float dt) override {
        if (_flashTimer > 0.0f)
            _flashTimer = max(0.0f, _flashTimer - dt);
    }

    void render(Window& window, const GECamera& camera) const override {
        if (!_image) return;

        const int camX = camera.getX();
        const int camY = camera.getY();
        const int winW = window.getWidth();
        const int winH = window.getHeight();

        const int originX = _origin.x - camX;
        const int originY = _origin.y - camY;
        const int w = _image->width;
        const int h = _image->height;

        for (int dy = 0; dy < h; ++dy) {
            const int screenY = originY + dy;
            if (screenY < 0 || screenY >= winH) continue;

            for (int dx = 0; dx < w; ++dx) {
                const int screenX = originX + dx;
                if (screenX < 0 || screenX >= winW) continue;

                if (_image->alphaAtUnchecked(dx, dy) > 0)
                    window.draw(screenX, screenY, _image->atUnchecked(dx, dy));
            }
        }

        if (_flashTimer > 0.0f) {
            for (int dy = 0; dy < h; ++dy) {
                const int screenY = originY + dy;
                if (screenY < 0 || screenY >= winH) continue;

                for (int dx = 0; dx < w; ++dx) {
                    const int screenX = originX + dx;
                    if (screenX < 0 || screenX >= winW) continue;
                    if (_image->alphaAtUnchecked(dx, dy) <= 0) continue;

                    window.draw(screenX, screenY, _flashColor.r, _flashColor.g, _flashColor.b);
                }
            }
        }

        if (_showHP)
            drawHP(window, camera);
    }

private:
    void drawHP(Window& window, const GECamera& camera) const {
        if (!_image) return;

        const int camX = camera.getX();
        const int camY = camera.getY();
        const int winW = window.getWidth();
        const int winH = window.getHeight();

        const int barWidth = _image->width;
        const int barHeight = 8;
        const int screenX = _origin.x - camX;
        const int screenY = _origin.y - camY + _image->height + 8;

        if (screenX + barWidth < 0 || screenY + barHeight < 0 || screenX >= winW || screenY >= winH)
            return;

        int left = max(0, screenX);
        int right = min(winW, screenX + barWidth);
        int top = max(0, screenY);
        int bottom = min(winH, screenY + barHeight);

        // ??????
        for (int y = top; y < bottom; ++y)
            for (int x = left; x < right; ++x)
                window.draw(x, y, 60, 60, 60);

        // ????
        const int filledWidth = static_cast<int>((right - left) * _hpRatio);
        for (int y = top; y < bottom; ++y) {
            for (int x = left; x < left + filledWidth; ++x) {
                if (_type == GEColliderType::Player)
                    window.draw(x, y, 30, 255, 30);
                else if (_type == GEColliderType::Enemy)
                    window.draw(x, y, 255, 30, 30);
                else
                    window.draw(x, y, 200, 200, 200);
            }
        }
    }
};
