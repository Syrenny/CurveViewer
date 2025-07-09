#pragma once

#include <QPointF>
#include <QVector>
#include <QWidget>

class Normalizer {
public:
    explicit Normalizer(const QVector<QPointF> &curve,
                        const QVector<QPointF> &other,
                        int width,
                        int height);

    QPointF normalize(const QPointF &p) const;

private:
    double minX{}, maxX{}, minY{}, maxY{};
    double rangeX{}, rangeY{};
    int width{}, height{};
};

class CurveWidget : public QWidget {
    Q_OBJECT

public:
    explicit CurveWidget(QWidget *parent = nullptr);
    void loadCurve(const QString &filename, bool isFirst);
    void switchCurves();
    ~CurveWidget() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QPointF> curveA;
    QVector<QPointF> curveB;
    bool aIsTop = true;
    static QVector<QPair<QPointF, QPointF>>
    makeSegments(const QVector<QPointF> &curve);
    static QVector<QPointF> findIntersectionsTwoPointers(
            const QVector<QPointF> &curve, const QVector<QPointF> &other);
    void drawCurve(QPainter &painter,
                   const QVector<QPointF> &curve,
                   const QVector<QPointF> &intersections,
                   bool isTop,
                   const Normalizer &normalizer);
    void drawCurves(QPainter &painter,
                    const QVector<QPointF> &curve,
                    const QVector<QPointF> &other,
                    bool isTop);
};
