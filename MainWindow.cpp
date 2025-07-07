#include "MainWindow.h"

#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), curveWidget(new CurveWidget(this)) {
    auto* central = new QWidget(this);
    auto* layout  = new QVBoxLayout(central);

    auto* load1     = new QPushButton("Загрузить первую кривую");
    auto* load2     = new QPushButton("Загрузить вторую кривую");
    auto* switchBtn = new QPushButton("Переключить положение");

    layout->addWidget(curveWidget);
    layout->addWidget(load1);
    layout->addWidget(load2);
    layout->addWidget(switchBtn);

    setCentralWidget(central);
    resize(800, 600);
    setWindowTitle("Виджет кривых");

    connect(load1, &QPushButton::clicked, this, &MainWindow::loadFirstCurve);
    connect(load2, &QPushButton::clicked, this, &MainWindow::loadSecondCurve);
    connect(switchBtn, &QPushButton::clicked, this, &MainWindow::switchCurves);
}

void MainWindow::loadFirstCurve() {
    QString filename = QFileDialog::getOpenFileName(this, "Выберите первую кривую");
    if (!filename.isEmpty())
        curveWidget->loadCurve(filename, true);
}

void MainWindow::loadSecondCurve() {
    QString filename = QFileDialog::getOpenFileName(this, "Выберите вторую кривую");
    if (!filename.isEmpty())
        curveWidget->loadCurve(filename, false);
}

void MainWindow::switchCurves() { curveWidget->switchCurves(); }
