#include "CurveWidget.h"

#include "utils/CurveLoader.h"

#include <QPainter>
#include <QPen>
#include <algorithm>

CurveWidget::CurveWidget(QWidget* parent) : QWidget(parent) {}

void CurveWidget::loadCurve(const QString& filename, bool isFirst) {
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

void CurveWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (curveA.isEmpty() || curveB.isEmpty())
        return;

    drawCurve(painter, aIsTop ? curveA : curveB, aIsTop ? curveB : curveA, true);
    drawCurve(painter, aIsTop ? curveB : curveA, aIsTop ? curveA : curveB, false);
}

void CurveWidget::drawCurve(
    QPainter& painter, const QVector<QPointF>& curve, const QVector<QPointF>& other, bool isTop) {
    if (curve.size() < 2)
        return;

    QPen pen(isTop ? Qt::blue : Qt::red, 2);
    painter.setPen(pen);

    for (int i = 1; i < curve.size(); ++i) {
        QPointF p1 = curve[i - 1];
        QPointF p2 = curve[i];

        QPointF o1 = other[i - 1];
        QPointF o2 = other[i];

        bool above = (p1.y() < o1.y()) && (p2.y() < o2.y());
        if (isTop)
            painter.setPen(QPen(Qt::blue, 2, above ? Qt::SolidLine : Qt::DashLine));
        else
            painter.setPen(QPen(Qt::red, 2, above ? Qt::DashLine : Qt::SolidLine));

        painter.drawLine(p1, p2);
    }
}
