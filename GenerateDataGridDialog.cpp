#include "GenerateDataGridDialog.h"
#include "ui_GenerateDataGridDialog.h"
#include <QDebug>

GenerateDataGridDialog::GenerateDataGridDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::GenerateDataGridDialog) {
  ui->setupUi(this);

  connect(this, &GenerateDataGridDialog::accepted,
          this, &GenerateDataGridDialog::apply);
}

GenerateDataGridDialog::~GenerateDataGridDialog() {
  delete ui;
}

void GenerateDataGridDialog::setMinX(double x) {
  ui->minBoundXSpinBox->setValue(-x);
}

void GenerateDataGridDialog::setMinY(double y) {
  ui->minBoundYSpinBox->setValue(-y);
}

void GenerateDataGridDialog::setMinZ(double z) {
  ui->minBoundZSpinBox->setValue(-z);
}

void GenerateDataGridDialog::apply() {

  float minX = ui->minBoundXSpinBox->value();
  float minY = ui->minBoundYSpinBox->value();
  float minZ = ui->minBoundZSpinBox->value();

  float maxX = ui->maxBoundXSpinBox->value();
  float maxY = ui->maxBoundYSpinBox->value();
  float maxZ = ui->maxBoundZSpinBox->value();

  float stepX = ui->stepXSpinBox->value();
  float stepY = ui->stepYSpinBox->value();
  float stepZ = ui->stepZSpinBox->value();

  emit dataParametersChanged(minX, minY, minZ,
                             maxX, maxY, maxZ,
                             stepX, stepY, stepZ);
}
