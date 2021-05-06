#include "GenerateDataGridDialog.h"
#include "ui_GenerateDataGridDialog.h"

GenerateDataGridDialog::GenerateDataGridDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::GenerateDataGridDialog)
{
  ui->setupUi(this);
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
