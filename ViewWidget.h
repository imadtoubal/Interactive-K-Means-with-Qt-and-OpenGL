#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include "KMeans.h"
#include <stack>
#include <QTimer>

class ViewWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  ViewWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

protected:
  void initializeGL() override;
  void paintGL() override;
  QOpenGLShaderProgram m_program;

private slots:
  void updateTurntable();

public slots:
  void stepForward();
  void stepBackward();
  void reset();

private:
  std::stack<std::vector<float> > m_history;
  int* m_classes;
  int m_k;
  Dataset* m_data;
  KMeans m_model;

  float m_turntableAngle = 0.0f;

};

#endif // VIEWWIDGET_H
