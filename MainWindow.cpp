#include "MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // K-Means animation
  m_fps = ui->fpsSpinBox->value();
  m_animationTimer->callOnTimeout(this, &MainWindow::stepForward);
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::updateUI() {
  ui->stepLabel->setText( QString::number(m_step) );
}

void MainWindow::stepForward() {
  ui->viewWidget->stepForward();
  m_step += 1;
  updateUI();
}

void MainWindow::stepBackward() {
  ui->viewWidget->stepBackward();
  m_step = qMax(m_step - 1, 0);
  updateUI();
}

void MainWindow::startAnimation() {
  m_fps = ui->fpsSpinBox->value();
  m_animationTimer->start(1000.0f / m_fps);
  ui->animateButton->setText(QString("Pause"));
  ui->fpsSpinBox->setDisabled(true);
}

void MainWindow::pauseAnimation() {
  m_animationTimer->stop();
  ui->animateButton->setText(QString("Play"));
  ui->fpsSpinBox->setDisabled(false);
}

void MainWindow::toggleAnimation() {
  m_animating = !m_animating;
  if (m_animating) {
    startAnimation();
  } else {
    pauseAnimation();
  }
}

void MainWindow::reset() {
  pauseAnimation();
  m_step = 0;
  ui->viewWidget->reset();
  updateUI();
}
