#pragma once

#include "CurveWidget.h"

#include <QMainWindow>
#include "ui_MainWindow.h"

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void loadFirstCurve();
    void loadSecondCurve();
    void switchCurves() const;

private:
    CurveWidget *curveWidget;
    Ui::MainWindow *ui;
};
