#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <string>
#include <cstdio>
#include <cmath>
#include <set>
#include <chrono>

#define INF 1.0/0.0

struct Object {
  // First item is class
  // The rest are features
  std::vector<double> features;

  void print() {
    for (auto it = features.begin(); it != features.end(); ++it) {
      std::cout << *it << ' ';
    }
    std::cout << std::endl;
  }
};

// Returns the distance between two objects.
// featureIndicies is the indices of the features to use in the object's feature list
// obj1 and obj2 are the objects you are calculating the distance between
double distance
(const std::set<int> &featureIndicies, const Object &obj1, const Object &obj2);

// Returns the index of the nearest neighbor.
// featureIndicies is the idindices of the features to use in the distance formula.
// objects should be a vector of all objects.
// index should be the index of the object in objects that you want to find the nearest neighbor for.
int nearestNeighbor
(const std::set<int> &featureIndicies, const std::vector<Object> &objects, const int index);

// Normalize features of objetcs
// All object->features should be of the same length
// objects is the list of objects whose features you want to normalize
void normalize
(std::vector<Object> &objects);

// Leave one out validation
// Returns the accuracy of using certain features
// featureIndicies is a list of indicies that indecies in object->features
// objects is a list of Objects that we are finding the accuracy of
double validation
(const std::set<int> &featureIndicies, const std::vector<Object> &objects);

// Greedy forward selection algorithm
// Add one feature at a time picking the best one to expand upon
std::set<int> forwardSelection
(const std::vector<Object> &trainingSet);

// Greedy backward elimination algorithm
// Start with a classification using all the features
// Get rid of features one by one until they start to decrease accuracy
std::set<int> backwardElimination
(const std::vector<Object> &trainingSet);

// Prints the feature list wrapped with curly braces
// Ex. {1, 2, 3, 4}
void printFeatureList
(const std::set<int> &featureList);

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Incorrect usage. \nCorrect usage: " << argv[0] <<
      " <filename> \nwhere <filename> is the name of the file for your dataset" << std::endl;
    exit(-1);
  }

  std::vector<Object> instances;

  std::fstream file;
  file.open(argv[1]);
  if (file.fail()) {
    std::cout << "Failed to open file " << argv[1] << std::endl;
    exit(-2);
  }

  std::cout << "Reading data from input file ";
  std::string line;
  double temp;
  while (!getline(file, line).eof()) {
    instances.push_back(Object());
    // Read in features
    while (sscanf(line.c_str(), "%lf", &temp) != 0) {
      // Delete leading whitespace
      while (line.at(0) == ' ') {
	line.erase(0, 1);
      }
      // Add in feature
      instances.back().features.push_back(temp);
      std::size_t pos = line.find(' ', 0);
      // No more features left
      if (pos == std::string::npos) {
	break;
      }
      // Remove added feature from input data
      line = line.substr(pos);
    }
  }
  file.close();
  std::cout << "(done)" << std::endl;

  std::cout << "Normalizing data ";
  normalize(instances);
  std::cout << "(done)" << std::endl;

  // INPUT
  std::cout << "Type the number of the algorithm you want to run." << std::endl
	    << "\t1) Forward Selection" << std::endl
	    << "\t2) Backward Elimination" << std::endl;
  int input;
  std::cin >> input;
  std::set<int> (*search)(const std::vector<Object>&) = 0;
  while (search == 0) {
    switch(input) {
    case 1:
      search = &forwardSelection;
      break;
    case 2:
      search = &backwardElimination;
      break;
    default:
      std::cout << "Please enter a valid selection." << std::endl;
      std::cin >> input;
      break;
    }
  }

  // Format output
  std::cout.setf(std::ios::fixed, std::ios::floatfield);
  std::cout.precision(3);

  /*
  // DEBUG
  std::cout << "num instances: " << instances.size() << std::endl;
  for (auto it = instances.begin(); it != instances.end(); ++it) 
    it->print();
  // * DEBUG
  */

  auto start = std::chrono::high_resolution_clock::now();
  std::set<int> featureList = search(instances);

  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "The search took "
	    << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()
	    << " milliseconds." << std::endl;

  // OUTPUT
  std::cout << "Feature list ";
  printFeatureList(featureList);
  std::cout << " is the best feature subset, with an accuracy of "
	    << validation(featureList, instances) << std::endl;
  
  return 0;
}

double distance
(const std::set<int> &featureIndicies, const Object &obj1, const Object &obj2) {
  double distance = 0;
  for(auto it = featureIndicies.begin(); it != featureIndicies.end(); ++it) {
    distance += pow(obj1.features.at(*it) - obj2.features.at(*it), 2);
  }
  /*
    We can exclude the square root in the euclidean distance function
    This is because for all non-negative numbers x, y
    sqrt(x) < sqrt(y) <=> x < y
  */
  //distance = pow(distance, 0.5);
  return distance;
}

int nearestNeighbor
(const std::set<int> &featureIndicies, const std::vector<Object> &objects, const int index) {
  assert(index < objects.size());

  double minDistance = INF;
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

  return minIndex;
}

// Formula: (X - min)/(max - min)
void normalize
(std::vector<Object> &objects) {
  // Check if there are features to normalize
  if (objects.at(0).features.size() <= 1)
    return;

  // Go through features but skip index 0 (class)
  for (int i = 1; i < objects.at(0).features.size(); ++i) {
    double max = -INF;
    double min = INF;
    // Go through objects and find max and min value of each feature
    for (int j = 0; j < objects.size(); ++j) {
      if (objects.at(j).features.at(i) > max)
	max = objects.at(j).features.at(i);
      if (objects.at(j).features.at(i) < min)
	min = objects.at(j).features.at(i);
    }
    for (int j = 0; j < objects.size(); ++j)
      objects.at(j).features.at(i) = (objects.at(j).features.at(i) - min) / (max - min);
  }
}

double validation
(const std::set<int> &featureIndicies, const std::vector<Object> &objects) {
  double correct = 0; // Number of correct classifications
  // Go through objects leaving one out each time
  for (int i = 0; i < objects.size(); ++i) {
    if (objects.at(nearestNeighbor(featureIndicies, objects, i)).features.at(0)
	== objects.at(i).features.at(0)) {
      ++correct;
    }
  }
  return correct / objects.size();
}


std::set<int> forwardSelection
(const std::vector<Object> &trainingSet) {
  if (trainingSet.size() == 0) {
    std::cout << "Error: Training set has no objects" << std::endl;
    exit(0);
  }

  std::set<int> remainingFeatures;
  for (int i = 1; i < trainingSet.at(0).features.size(); ++i) {
    remainingFeatures.insert(i);
  }
  std::set<int> featureList;
  std::set<int> localFeatureList;
  double accuracy = 0;
  int feature = -1;
  bool gain = true;

  while (!remainingFeatures.empty()) {
    double maxAccuracy = 0;
    for (auto it = remainingFeatures.begin(); it != remainingFeatures.end(); ++it) {
      localFeatureList.insert(*it);
      
      double temp = validation(localFeatureList, trainingSet);
      if (temp > maxAccuracy) {
     	maxAccuracy = temp;
	feature = *it;
      }
      std::cout << "Acurracy with features: ";
      printFeatureList(localFeatureList);
      std::cout << " is: " << temp << std::endl;
      
      localFeatureList.erase(*it);
    }
    std::cout << "Best choice is to add feature: " << feature << std::endl;
    localFeatureList.insert(feature);
    remainingFeatures.erase(feature);
    if (maxAccuracy > accuracy) {
      featureList = localFeatureList;
      accuracy = maxAccuracy;
    } 
  }

  return featureList;
}

std::set<int> backwardElimination
(const std::vector<Object> &trainingSet) {
  if (trainingSet.size() == 0) {
    std::cout << "Error: Training set has no objects" << std::endl;
    exit(0);
  }

  // This feature list is contains the best one we find
  std::set<int> featureList;
  // This feature list is contains the best list at every step. 
  std::set<int> localFeatureList;
  // Fill feature list with every feature
  for (int i = 1; i < trainingSet.at(0).features.size(); ++i) {
    localFeatureList.insert(i);
  }
  double accuracy = validation(localFeatureList, trainingSet);
  int feature = -1;

  while (localFeatureList.size() > 1) {
    double maxAccuracy = 0;
    for (auto it = localFeatureList.begin(); it != localFeatureList.end(); ++it) {
      localFeatureList.erase(*it);
      
      double temp = validation(localFeatureList, trainingSet);
      if (temp > maxAccuracy) {
     	maxAccuracy = temp;
	feature = *it;
      }
      std::cout << "Acurracy with features: ";
      printFeatureList(localFeatureList);
      std::cout << " is: " << temp << std::endl;
      
      localFeatureList.insert(*it);
    }
    std::cout << "Best choice is to remove feature: " << feature << std::endl;
    localFeatureList.erase(feature);
    if (maxAccuracy > accuracy) {
      featureList = localFeatureList;
      accuracy = maxAccuracy;
    }
  }
  
  return featureList;
}

void printFeatureList
(const std::set<int> &featureList) {
  if (featureList.empty())
    return;
  std::cout << "{";
  for (auto it = featureList.begin(); it != --featureList.end(); ++it) {
    std::cout << *it << ", ";
  }
  std::cout << *(--featureList.end()) << '}' << std::flush;
}
