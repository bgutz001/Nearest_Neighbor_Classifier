#include <iostream>
#include <vector>
#include <assert.h>
#include <math.h>

struct Object {
  int oClass;
  std::vector<double> features;
  
  Object() {
    oClass = 0;
  }
  
};

// Returns the distance between two objects.
// featureIndicies is the idindices of the features to use in the object's feature list
// obj1 and obj2 are the objects you are calculating the distance between
double distance
(const std::vector<int> &featureIndicies, const Object &obj1, const Object &obj2);

// Returns the class of the nearest neighbor.
// featureIndicies is the idindices of the features to use in the distance formula.
// objects should be a vector of all objects.
// index should be the index of the object in objects that you want to find the nearest neighbor for.
int nearestNeighbor
(const std::vector<int> &featureIndicies, const std::vector<Object> &objects, const int index);

// Greedy forward selection algorithm
// Add one feature at a time picking the best one to expand upon
void forwardSelection(std::vector<int> &featureList);

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Incorrect usage. \nCorrect usage: " << argv[0] <<
      " <filename> \nwhere <filename> is the name of the file for your dataset" << std::endl;
  }

  
  
  
  return 0;
}

double distance
(const std::vector<int> &featureIndicies, const Object &obj1, const Object &obj2) {
  double distance = 0;
  for(auto it = featureIndicies.begin(); it != featureIndicies.end(); ++it) {
    distance += pow(obj1.features.at(*it) - obj2.features.at(*it), 2);
  }
  distance = pow(distance, 0.5);
  return distance;
}

int nearestNeighbor
(const std::vector<int> &featureIndicies, const std::vector<Object> &objects, const int index) {
  assert(index > objects.size());

  double minDistance = 1.0/0.0;  // Set min distance to INF
  double curDistance = 0;
  int minIndex = 0;              // The index of the nearest neighbor
  for (int i = 0; i < index; ++i) {
    curDistance = distance(featureIndicies, objects.at(i), objects.at(index));
    if (curDistance < minDistance) {
      minDistance = curDistance;
      minIndex = i;
    }
  }
  for (int i = index + 1; i < objects.size(); ++i) {
    curDistance = distance(featureIndicies, objects.at(i), objects.at(index));
    if (curDistance < minDistance) {
      minDistance = curDistance;
      minIndex = i;
    }
  }
}

std::vector<int>
forwardSelection(std::vector<Object> &trainingSet, int cnt) {
  static std::vector<int> featureList;
  if (trainingSet.size() == 0) {
    return featureList;
  }
  
  static int numFeatures = trainingSet.at(0).features.size();

  // TODO: fix condition
  for (int i = 0 ; i < numFeatures - cnt; ++i) {
    // Add feature
    featureList.push_back(i);
    // Check its accuracy

    // If it has the best accuracy so far keep it

    // If not then remove it
    //    featureList.pop();
  }

  //forwardSelection(std::vector<Object> trainingSet, cnt + 1);
  return featureList;
}
