#pragma once

#include <QPointF>
#include <QString>
#include <QVector>

class CurveLoader {
public:
    static QVector<QPointF> loadFromFile(const QString& filename);
};
