#include "ViewWidget.h"
#include "KMeans.h"

#include <QtMath>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QTimer>

ViewWidget::ViewWidget(QWidget *parent, Qt::WindowFlags f) :
  QOpenGLWidget(parent, f) {
  auto turntableTimer = new QTimer(this);
  turntableTimer->callOnTimeout(this, &ViewWidget::updateTurntable);
  turntableTimer->start(30);

  // Initializing model
  Point center = {0, 0, 0};

  m_k = 8;
  // m_data = KMeans::generateKNormalDistributions(m_k, 10000, center, .3);
  // m_data = KMeans::generate3DGrid(5.0f, 5.0f, 5.0f, .2f, .2f, .2f);
  m_data = KMeans::generateUniformDistribution(10000, {-2, -2, -2}, {2, 2, 2});
  model = KMeans(m_data, m_k);

  // Make a first step
  model.step();
  m_classes = model.getClasses();

  std::vector<float> centers = *model.getCenters()->data;
  m_history.push(centers);

}

QVector<GLfloat> getColors(int* classes, int size, int k) {

  double vmin = 0;
  double vmax = k - 1;
  QVector<GLfloat> colors(size * 3);

  for (int i = 0; i < size; i++) {
    int r = 3 * i + 0;
    int g = 3 * i + 1;
    int b = 3 * i + 2;

    colors[r] = 1;
    colors[g] = 1;
    colors[b] = 1;

    double v = classes[i];
    double dv;

    if (v < vmin)
      v = vmin;
    if (v > vmax)
      v = vmax;

    dv = vmax - vmin;

    if (v < (vmin + 0.25 * dv)) {
      colors[r] = 0;
      colors[g] = 4 * (v - vmin) / dv;
    } else if (v < (vmin + 0.5 * dv)) {
      colors[r] = 0;
      colors[b] = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
    } else if (v < (vmin + 0.75 * dv)) {
      colors[r] = 4 * (v - vmin - 0.5 * dv) / dv;
      colors[b] = 0;
    } else {
      colors[g] = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
      colors[b] = 0;
    }
  }

  return colors;
}

void ViewWidget::initializeGL() {
  initializeOpenGLFunctions();
  float r = m_backgroundColor.redF();
  float g = m_backgroundColor.greenF();
  float b = m_backgroundColor.blueF();
  float a = m_backgroundColor.alphaF();
  glClearColor(r, g, b, a);

  m_program.addShaderFromSourceCode(QOpenGLShader::Vertex,
    "attribute highp vec4 vertex;\n"
    "attribute mediump vec4 color;\n"
    "varying mediump vec4 vColor;\n"
    "uniform highp mat4 matrix;\n"
    "void main(void)\n"
    "{\n"
    "   gl_Position = matrix * vertex;\n"
    "   vColor = color;\n"
    "}");

  m_program.addShaderFromSourceCode(QOpenGLShader::Fragment,
    "varying mediump vec4 vColor;\n"
    "void main(void)\n"
    "{\n"
    "   gl_FragColor = vColor;\n"
    "}");

  m_program.link();


}

void ViewWidget::paintGL() {

  m_program.bind();
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_DEPTH_TEST);

  QMatrix4x4 pmvMatrix;
  pmvMatrix.perspective(40, (float) width() / height(), 1, 100);
  pmvMatrix.lookAt({0, 0, 15}, {0, 0, 0}, {0, 1, 0});
  pmvMatrix.rotate(m_turntableAngle, {0, 1, 0});


  m_program.enableAttributeArray("vertex");
  m_program.enableAttributeArray("color");

  m_program.setUniformValue("matrix", pmvMatrix);

  if (m_showPoints) {
    glPointSize(m_pointSize);

    QVector<GLfloat> colors = getColors(m_classes, m_data->size, m_k);
    m_program.setAttributeArray("vertex", m_data->data->data(), 3);
    m_program.setAttributeArray("color", colors.constData(), 3);
    glDrawArrays(GL_POINTS, 0, m_data->size * m_maxDisplayPerc);
  }

  if (m_showCentroids) {
    glPointSize(m_pointSize * 10);

    std::vector<int> uniqueClasses;
    uniqueClasses.reserve(model.getK());
    for (int i = 0; i < model.getK(); i++)
      uniqueClasses[i] = i;

    QVector<GLfloat> colors = getColors(uniqueClasses.data(), m_data->size, m_k);
    m_program.setAttributeArray("vertex", model.getCenters()->data->data(), 3);
    m_program.setAttributeArray("color", colors.constData(), 3);
    glDrawArrays(GL_POINTS, 0, model.getK());
  }

  m_program.disableAttributeArray("vertex");
  m_program.disableAttributeArray("colors");
}

void ViewWidget::updateTurntable() {
  m_turntableAngle += 1.0f;
  update();
}

void ViewWidget::stepForward() {

  // Push current centers to stack
  auto centers = model.getCenters()->data->data();
  std::vector<float> prevCenters(model.getK() * model.getDim());
  for (int i = 0, l = model.getK() * model.getDim(); i < l; i++) {
    prevCenters[i] = centers[i];
  }
  m_history.push(prevCenters);

  // Step K-Means forward
  model.step();
}

void ViewWidget::stepBackward() {
  if (m_history.size() > 1) {
    std::vector<float> centers = m_history.top();
    model.setCenters(centers);
    m_history.pop();
  }
}

void ViewWidget::reset() {
  model.initializeCenters();
  m_history.empty();
}

void ViewWidget::setPointSize(int s) {
  m_pointSize = s;
}

void ViewWidget::setMaxDisplayPerc(int s) {
  m_maxDisplayPerc = (float) s / 100.0f;
}

void ViewWidget::setK(int k) {
  model.setK(k);
}

void ViewWidget::setBackgroundColor(QColor c) {
  m_backgroundColor = c;
}

void ViewWidget::setShowPoints(bool show) {
  m_showPoints = show;
}

void ViewWidget::setShowCentroids(bool show) {
  m_showCentroids = show;
}
