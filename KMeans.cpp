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


// KMeans::KMeans(int n, int d, int k) {
//   m_size = n;
//   m_dim = d;
//   m_k = k;

//   m_classes = new int[m_size];
// }

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

void KMeans::initializeCenters() {
  // Initialize the cluster centers
  initializeCentersRandomReal();

  // Assign classes
  assignClasses();
}

void KMeans::initializeCentersRandom() {

  std::set<int> visited;
  while (visited.size() < m_k) {
    int r = prng(m_size);
    if (visited.find(r) == visited.end()) {
      // Copy point to center
      for (int i = 0; i < m_dim; i++)
        m_centers->at(visited.size())[i] = m_dataset->at(r)[i];

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

int* KMeans::getClasses() {
  return m_classes;
}

int KMeans::getK() {
  return m_k;
}

int KMeans::getDim() {
  return m_dim;
}

float KMeans::l1Distance(float* point1, float* point2, int d) {
  float total = 0;
  for (unsigned i = 0; i < d; i++) {
    float diff = point1[i] - point2[i];
    total += abs(diff);
  }
  return total;
}

float KMeans::l2Distance(float* point1, float* point2, int d) {
  double total = 0;
  for (unsigned i = 0; i < d; i++) {
    double diff = point1[i] - point2[i];
    total += diff * diff;
  }
  return sqrt(total);
}

float KMeans::l2DistanceSquared(float* point1, float* point2, int d) {
  double total = 0;
  for (unsigned i = 0; i < d; i++) {
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

  for (int idx = 0; idx < k; idx++) {
    Point tempCenter(d);

    std::uniform_real_distribution<float> distribution(-2, 2);
    for (int j = 0; j < d; j++)
      tempCenter[j] = center[j] + distribution(generator);

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < d; j++) {
        std::normal_distribution<float> distribution(tempCenter[j], sigma);
        dataset->at(idx * n + i)[j] = distribution(generator);
      }
    }
  }
  return dataset;
}

float KMeans::step() {

  // Update centers
  updateCenters();

  // Calculate the movement
  float deltaDistance = 0;
  for (int i = 0; i < m_k; i++)
    deltaDistance += l2Distance(m_centers->at(i), m_nextCenters->at(i), m_dim) / m_k;

  if (deltaDistance < 0.01f)
    return deltaDistance;

  // Swap new centers and old centers
  std::swap(m_centers, m_nextCenters);

  // Assign classes
  assignClasses();

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
