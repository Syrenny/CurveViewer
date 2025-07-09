#pragma once

#include <QVector>
#include <QWidget>

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

    void drawCurves(QPainter &painter,
                    const QVector<QPointF> &curve,
                    const QVector<QPointF> &other,
                    bool isTop);
};
