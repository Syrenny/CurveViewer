#include "CurveWidget.h"

#include <QPainter>
#include <QPen>
#include <algorithm>
#include <optional>
#include <set>
#include <vector>
#include "CurveLoader.h"
#include "QDebug"

CurveWidget::CurveWidget(QWidget *parent) : QWidget(parent) {}

void CurveWidget::loadCurve(const QString &filename, bool isFirst) {
    auto data = CurveLoader::loadFromFile(filename);

    qDebug() << "Loaded curve with" << data.size() << "points";

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

// helper: cross product for QPointF
static double cross(const QPointF &a, const QPointF &b) {
    return a.x() * b.y() - a.y() * b.x();
}

// Глобальная переменная для текущей позиции sweep line
static double sweepX = 0.0;

// Функция вычисления Y точки пересечения сегмента с вертикальной линией x =
// sweepX
static double getYatX(const QPointF &p1, const QPointF &p2, double x) {
    if (std::abs(p1.x() - p2.x()) < 1e-12)
        return p1.y();
    double t = (x - p1.x()) / (p2.x() - p1.x());
    return p1.y() + t * (p2.y() - p1.y());
}

// Структура для сегмента с дополнительными данными
struct SegmentInfo {
    const QVector<QPointF>
            *curve; // Указатель на кривую, чтобы идентифицировать
    int startIndex; // индекс начала сегмента (curve[startIndex] ->
                    // curve[startIndex+1])

    bool operator<(const SegmentInfo &other) const {
        // Компаратор для std::set на основе Y координаты пересечения с
        // вертикальной линией sweepX
        double y1 =
                getYatX((*curve)[startIndex], (*curve)[startIndex + 1], sweepX);
        double y2 = getYatX((*other.curve)[other.startIndex],
                            (*other.curve)[other.startIndex + 1],
                            sweepX);
        if (std::abs(y1 - y2) > 1e-12)
            return y1 < y2;
        // Если Y равны — сравним адреса и индексы, чтобы избежать равенства и
        // undefined behavior
        if (curve != other.curve)
            return curve < other.curve;
        return startIndex < other.startIndex;
    }
};

// События sweep line
enum EventType { Start = 0, End = 1 };

struct Event {
    double x;
    EventType type;
    SegmentInfo segment;

    bool operator<(const Event &other) const {
        if (std::abs(x - other.x) > 1e-12)
            return x < other.x;
        return type < other.type; // Start перед End
    }
};

// Функция проверки пересечения (у тебя есть segmentIntersection)
// Используем твою, если нужно, можно вынести сюда

// Проверка пересечения двух сегментов (с твоей функцией segmentIntersection)
static std::optional<QPointF>
segmentIntersection(const QPointF &p1,
                    const QPointF &p2,
                    const QPointF &q1,
                    const QPointF &q2) {
    QPointF r = p2 - p1;
    QPointF s = q2 - q1;
    double rxs = cross(r, s);
    if (std::abs(rxs) < 1e-12) // параллельны или коллинеарны
        return std::nullopt;

    QPointF qp = q1 - p1;
    double t = cross(qp, s) / rxs;
    double u = cross(qp, r) / rxs;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        return p1 + t * r;
    }
    return std::nullopt;
}

struct Intersection {
    int segmentIndex;
    QPointF point;
};

static std::vector<Intersection> findIntersectionsSweepLine(
        const QVector<QPointF> &curve1, const QVector<QPointF> &curve2) {
    if (curve1.isEmpty() || curve2.isEmpty())
        return {};

    std::vector<Event> events;

    for (int i = 0; i + 1 < curve1.size(); ++i) {
        double x1 = curve1[i].x();
        double x2 = curve1[i + 1].x();
        Event startEvent{std::min(x1, x2), EventType::Start, {&curve1, i}};
        Event endEvent{std::max(x1, x2), EventType::End, {&curve1, i}};
        events.push_back(startEvent);
        events.push_back(endEvent);
    }

    for (int i = 0; i + 1 < curve2.size(); ++i) {
        double x1 = curve2[i].x();
        double x2 = curve2[i + 1].x();
        Event startEvent{std::min(x1, x2), EventType::Start, {&curve2, i}};
        Event endEvent{std::max(x1, x2), EventType::End, {&curve2, i}};
        events.push_back(startEvent);
        events.push_back(endEvent);
    }

    std::sort(events.begin(), events.end());

    std::set<SegmentInfo> activeSet;
    std::vector<Intersection> intersections;

    auto checkIntersection = [&](const SegmentInfo &a, const SegmentInfo &b) {
        if (a.curve == b.curve)
            return;

        auto p1 = (*a.curve)[a.startIndex];
        auto p2 = (*a.curve)[a.startIndex + 1];
        auto q1 = (*b.curve)[b.startIndex];
        auto q2 = (*b.curve)[b.startIndex + 1];

        auto inter = segmentIntersection(p1, p2, q1, q2);
        if (inter.has_value()) {
            // Если a.curve == curve1, то segmentIndex = a.startIndex
            if (a.curve == &curve1) {
                intersections.push_back({a.startIndex, inter.value()});
            } else if (b.curve == &curve1) {
                intersections.push_back({b.startIndex, inter.value()});
            }
            // Если пересекаются сегменты обеих кривых, обязательно добавим с
            // индексом из curve1
        }
    };

    for (const auto &event: events) {
        sweepX = event.x;

        if (event.type == EventType::Start) {
            auto [it, inserted] = activeSet.insert(event.segment);
            if (!inserted)
                continue;

            auto next = std::next(it);
            if (next != activeSet.end())
                checkIntersection(*it, *next);
            if (it != activeSet.begin()) {
                auto prev = std::prev(it);
                checkIntersection(*it, *prev);
            }
        } else {
            auto it = activeSet.find(event.segment);
            if (it == activeSet.end())
                continue;

            auto next = std::next(it);
            auto prev =
                    (it == activeSet.begin()) ? activeSet.end() : std::prev(it);

            if (next != activeSet.end() && prev != activeSet.end())
                checkIntersection(*prev, *next);

            activeSet.erase(it);
        }
    }

    // Сортируем пересечения по индексу сегмента
    std::sort(intersections.begin(),
              intersections.end(),
              [](const Intersection &a, const Intersection &b) {
                  return a.segmentIndex < b.segmentIndex;
              });

    return intersections;
}

class Normalizer {
public:
    Normalizer(const QVector<QPointF> &curve,
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

    QPointF normalize(const QPointF &p) const {
        double x = (p.x() - minX) / rangeX * width;
        double y = height - (p.y() - minY) / rangeY * height; // инверсия Y
        return QPointF(x, y);
    }

private:
    double minX{}, maxX{}, minY{}, maxY{};
    double rangeX{}, rangeY{};
    int width{}, height{};
};

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

    auto intersections = findIntersectionsSweepLine(primary, secondary);
    std::sort(intersections.begin(),
              intersections.end(),
              [](const Intersection &a, const Intersection &b) {
                  return a.segmentIndex < b.segmentIndex;
              });

    int m = secondary.size();

    int interIdx = 0;
    Qt::PenStyle solid = Qt::SolidLine;
    Qt::PenStyle dash = Qt::DashLine;

    int i = 1;
    for (; i < n; ++i) {
        QPointF p1 = normalizer.normalize(primary[i - 1]);
        QPointF p2 = normalizer.normalize(primary[i]);

        bool hasIntersection =
                (interIdx < static_cast<int>(intersections.size()) &&
                 intersections[interIdx].segmentIndex == i - 1);

        if (hasIntersection && i < m) {
            QPointF interPoint = intersections[interIdx].point;
            QPointF normInterPoint = normalizer.normalize(interPoint);

            double x_inter = interPoint.x();

            double y_curve = getYatX(primary[i - 1], primary[i], x_inter);
            double y_other = getYatX(secondary[i - 1], secondary[i], x_inter);

            bool isCurveLower =
                    isTop ? (y_curve > y_other) : (y_curve < y_other);

            Qt::PenStyle style1 = isCurveLower ? dash : solid;
            Qt::PenStyle style2 = isCurveLower ? solid : dash;

            QPen pen(isTop ? Qt::blue : Qt::red, 2, style1);
            painter.setPen(pen);
            painter.drawLine(p1, normInterPoint);

            pen.setStyle(style2);
            painter.setPen(pen);
            painter.drawLine(normInterPoint, p2);

            interIdx++;
        } else if (i >= m) {
            // Secondary закончилась — просто дорисовываем primary как сплошную
            // линию
            QPen pen(isTop ? Qt::blue : Qt::red, 2, solid);
            painter.setPen(pen);
            painter.drawLine(p1, p2);
        } else {
            QPen pen(isTop ? Qt::blue : Qt::red, 2, solid);
            painter.setPen(pen);
            painter.drawLine(p1, p2);
        }
    }
}
CurveWidget::~CurveWidget() = default;
