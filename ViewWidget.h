#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <stack>
#include <QTimer>

#include "KMeans.h"

class ViewWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  ViewWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  KMeans* model;

protected:
  void initializeGL() override;
  void paintGL() override;
  QOpenGLShaderProgram m_program;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

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

  void setDataset3DGrid(float minX, float minY, float minZ,
                        float maxX, float maxY, float maxZ,
                        float stepX, float stepY, float stepZ);

  void setDataset3DUniform(float minX, float minY, float minZ,
                           float maxX, float maxY, float maxZ,
                           int numPoints);
  void setDataset(Dataset* dataset);

  void zoomIn();
  void zoomOut();

private:
  std::stack<std::vector<float> > m_history;
  int* m_classes;
  int m_k = 4;
  int m_pointSize;
  int m_step;

  QPoint m_lastPos;
  bool m_mousePressed = false;
  QPointF m_rotation;
  QPointF m_initRotation;
  QPointF m_position;
  QPointF m_initPosition;
  float m_zoom = 10;
  float m_zoomStep = 1.25;

  float m_maxDisplayPerc;
  QColor m_backgroundColor;

  bool m_showPoints = true;
  bool m_showCentroids = true;

  Dataset* m_data;

  float m_turntableAngle = 0.0f;

  bool m_loadingData = false;

};

#endif // VIEWWIDGET_H
