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

  m_k = 5;
  m_data = KMeans::generateKNormalDistributions(m_k, 1000, center, .5);

  m_model = KMeans(m_data, m_k);

  // Make a first step
  m_model.step();
  m_classes = m_model.getClasses();

  std::vector<float> centers = *m_model.getCenters()->data;
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

  glClearColor(0.2, 0.2, 0.2, 1.0);

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

  QVector<GLfloat> colors = getColors(m_classes, m_data->size, m_k);


  QMatrix4x4 pmvMatrix;
  pmvMatrix.perspective(40, (float) width() / height(), 1, 100);
  pmvMatrix.lookAt({0, 0, 10}, {0, 0, 0}, {0, 1, 0});
  pmvMatrix.rotate(m_turntableAngle, {0, 1, 0});
  glPointSize(8);

  m_program.enableAttributeArray("vertex");
  m_program.enableAttributeArray("color");

  m_program.setUniformValue("matrix", pmvMatrix);

  m_program.setAttributeArray("vertex", m_data->data->data(), 3);
  m_program.setAttributeArray("color", colors.constData(), 3);

  glDrawArrays(GL_POINTS, 0, m_data->size);

  m_program.disableAttributeArray("vertex");
  m_program.disableAttributeArray("colors");
}

void ViewWidget::updateTurntable() {
  m_turntableAngle += 1.0f;
  update();
}

void ViewWidget::stepForward() {
  // Push current centers to stack
  auto centers = m_model.getCenters()->data->data();
  std::vector<float> prevCenters(m_model.getK() * m_model.getDim());
  for (int i = 0, l = m_model.getK() * m_model.getDim(); i < l; i++) {
    prevCenters[i] = centers[i];
  }
  m_history.push(prevCenters);

  // Step K-Means forward
  m_model.step();
}

void ViewWidget::stepBackward() {
  if (m_history.size() > 1) {
    std::vector<float> centers = m_history.top();
    m_model.setCenters(centers);
    m_history.pop();
  }
}

void ViewWidget::reset() {
  m_model.initializeCenters();
  m_history.empty();
}
