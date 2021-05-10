#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GenerateDataGridDialog.h"
#include "ViewWidget.h"
#include "KMeans.h"


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_generateDataGridDialog(new GenerateDataGridDialog(this))
  , m_generateDataUniformDialog(new GenerateDataUniformDialog(this))
  , m_fileDialog(new QFileDialog(this)){
  ui->setupUi(this);

  // Connect generate 3D grid dialog box
  connect(m_generateDataGridDialog,
          &GenerateDataGridDialog::dataParametersChanged,
          ui->viewWidget, &ViewWidget::setDataset3DGrid);

  // Connect generate 3D uniform random dialog box
  connect(m_generateDataUniformDialog,
          &GenerateDataUniformDialog::dataParametersChanged,
          ui->viewWidget, &ViewWidget::setDataset3DUniform);

  // Point size
  ui->viewWidget->setPointSize(ui->pointSizeSpinBox->value());

  // Background color
  ui->viewWidget->setBackgroundColor(palette().background().color());

  // Set K
  int k = ui->kSpinBox->value();
  ui->viewWidget->setK(k);

  // K-Means animation
  m_fps = ui->fpsSpinBox->value();
  m_animationTimer->callOnTimeout(this, &MainWindow::stepForward);

  // Maximum number of points to be displayed
  updateMaxDisplayPerc();

  connect(m_fileDialog, &QFileDialog::fileSelected,
          this, &MainWindow::loadData);

}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::updateUI() {
  ui->stepLabel->setText( QString::number(m_step) );
  ui->energyLabel->setText(QString::number(ui->viewWidget->model->getEnergy()));
}

void MainWindow::stepForward() {

  // TODO: make it dynamic

  float deltaEnergy = ui->viewWidget->model->getDeltaEnergy();
  if (deltaEnergy > 0.0001f) {
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
  ui->viewWidget->model->setInitMethod(method);
  ui->viewWidget->reset();
  updateUI();
}

void MainWindow::setK(int k) {
  toggleAnimation(false);
  m_step = 0;
  int method = ui->initializationComboBox->currentIndex();

  ui->viewWidget->setK(k);
  ui->viewWidget->model->setInitMethod(method);
  ui->viewWidget->reset();
  updateUI();
}

void MainWindow::showGenerateDataGridDialog() {
  m_generateDataGridDialog->show();
}

void MainWindow::showGenerateDataUniformDialog() {
  m_generateDataUniformDialog->show();
}

void MainWindow::openDialog() {
  m_fileDialog->open();
}

void MainWindow::loadData(const QString &filePath) {
  // Read file dialog
  QFile file(filePath);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      return;

  QTextStream in(&file);

  int n, dim;
  // Read first two lines
  if (!in.atEnd())
    n = in.readLine().toInt();
  else
    return;

  if (!in.atEnd())
    dim = in.readLine().toInt();
  else
    return;

  Dataset* newDataset = new Dataset(n, dim);

  // Read data
  int i = 0;
  while (!in.atEnd()) {
    QString line = in.readLine();
    QStringList numList = line.split(' ');

    for (int j = 0; j < numList.size(); j++)
      newDataset->at(i)[j] = numList[j].toFloat();
    i++;
  }

  KMeans::standardizeDatasetInPlace(newDataset);
  ui->viewWidget->setDataset(newDataset);

}
