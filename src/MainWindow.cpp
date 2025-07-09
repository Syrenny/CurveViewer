#include "MainWindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    curveWidget(new CurveWidget(this)),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->curveWidget->layout()->addWidget(curveWidget);

    ui->label1->setText(QString(""));
    ui->label2->setText(QString(""));
    ui->errorLabel->setText(QString(""));
    connect(ui->load1,
            &QPushButton::clicked,
            this,
            &MainWindow::loadFirstCurve);
    connect(ui->load2,
            &QPushButton::clicked,
            this,
            &MainWindow::loadSecondCurve);
    connect(ui->switchBtn,
            &QPushButton::clicked,
            this,
            &MainWindow::switchCurves);
}

void MainWindow::loadFirstCurve() {
    const QString filepath =
            QFileDialog::getOpenFileName(this, "Выберите первую кривую");
    if (!filepath.isEmpty()) {
        const QString filename = QFileInfo(filepath).fileName();
        ui->label1->setText(filename);
        curveWidget->loadCurve(filepath, true);
    } else
        qDebug() << "File is empty";
}

void MainWindow::loadSecondCurve() {
    const QString filepath =
            QFileDialog::getOpenFileName(this, "Выберите вторую кривую");
    if (!filepath.isEmpty()) {
        const QString filename = QFileInfo(filepath).fileName();
        ui->label2->setText(filename);
        curveWidget->loadCurve(filepath, false);
    } else
        qDebug() << "File is empty";
}

void MainWindow::switchCurves() const { curveWidget->switchCurves(); }

MainWindow::~MainWindow() { delete ui; };
