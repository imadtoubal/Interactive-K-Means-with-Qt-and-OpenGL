#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

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
  int m_step;
  void updateUI();

  QTimer* m_animationTimer = new QTimer(this);
  bool m_animating = false;
  int m_fps;

public slots:
  void stepForward();
  void stepBackward();

  void startAnimation();
  void pauseAnimation();
  void toggleAnimation();

  void reset();

};
#endif // MAINWINDOW_H