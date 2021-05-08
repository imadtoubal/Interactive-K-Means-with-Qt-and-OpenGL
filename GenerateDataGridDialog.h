#ifndef GENERATEDATAGRIDDIALOG_H
#define GENERATEDATAGRIDDIALOG_H

#include <QDialog>

namespace Ui {
class GenerateDataGridDialog;
}

class GenerateDataGridDialog : public QDialog
{
  Q_OBJECT

public:
  explicit GenerateDataGridDialog(QWidget *parent = nullptr);
  ~GenerateDataGridDialog();

signals:
  void dataParametersChanged(float, float, float,
                             float, float, float,
                             float, float, float);

public slots:
  void setMinX(double x);
  void setMinY(double y);
  void setMinZ(double z);

  void apply();

private:
  Ui::GenerateDataGridDialog *ui;
};

#endif // GENERATEDATAGRIDDIALOG_H
