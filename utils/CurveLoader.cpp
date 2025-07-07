#include "CurveLoader.h"

#include <QFile>
#include <QTextStream>

QVector<QPointF> CurveLoader::loadFromFile(const QString& filename) {
    QVector<QPointF> points;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return points;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        auto parts   = line.split('\t');
        if (parts.size() >= 2)
            points.append(QPointF(parts[0].toDouble(), parts[1].toDouble()));
    }

    return points;
}
