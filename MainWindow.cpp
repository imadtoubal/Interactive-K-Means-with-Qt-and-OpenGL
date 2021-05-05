#include "MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // Point size
  ui->viewWidget->setPointSize(ui->pointSizeSpinBox->value());

  // Background color
  ui->viewWidget->setBackgroundColor(palette().background().color());

  // K-Means animation
  m_fps = ui->fpsSpinBox->value();
  m_animationTimer->callOnTimeout(this, &MainWindow::stepForward);

  // Maximum number of points to be displayed
  updateMaxDisplayPerc();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::updateUI() {
  ui->stepLabel->setText( QString::number(m_step) );
}

void MainWindow::stepForward() {
  // TODO: make it dynamic
  // TODO: address the issue with swapping. When you swap after iteration,
  // the distance remains the same after each step when vonverged.

  if (ui->viewWidget->model.getEnergy() > 0.01f) {
    ui->viewWidget->stepForward();
    m_step += 1;
    updateUI();
  } else {
    toggleAnimation(false);
  }
}

void MainWindow::stepBackward() {
  ui->viewWidget->stepBackward();
  m_step = qMax(m_step - 1, 0);
  updateUI();
}

void MainWindow::toggleAnimation() {
  m_animating = !m_animating;
  toggleAnimation(m_animating);
}

void MainWindow::toggleAnimation(bool animate)
{
  m_animating = animate;
  if (animate) {
    m_fps = ui->fpsSpinBox->value();
    m_animationTimer->start(1000.0f / m_fps);
    ui->animateButton->setText(QString("Pause"));
  } else {
    m_animationTimer->stop();
    ui->animateButton->setText(QString("Play"));
  }
  ui->actionAnimate->setChecked(animate);
  ui->fpsSpinBox->setDisabled(animate);
}

void MainWindow::updateMaxDisplayPerc() {
  int perc = ui->displayPointsSlider->value();
  ui->viewWidget->setMaxDisplayPerc(perc);
}

void MainWindow::reset() {
  toggleAnimation(false);
  m_step = 0;
  int method = ui->initializationComboBox->currentIndex();
  ui->viewWidget->model.setInitMethod(method);
  ui->viewWidget->reset();
  updateUI();
}
