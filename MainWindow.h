#pragma once

#include "CurveWidget.h"

#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT

    class Inner {
    public:
        void doSomething();
    };

public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void loadFirstCurve();
    void loadSecondCurve();
    void switchCurves();

private:
    CurveWidget* curveWidget;
};
