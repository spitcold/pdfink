#include "OverlayWidget.h"
#include <QTouchEvent>
#include <QMouseEvent>
#include <QDebug>

OverlayWidget::OverlayWidget(QWidget* parent)
    : QWidget(parent), currentTool(Tool::Freehand), twoFingerTimer(new QTimer(this)), twoFingerLongPressCandidate(false)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_StaticContents, false);

    twoFingerTimer->setSingleShot(true);
    twoFingerTimer->setInterval(600);
    connect(twoFingerTimer, &QTimer::timeout, [this](){
        if (touchPoints.size() == 2) {
            twoFingerLongPressCandidate = true;
            auto it = touchPoints.begin();
            QPointF a = it.value().pos();
            ++it;
            QPointF b = it.value().pos();
            twoFingerTargetRect = QRectF(a, b).normalized().adjusted(-20,-20,20,20);
            update();
        }
    });
}

void OverlayWidget::setTool(Tool t) { currentTool = t; }
void OverlayWidget::setBackgroundImage(const QImage& img) { background = img; update(); }

bool OverlayWidget::event(QEvent* ev)
{
    if (ev->type() == QEvent::TouchBegin || ev->type() == QEvent::TouchUpdate || ev->type() == QEvent::TouchEnd) {
        QTouchEvent* tev = static_cast<QTouchEvent*>(ev);
        if (tev->type() == QEvent::TouchBegin) handleTouchBegin(tev);
        else if (tev->type() == QEvent::TouchUpdate) handleTouchUpdate(tev);
        else handleTouchEnd(tev);
        return true;
    } else if (ev->type() == QEvent::Gesture) {
        return true;
    }
    return QWidget::event(ev);
}

void OverlayWidget::handleTouchBegin(QTouchEvent* tev)
{
    for (const QTouchEvent::TouchPoint& tp : tev->points()) {
        touchPoints[tp.id()] = tp;
    }
    if (touchPoints.size() == 2) {
        twoFingerTimer->start();
    } else {
        twoFingerTimer->stop();
        twoFingerLongPressCandidate = false;
    }

    for (const QTouchEvent::TouchPoint& tp : tev->points()) {
        QRectF r = tp.rect();
        if (r.width() > 60 || r.height() > 60) {
            QRectF eraseArea(tp.pos() - QPointF(30,30), QSizeF(60,60));
            eraseAt(eraseArea);
        } else if (currentTool == Tool::Freehand && tp.state() == Qt::TouchPointPressed) {
            currentPath = QPainterPath();
            currentPath.moveTo(tp.pos());
        }
    }
    update();
}

void OverlayWidget::handleTouchUpdate(QTouchEvent* tev)
{
    for (const QTouchEvent::TouchPoint& tp : tev->points()) {
        touchPoints[tp.id()] = tp;
        if (currentTool == Tool::Freehand && tp.state() == Qt::TouchPointMoved) {
            currentPath.lineTo(tp.pos());
        }
    }
    update();
}

void OverlayWidget::handleTouchEnd(QTouchEvent* tev)
{
    for (const QTouchEvent::TouchPoint& tp : tev->points()) {
        touchPoints.remove(tp.id());
    }
    if (!currentPath.isEmpty()) {
        Stroke s;
        s.path = currentPath;
        s.pen = QPen(Qt::red, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        strokes.push_back(std::move(s));
        currentPath = QPainterPath();
    }
    twoFingerTimer->stop();
    twoFingerLongPressCandidate = false;
    update();
}

void OverlayWidget::mousePressEvent(QMouseEvent* ev)
{
    if (ev->button() != Qt::LeftButton) return;
    startPt = ev->pos();
    if (currentTool == Tool::Freehand) {
        currentPath = QPainterPath();
        currentPath.moveTo(ev->pos());
    }
}

void OverlayWidget::mouseMoveEvent(QMouseEvent* ev)
{
    if (ev->buttons() & Qt::LeftButton) {
        if (currentTool == Tool::Freehand) {
            currentPath.lineTo(ev->pos());
            update();
        }
    }
}

void OverlayWidget::mouseReleaseEvent(QMouseEvent* ev)
{
    Q_UNUSED(ev)
    commitCurrentShape();
}

void OverlayWidget::commitCurrentShape()
{
    if (!currentPath.isEmpty()) {
        Stroke s;
        s.path = currentPath;
        s.pen = QPen(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        strokes.push_back(std::move(s));
        currentPath = QPainterPath();
        update();
    }
}

void OverlayWidget::eraseAt(const QRectF& area)
{
    std::vector<Stroke> remain;
    for (auto &s : strokes) {
        if (s.path.boundingRect().intersects(area)) {
            // erased
        } else remain.push_back(s);
    }
    strokes.swap(remain);
    update();
}

void OverlayWidget::paintEvent(QPaintEvent* ev)
{
    Q_UNUSED(ev)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    if (!background.isNull()) {
        p.drawImage(rect(), background);
    }

    for (const auto &s : strokes) {
        p.setPen(s.pen);
        p.drawPath(s.path);
    }
    if (!currentPath.isEmpty()) {
        p.setPen(QPen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPath(currentPath);
    }

    if (twoFingerLongPressCandidate) {
        p.setPen(QPen(Qt::yellow, 2, Qt::DashLine));
        p.drawRect(twoFingerTargetRect);
    }
}

void OverlayWidget::resizeEvent(QResizeEvent* ev)
{
    Q_UNUSED(ev)
}