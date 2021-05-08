#include "GenerateDataUniformDialog.h"
#include "ui_GenerateDataUniformDialog.h"

GenerateDataUniformDialog::GenerateDataUniformDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::GenerateDataUniformDialog) {
  ui->setupUi(this);

  connect(this, &GenerateDataUniformDialog::accepted,
          this, &GenerateDataUniformDialog::apply);
}

GenerateDataUniformDialog::~GenerateDataUniformDialog() {
  delete ui;
}

void GenerateDataUniformDialog::setMinX(double x) {
  ui->minBoundXSpinBox->setValue(-x);
}

void GenerateDataUniformDialog::setMinY(double y) {
  ui->minBoundYSpinBox->setValue(-y);

}

void GenerateDataUniformDialog::setMinZ(double z) {
  ui->minBoundZSpinBox->setValue(-z);
}

void GenerateDataUniformDialog::apply() {

  float minX = ui->minBoundXSpinBox->value();
  float minY = ui->minBoundYSpinBox->value();
  float minZ = ui->minBoundZSpinBox->value();

  float maxX = ui->maxBoundXSpinBox->value();
  float maxY = ui->maxBoundYSpinBox->value();
  float maxZ = ui->maxBoundZSpinBox->value();

  int numPoints = ui->numPointsSpinBox->value();

  emit dataParametersChanged(minX, minY, minZ,
                             maxX, maxY, maxZ,
                             numPoints);
}
