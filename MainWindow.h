#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QFileDialog>

#include "GenerateDataGridDialog.h"
#include "GenerateDataUniformDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  int m_step = 0;
  void updateUI();

  // Dialog boxes
  GenerateDataGridDialog* m_generateDataGridDialog;
  GenerateDataUniformDialog* m_generateDataUniformDialog;

  QFileDialog* m_fileDialog;

  QTimer* m_animationTimer = new QTimer(this);
  bool m_animating = false;
  int m_fps;

public slots:
  void stepForward();
  void stepBackward();

  void toggleAnimation();
  void toggleAnimation(bool animate);

  void updateMaxDisplayPerc();

  void reset();
  void setK(int k);

  void showGenerateDataGridDialog();
  void showGenerateDataUniformDialog();

  void openDialog();
  void loadData(const QString &file);

};
#endif // MAINWINDOW_H
