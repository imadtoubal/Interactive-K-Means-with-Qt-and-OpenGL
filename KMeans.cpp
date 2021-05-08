#include <chrono>
#include <iostream>
#include <iterator>
#include <math.h>
#include <ostream>
#include <random>
#include <set>
#include <QDebug>
#include "KMeans.h"

void printDatasetDebug(Dataset *dataset) {
  std::cout << "[\n";
  for (int j = 0; j < dataset->size; j++) {
    std::cout << "[";
    for (int i = 0; i < dataset->dim; i++)
      std::cout << dataset->at(j)[i] << ", ";
    std::cout << "]," << std::endl;
  }
  std::cout << "]" << std::endl;
}

// Helper function to generate random int
int prng(int n) {
  auto t = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  srand(t);
  return rand() % n;
}

Point datasetMin(Dataset* dataset) {

  // Initialize minimum point
  Point minPoint(dataset->dim);
  float maxVal = std::numeric_limits<float>::max();
  for (int i = 0; i < dataset->dim; i++) {
    minPoint[i] = maxVal;
  }

  // Find minimum point
  for (int j = 0; j < dataset->size; j++) {
    for (int i = 0; i < dataset->dim; i++) {
      if (minPoint[i] > dataset->at(j)[i]) {
        minPoint[i] = dataset->at(j)[i];
      }
    }
  }

  // Return minimum point
  return minPoint;
}

Point datasetMax(Dataset* dataset) {

  // Initialize minimum point
  Point maxPoint(dataset->dim);
  float maxVal = std::numeric_limits<float>::min();
  for (int i = 0; i < dataset->dim; i++) {
    maxPoint[i] = maxVal;
  }

  // Find maximum point
  for (int j = 0; j < dataset->size; j++) {
    for (int i = 0; i < dataset->dim; i++) {
      if (maxPoint[i] < dataset->at(j)[i]) {
        maxPoint[i] = dataset->at(j)[i];
      }
    }
  }

  // Return maximum point
  return maxPoint;
}

KMeans::KMeans(Dataset* dataset, int k) {
  m_dataset = dataset;
  m_size = dataset->size;
  m_dim = dataset->dim;
  m_k = k;

  m_centers = new Dataset(k, m_dim);
  m_nextCenters = new Dataset(k, m_dim);
  m_classes = new int[m_size];

  // Initialize the cluster centers
  initializeCenters();
}

KMeans::~KMeans() {
  delete m_centers;
  delete m_classes;
  delete m_nextCenters;
}

void KMeans::setDataset(Dataset *dataset) {

  m_dataset = dataset;
  m_size = dataset->size;
  m_dim = dataset->dim;

  // Initialize the cluster centers
  initializeCenters();
}

void KMeans::initializeCenters() {
  // Reset energy
  m_deltaEnergy = std::numeric_limits<float>::max();

  // Initialize the cluster centers
  if (m_initMethod == INIT_RANDOM)
    initializeCentersRandom();
  else if (m_initMethod == INIT_RANDOM_D2)
    initializeCentersD2();
  else
    initializeCentersRandomReal();

  // Assign classes
  assignClasses();
}

void KMeans::initializeCentersRandom() {

  // Initialize visited hashset
  std::set<int> visited;

  while ( (int) visited.size() < m_k) {
    // Get random point
    int r = prng(m_size);

    // If point is not already added
    if (visited.find(r) == visited.end()) {

      // Copy point to center
      for (int j = 0; j < m_dim; j++)
        m_centers->at(visited.size())[j] = m_dataset->at(r)[j];

      // Mark pooint as visited
      visited.insert(r);
    }
  }
}

void KMeans::initializeCentersRandomReal() {

  // Get dataset bounds
  Point minPoint = datasetMin(m_dataset);
  Point maxPoint = datasetMax(m_dataset);

  // Initialize a random engine
  auto t = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::default_random_engine generator(t);

  // Get uniformly distibuted cluster centers
  for (int j = 0; j < m_k; j++) {
    for (int i = 0; i < m_dim; i++) {
      // Generate random number within range
      std::uniform_real_distribution<float>
      distribution(minPoint[i], maxPoint[i]);

      // Append number
      m_centers->at(j)[i] = distribution(generator);
    }
  }
}

void KMeans::initializeCentersD2() {
  // Get first point
  int r = prng(m_size);
  for (int j = 0; j < m_dim; j++)
    m_centers->at(0)[j] = m_dataset->at(r)[j];

  float probs[m_dataset->size];
  for (int k = 1; k < m_k; k++) {
    // float probSum = 0;
    float maxProb = std::numeric_limits<float>::min();
    int maxIdx;

    for (int i = 0, l = m_dataset->size; i < l; i++) {
      probs[i] = std::numeric_limits<float>::max();
      for (int j = 0; j < k; j++) {
        float dist = l2DistanceSquared(m_centers->at(j), m_dataset->at(i), m_dim);
        probs[i] = std::min(dist, probs[i]);
      }

      // probSum += probs[i];

      if (probs[i] > maxProb) {
        maxProb = probs[i];
        maxIdx = i;
      }
    }

    for (int j = 0; j < m_dim; j++)
      m_centers->at(k)[j] = m_dataset->at(maxIdx)[j];

  }

}


void KMeans::updateCenters() {

  // Keep track of cluster counts
  std::vector<int> newCentersCount(m_k);

  for (int i = 0; i < m_k; i++) {
    newCentersCount[i] = 0;
    for (int j = 0; j < m_dim; j++)
      m_nextCenters->at(i)[j] = 0.0f;
  }

  // Calculate centers
  for (int i = 0; i < m_size; i++) {
    newCentersCount[m_classes[i]]++;
    for (int j = 0; j < m_dim; j++)
      m_nextCenters->at(m_classes[i])[j] += m_dataset->at(i)[j];
  }
  for (int i = 0; i < m_k; i++)
    for (int j = 0; j < m_dim; j++) {
      // Avoid nan (0/0)
      if (m_nextCenters->at(i)[j] != 0 && newCentersCount[i] != 0) {
        m_nextCenters->at(i)[j] /= newCentersCount[i];
      }
    }
}

void KMeans::assignClasses()
{
  for (int i = 0; i < m_size; i++) {
    float minDistance = std::numeric_limits<float>::max();
    int assignedClass = 0;
    for (int j = 0; j < m_k; j++) {
      float d = l2DistanceSquared(m_centers->at(j), m_dataset->at(i), m_dim);
      if (d < minDistance) {
        minDistance = d;
        assignedClass = j;
      }
    }
    m_classes[i] = assignedClass;
  }
}

KMeans::KMeans() {

}

Dataset* KMeans::getCenters() {
  return m_centers;
}

void KMeans::setCenters(std::vector<float> centers) {
  for (int i = 0; i < m_k; i++) {
    for (int j = 0; j < m_dim; j++) {
      m_centers->at(i)[j] = centers[i * m_dim + j];
    }
  }
  assignClasses();
}

void KMeans::setInitMethod(int method) {
  m_initMethod = method;
}

void KMeans::setK(int k) {
  m_k = k;
  delete m_centers;
  delete m_nextCenters;
  m_centers = new Dataset(k, m_dim);
  m_nextCenters = new Dataset(k, m_dim);
  initializeCenters();
}

int KMeans::getInitMethod() {
  return m_initMethod;
}

int* KMeans::getClasses() {
  return m_classes;
}

int KMeans::getK() {
  return m_k;
}

int KMeans::getDim() {
  return m_dim;
}

float KMeans::getDeltaEnergy() {
  // Calculate the movement
  return m_deltaEnergy;
}

float KMeans::getEnergy() {
  float energy = 0.0f;
  for (int i = 0; i < m_dataset->size; i++) {
    energy += l2Distance(m_dataset->at(i), m_centers->at(m_classes[i]), m_dim);
  }
  return energy / m_dataset->size;
}

float KMeans::l1Distance(float* point1, float* point2, int d) {
  float total = 0;
  for (int i = 0; i < d; i++) {
    float diff = point1[i] - point2[i];
    total += abs(diff);
  }
  return total;
}

float KMeans::l2Distance(float* point1, float* point2, int d) {
  double total = 0;
  for (int i = 0; i < d; i++) {
    double diff = point1[i] - point2[i];
    total += diff * diff;
  }
  return sqrt(total);
}

float KMeans::l2DistanceSquared(float* point1, float* point2, int d) {
  double total = 0;
  for (int i = 0; i < d; i++) {
    double diff = point1[i] - point2[i];
    total += diff * diff;
  }
  return total;
}


Dataset* KMeans::generate2DGrid(float w, float h, float colSize, float rowSize) {
  float xGridSize = (float) w / colSize, yGridSize = (float) h / rowSize;
  int m_size = xGridSize * yGridSize;

  Dataset* dataset = new Dataset(m_size, 2);
  for (float i = 0; i < w; i += colSize) {
    for (float j = 0; j < h; j += rowSize) {
      dataset->data->push_back((float) i);
      dataset->data->push_back((float) j);
    }
  }

  return dataset;
}

Dataset* KMeans::generate3DGrid(float w, float h, float d,
                                float colSize, float rowSize, float depSize) {
  float xGridSize = w / colSize;
  float yGridSize = h / rowSize;
  float zGridSize = d / depSize;

  int m_size = std::ceil(xGridSize)
             * std::ceil(yGridSize)
             * std::ceil(zGridSize);

  Dataset* dataset = new Dataset(m_size, 3);

  int idx = 0;

  for (int i = 0; i < xGridSize; i++) {
    for (int j = 0; j < yGridSize; j++) {
      for (int k = 0; k < zGridSize; k++) {
        dataset->at(idx)[0] = -w / 2 + i * colSize;
        dataset->at(idx)[1] = -h / 2 + j * colSize;
        dataset->at(idx)[2] = -d / 2 + k * colSize;

        idx++;
      }
    }
  }

  return dataset;
}


Dataset* KMeans::generateNormalDistribution(int n, Point center, float sigma) {
  int d = center.size();
  std::default_random_engine generator;
  std::vector<std::normal_distribution<float> > distributions;
  for (int j = 0; j < d; j++) {
    std::normal_distribution<float> distribution(center[j], sigma);
    distributions.push_back(distribution);
  }

  Dataset* dataset = new Dataset(n, d);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < d; j++) {
      float x = distributions[j](generator);
      dataset->at(i)[j] = x;
    }
  }

  return dataset;
}

Dataset*
KMeans::generateKNormalDistributions(int k, int n, Point center, float sigma) {
  int d = center.size();

  auto t = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::default_random_engine generator(t);

  Dataset* dataset = new Dataset(k * n, d);
  Dataset* centers = new Dataset(k, d);

  std::uniform_real_distribution<float> distribution(-2, 2);
  for (int idx = 0; idx < k; idx++)
    for (int j = 0; j < d; j++)
      centers->at(idx)[j] = center[j] + distribution(generator);


  for (int idx = 0; idx < k; idx++) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < d; j++) {
        std::normal_distribution<float> distribution(centers->at(idx)[j], sigma);
        dataset->at(i * k + idx)[j] = distribution(generator);
      }
    }
  }

  return dataset;
}

Dataset *KMeans::generateUniformDistribution(int n, Point minP, Point maxP) {
  auto t = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::default_random_engine generator(t);

  int d = minP.size();
  Dataset* dataset = new Dataset(n, d);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < d; j++) {
      std::uniform_real_distribution<float> distribution(minP[j], maxP[j]);
      dataset->at(i)[j] = distribution(generator);
    }
  }

  return dataset;
}

std::vector<float> KMeans::datasetMean(Dataset *dataset) {
  std::vector<float> meanVec;
  meanVec.reserve(dataset->dim);

  for (int j = 0; j < dataset->dim; j++)
    meanVec[j] = 0.0f;

  for (int i = 0; i < dataset->size; i++) {
    for (int j = 0; j < dataset->dim; j++) {
      meanVec[j] += dataset->at(i)[j] / dataset->size;
    }
  }
  return meanVec;
}

std::vector<float> KMeans::datasetStd(Dataset *dataset) {
  std::vector<float> meanVec = datasetMean(dataset);

  std::vector<float> stdevVec(dataset->dim);
  for (int j = 0; j < dataset->dim; j++)
    stdevVec[j] = 0.0f;

  for (int i = 0; i < dataset->size; i++) {
    for (int j = 0; j < dataset->dim; j++) {
      stdevVec[j] += std::powf(dataset->at(i)[j] - meanVec[j], 2)
                   / dataset->size;
    }
  }

  for (int j = 0; j < dataset->dim; j++)
    stdevVec[j] = std::sqrt(stdevVec[j]);

  return stdevVec;
}

Dataset *KMeans::standardizeDataset(Dataset *dataset) {
  Dataset* newDataset = new Dataset(dataset->size, dataset->dim);

  // Calculate the mean
  std::vector<float> meanVec = datasetMean(dataset);

  // Calculate the standard deviation
  std::vector<float> stdevVec = datasetStd(dataset);

  // z-Normalization
  for (int i = 0; i < dataset->size; i++) {
    for (int j = 0; j < dataset->dim; j++) {
      newDataset->at(i)[j] = (dataset->at(i)[j] - meanVec[j]) / stdevVec[j];
    }
  }
  return newDataset;
}

void KMeans::standardizeDatasetInPlace(Dataset *dataset) {

  // Calculate the mean
  std::vector<float> meanVec = datasetMean(dataset);

  // Calculate the standard deviation
  std::vector<float> stdevVec = datasetStd(dataset);

  // z-Normalization
  for (int i = 0; i < dataset->size; i++) {
    for (int j = 0; j < dataset->dim; j++) {
      dataset->at(i)[j] = (dataset->at(i)[j] - meanVec[j]) / stdevVec[j];
    }
  }
}

Dataset *KMeans::reduceDimPCA(Dataset *dataset) {
  Dataset* normDataset = new Dataset(dataset->size, dataset->dim);

  // 1. Standardization
  // Calculate the mean
  std::vector<float> meanVec = datasetMean(dataset);

  // Calculate the standard deviation
  std::vector<float> stdevVec = datasetStd(dataset);

  // z-Normalization
  for (int i = 0; i < dataset->size; i++) {
    for (int j = 0; j < dataset->dim; j++) {
      normDataset->at(i)[j] = (dataset->at(i)[j] - meanVec[j]) / stdevVec[j];
    }
  }

  // 2. Covariance matrix
  Dataset* covarianceMatrix = new Dataset(dataset->dim, dataset->dim);
  for (int i = 0; i < normDataset->size; i++) {
    for (int j = 0; j < normDataset->dim; j++) {
      for (int k = 0; k < normDataset->dim; k++) {
        covarianceMatrix->at(j)[k] += (normDataset->at(i)[j] - meanVec[j])
                                    * (normDataset->at(i)[k] - meanVec[k])
                                    / (normDataset->size - 1);
      }
    }
  }

 delete normDataset;
 delete covarianceMatrix;

}

float KMeans::step() {

  // Update centers
  updateCenters();

  // Calculate the movement
  float deltaDistance = 0;
  for (int i = 0; i < m_k; i++)
    deltaDistance += l2Distance(m_centers->at(i), m_nextCenters->at(i), m_dim) / m_k;

  // Copy centers to next centers
  for (int i = 0; i < m_k; i++) {
    for (int j = 0; j < m_dim; j++) {
      m_centers->at(i)[j] = m_nextCenters->at(i)[j];
    }
  }

  // Assign classes
  assignClasses();

  m_deltaEnergy = deltaDistance;
  return deltaDistance;
}

void KMeans::cluster(int maxIterations, float stoppingThresh) {

  // Associate points to cluster centers
  float deltaDistance = std::numeric_limits<float>::max();
  int iter = 0;
  while (deltaDistance > stoppingThresh) {
    deltaDistance = step();
    if (++iter > maxIterations) break;
  }
}
