#pragma once

#include "Event.h"
#include "Object.h"
#include "Rect.h"

class Widget : public Object {
public:
    explicit Widget(Widget* parent = nullptr);
    virtual ~Widget();

    virtual void event(Event&);
    virtual void onPaint(PaintEvent&);
    virtual void onShow(ShowEvent&);
    virtual void onHide(HideEvent&);
    virtual void onKeyDown(KeyEvent&);
    virtual void onKeyUp(KeyEvent&);
    virtual void onMouseMove(MouseEvent&);
    virtual void onMouseDown(MouseEvent&);
    virtual void onMouseUp(MouseEvent&);

    Rect rect() const { return m_rect; }
    int x() const { return rect().x(); }
    int y() const { return rect().y(); }
    int width() const { return rect().width(); }
    int height() const { return rect().height(); }

    void update();

    struct HitTestResult {
        Widget* widget { nullptr };
        int localX { 0 };
        int localY { 0 };
    };
    HitTestResult hitTest(int x, int y);

    virtual const char* className() const override { return "Widget"; }

    void setRect(const Rect&);

private:
    Rect m_rect;
};