#ifndef KMEANS_H
#define KMEANS_H

#include <vector>

typedef std::vector<float> Point;

class Dataset {
public:
  std::vector<float>* data = new std::vector<float>;
  int size;
  int dim;

  Dataset(int size, int dim) : size(size), dim(dim){
    data->reserve(size * dim);
  }

  Dataset(std::vector<std::vector<float> > dataset) : size(dataset.size()),
    dim(dataset[0].size()) {

    for (auto row = dataset.begin(); row != dataset.end(); row++) {
      for (auto val = row->begin(); val != row->end(); val++)
        data->push_back(*val);
    }
  }

  float* at(int index) {
    return data->data() + (index * dim);
  }

  ~Dataset() {
    delete data;
  }

};

Point datasetMin(Dataset* dataset);
Point datasetMax(Dataset* dataset);


class KMeans {
private:
  Dataset* m_dataset;
  int* m_classes;
  int m_dim;
  int m_k;
  int m_size;

  Dataset* m_centers;
  Dataset* m_nextCenters;

  void initializeCentersRandom();
  void initializeCentersRandomReal();
  void updateCenters();
  void assignClasses();

public:
  KMeans();
  KMeans(Dataset* dataset, int k);


  void initializeCenters();
  Dataset* getCenters();
  void setCenters(std::vector<float> centers);

  int* getClasses();
  int getK();
  int getDim();

  float step();
  void cluster(int maxIterations = 100, float stoppingThresh = 0.001);

  static Dataset* generate2DGrid(float w, float h, float colSize, float rowSize);
  Dataset* generateGrid(float minimum, float maximum, float step,int dim);
  static Dataset* generateNormalDistribution(int n, Point center, float sigma);
  static Dataset* generateKNormalDistributions(int k, int n, Point center,
                                              float sigma);

  static float l1Distance(float* point1, float* point2, int d);
  static float l2Distance(float* point1, float* point2, int d);
  static float l2DistanceSquared(float* point1, float* point2, int d);
};

#endif // KMEANS_H
