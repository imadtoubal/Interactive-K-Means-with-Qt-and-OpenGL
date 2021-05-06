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

public slots:
  void setMinX(double x);
  void setMinY(double y);
  void setMinZ(double z);

private:
  Ui::GenerateDataGridDialog *ui;
};

#endif // GENERATEDATAGRIDDIALOG_H
