#include "CurveWidget.h"

#include <QPainter>
#include <QPen>
#include <algorithm>
#include <optional>
#include "CurveLoader.h"
#include "QDebug"

CurveWidget::CurveWidget(QWidget *parent) : QWidget(parent) {}

void CurveWidget::loadCurve(const QString &filename, bool isFirst) {
    auto data = CurveLoader::loadFromFile(filename);

    if (isFirst)
        curveA = data;
    else
        curveB = data;
    update();
}

void CurveWidget::switchCurves() {
    aIsTop = !aIsTop;
    update();
}

void CurveWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setWindow(0, 0, width(), height());
    drawCurves(painter, curveA, curveB, aIsTop);
}

static double cross(const QPointF &a, const QPointF &b) {
    return a.x() * b.y() - a.y() * b.x();
}

static std::optional<QPointF>
segmentIntersection(const QPair<QPointF, QPointF> &seg1,
                    const QPair<QPointF, QPointF> &seg2) {
    const QPointF &p1 = seg1.first;
    const QPointF &p2 = seg1.second;
    const QPointF &q1 = seg2.first;
    const QPointF &q2 = seg2.second;

    QPointF r = p2 - p1;
    QPointF s = q2 - q1;
    double rxs = cross(r, s);
    if (std::abs(rxs) < 1e-12)
        return std::nullopt;

    QPointF qp = q1 - p1;
    double t = cross(qp, s) / rxs;
    double u = cross(qp, r) / rxs;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        return p1 + t * r;
    }
    return std::nullopt;
}

QVector<QPair<QPointF, QPointF>>
CurveWidget::makeSegments(const QVector<QPointF> &curve) {
    QVector<QPair<QPointF, QPointF>> segments;
    for (int i = 1; i < curve.size(); ++i) {
        segments.append(qMakePair(curve[i - 1], curve[i]));
    }
    return segments;
}

QVector<QPointF> CurveWidget::findIntersectionsTwoPointers(
        const QVector<QPointF> &curve, const QVector<QPointF> &other) {
    QVector<QPointF> result;

    auto segments1 = makeSegments(curve);
    auto segments2 = makeSegments(other);

    size_t i = 0, j = 0;
    while (i < segments1.size() && j < segments2.size()) {
        auto &s1 = segments1[i];
        auto &s2 = segments2[j];

        if (s1.second.x() >= s2.first.x() && s2.second.x() >= s1.first.x()) {
            auto intersection = segmentIntersection(s1, s2);
            if (intersection.has_value()) {
                result.push_back(intersection.value());
            }
        }

        if (s1.second.x() < s2.second.x())
            i++;
        else
            j++;
    }

    return result;
}

Normalizer::Normalizer(const QVector<QPointF> &curve,
                       const QVector<QPointF> &other,
                       int width,
                       int height) :
    width(width), height(height) {
    minX = curve[0].x();
    maxX = curve[0].x();
    minY = curve[0].y();
    maxY = curve[0].y();

    for (const auto &pt: curve + other) {
        minX = std::min(minX, pt.x());
        maxX = std::max(maxX, pt.x());
        minY = std::min(minY, pt.y());
        maxY = std::max(maxY, pt.y());
    }

    rangeX = (maxX - minX == 0) ? 1 : (maxX - minX);
    rangeY = (maxY - minY == 0) ? 1 : (maxY - minY);
}

QPointF Normalizer::normalize(const QPointF &p) const {
    double x = (p.x() - minX) / rangeX * width;
    double y = height - (p.y() - minY) / rangeY * height;
    return QPointF(x, y);
}

void CurveWidget::drawCurve(QPainter &painter,
                            const QVector<QPointF> &curve,
                            const QVector<QPointF> &intersections,
                            bool isTop,
                            const Normalizer &normalizer) {
    if (curve.size() < 2)
        return;

    Qt::PenStyle solid = Qt::SolidLine;
    Qt::PenStyle dash = Qt::DashLine;

    QPen pen(isTop ? Qt::red : Qt::green, 2);
    painter.setPen(pen);

    int interIdx = 0;
    bool drawSolid = isTop;

    for (int i = 1; i < curve.size(); ++i) {
        QPointF p1 = curve[i - 1];
        QPointF p2 = curve[i];

        while (interIdx < intersections.size() &&
               intersections[interIdx].x() >= std::min(p1.x(), p2.x()) &&
               intersections[interIdx].x() <= std::max(p1.x(), p2.x())) {
            QPointF intersection = intersections[interIdx];
            QPointF normP1 = normalizer.normalize(p1);
            QPointF normInter = normalizer.normalize(intersection);

            pen.setStyle(drawSolid ? solid : dash);
            painter.setPen(pen);
            painter.drawLine(normP1, normInter);

            p1 = intersection;
            interIdx++;
            drawSolid = !drawSolid;
        }

        QPointF normP1 = normalizer.normalize(p1);
        QPointF normP2 = normalizer.normalize(p2);
        pen.setStyle(drawSolid ? solid : dash);
        painter.setPen(pen);
        painter.drawLine(normP1, normP2);
    }
}

void CurveWidget::drawCurves(QPainter &painter,
                             const QVector<QPointF> &curve,
                             const QVector<QPointF> &other,
                             bool isTop) {
    if (curve.isEmpty() && other.isEmpty())
        return;
    const QVector<QPointF> &primary =
            (curve.size() >= other.size()) ? curve : other;
    const QVector<QPointF> &secondary =
            (curve.size() >= other.size()) ? other : curve;

    Normalizer normalizer(primary, secondary, this->width(), this->height());

    int n = primary.size();

    if (secondary.isEmpty()) {
        QPen pen(isTop ? Qt::blue : Qt::red, 2, Qt::SolidLine);
        painter.setPen(pen);
        for (int i = 1; i < n; ++i) {
            painter.drawLine(normalizer.normalize(primary[i - 1]),
                             normalizer.normalize(primary[i]));
        }
        return;
    }

    auto intersections = findIntersectionsTwoPointers(primary, secondary);

    drawCurve(painter, primary, intersections, isTop, normalizer);
    drawCurve(painter, secondary, intersections, !isTop, normalizer);
}
CurveWidget::~CurveWidget() = default;
