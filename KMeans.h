#ifndef KMEANS_H
#define KMEANS_H

#define INIT_RANDOM 0
#define INIT_RANDOM_REAL 2
#define INIT_RANDOM_D2 1



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
  int m_initMethod = INIT_RANDOM;
  float m_deltaEnergy;

  Dataset* m_centers;
  Dataset* m_nextCenters;

  void initializeCentersRandom();
  void initializeCentersRandomReal();
  void initializeCentersD2();
  void updateCenters();
  void assignClasses();

public:
  KMeans();
  KMeans(Dataset* dataset, int k);
  ~KMeans();
  void setDataset(Dataset* dataset);

  void initializeCenters();
  Dataset* getCenters();
  void setCenters(std::vector<float> centers);
  void setInitMethod(int method);
  void setK(int k);

  int* getClasses();
  int getInitMethod();
  int getK();
  int getDim();

  float getDeltaEnergy();
  float getEnergy();

  float step();
  void cluster(int maxIterations = 100, float stoppingThresh = 0.001);

  static Dataset* generate2DGrid(float w, float h, float colSize, float rowSize);
  static Dataset* generate3DGrid(float w, float h, float d,
                                 float colSize, float rowSize, float depSize);
  static Dataset* generateNormalDistribution(int n, Point center, float sigma);
  static Dataset* generateKNormalDistributions(int k, int n, Point center,
                                              float sigma);
  static Dataset* generateUniformDistribution(int n, Point minP, Point maxP);

  static std::vector<float> datasetMean(Dataset* dataset);
  static std::vector<float> datasetStd(Dataset* dataset);
  static Dataset* standardizeDataset(Dataset* dataset);
  static void standardizeDatasetInPlace(Dataset* dataset);
  static Dataset* reduceDimPCA(Dataset* dataset);

  static float l1Distance(float* point1, float* point2, int d);
  static float l2Distance(float* point1, float* point2, int d);
  static float l2DistanceSquared(float* point1, float* point2, int d);
};

#endif // KMEANS_H
