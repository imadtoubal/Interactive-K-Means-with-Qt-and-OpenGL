#ifndef GENERATEDATAUNIFORMDIALOG_H
#define GENERATEDATAUNIFORMDIALOG_H

#include <QDialog>

namespace Ui {
class GenerateDataUniformDialog;
}

class GenerateDataUniformDialog : public QDialog
{
  Q_OBJECT

public:
  explicit GenerateDataUniformDialog(QWidget *parent = nullptr);
  ~GenerateDataUniformDialog();

signals:
  void dataParametersChanged(float, float, float,
                             float, float, float,
                             int);

public slots:
  void setMinX(double x);
  void setMinY(double y);
  void setMinZ(double z);

  void apply();

private:
  Ui::GenerateDataUniformDialog *ui;
};

#endif // GENERATEDATAUNIFORMDIALOG_H
