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
  KMeans model;

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

  void setPointSize(int s);
  void setMaxDisplayPerc(int s);

  void setK(int k);
  void setBackgroundColor(QColor c);
  void setShowPoints(bool show);
  void setShowCentroids(bool show);

private:
  std::stack<std::vector<float> > m_history;
  int* m_classes;
  int m_k;
  int m_pointSize;
  int m_step;
  float m_maxDisplayPerc;
  QColor m_backgroundColor;

  bool m_showPoints = true;
  bool m_showCentroids = true;

  Dataset* m_data;

  float m_turntableAngle = 0.0f;

};

#endif // VIEWWIDGET_H
