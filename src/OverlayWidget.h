#pragma once
#include <QWidget>
#include <QPainterPath>
#include <vector>
#include <QTimer>
#include <QMap>

class OverlayWidget : public QWidget {
    Q_OBJECT
public:
    enum class Tool { Freehand, Line, Rect, Ellipse, Triangle, Eraser };
    explicit OverlayWidget(QWidget* parent = nullptr);
    void setTool(Tool t);
    void setBackgroundImage(const QImage& img);
protected:
    bool event(QEvent* ev) override;
    void paintEvent(QPaintEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;

    void mousePressEvent(QMouseEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;
private:
    struct Stroke { QPainterPath path; QPen pen; };
    std::vector<Stroke> strokes;
    QPainterPath currentPath;
    QImage background;
    Tool currentTool;
    QPointF startPt;
    QMap<int, QTouchEvent::TouchPoint> touchPoints;
    QTimer* twoFingerTimer;
    bool twoFingerLongPressCandidate;
    QRectF twoFingerTargetRect;
    void handleTouchBegin(QTouchEvent* tev);
    void handleTouchUpdate(QTouchEvent* tev);
    void handleTouchEnd(QTouchEvent* tev);
    void eraseAt(const QRectF& area);
    void commitCurrentShape();
};