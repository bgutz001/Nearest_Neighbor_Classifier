#include <iostream>
#include <vector>
#include <assert.h>
#include <math.h>
#include <fstream>
#include <string>
#include <cstdio>

#define MAX_NUM_FEATURES 1000

struct Object {
  // First item is class
  // The rest are features
  std::vector<double> features;
  
  Object() {

  }

  void print() {
    std::cout << "features length: " << features.size() << std::endl;
    for (auto it = features.begin(); it != features.end(); ++it) {
      std::cout << *it << ' ';
    }
    std::cout << std::endl;
  }
  
};

// Returns the distance between two objects.
// featureIndicies is the idindices of the features to use in the object's feature list
// obj1 and obj2 are the objects you are calculating the distance between
double distance
(const std::vector<int> &featureIndicies, const Object &obj1, const Object &obj2);

// Returns the index of the nearest neighbor.
// featureIndicies is the idindices of the features to use in the distance formula.
// objects should be a vector of all objects.
// index should be the index of the object in objects that you want to find the nearest neighbor for.
int nearestNeighbor
(const std::vector<int> &featureIndicies, const std::vector<Object> &objects, const int index);

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
(const std::vector<int> &featureIndicies, const std::vector<Object> &objects);

// Greedy forward selection algorithm
// Add one feature at a time picking the best one to expand upon
std::vector<int> forwardSelection
(std::vector<Object> &trainingSet);

// Greedy backward elimination algorithm
// Start with a classification using all the features
// Get rid of features one by one until they start to decrease accuracy
std::vector<int> backwardElimination
(std::vector<Object> &trainingSet);

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
      
      instances.back().features.push_back(temp);
      std::size_t pos = line.find(' ', 0);
      if (pos == std::string::npos) {
	break;
      }
      line = line.substr(pos);
    }
  }

  file.close();
  std::cout << "Done reading input data" << std::endl;
  
  normalize(instances);
  std::cout << "Done normalizing input data" << std::endl;

  /*
  // DEBUG
  std::cout << "num instances: " << instances.size() << std::endl;
  for (auto it = instances.begin(); it != instances.end(); ++it) {
    it->print();
  }
  // * DEBUG
  */

  std::vector<int> featureList = backwardElimination(instances);


  // OUTPUT
  std::cout << "Feature list {";
  for (int i = 0; i < featureList.size()-1; ++i) {
    std::cout << featureList.at(i) << ", ";
  }
  std::cout << featureList.back() << "} is the best feature subset, with an accuracy of "
	    << validation(featureList, instances) << std::endl;
  
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
  assert(index < objects.size());

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

  return minIndex;
}

void normalize
(std::vector<Object> &objects) {
  // Check if there are features to normalize
  if (objects.at(0).features.size() <= 1) {
    return;
  }

  double max = 0;
  // Go through features but skip index 0 (class)
  for (int i = 1; i < objects.at(0).features.size(); ++i) {
    // Go through objects and find max value of feature
    for (int j = 0; j < objects.size(); ++j) {
      if (std::abs(objects.at(j).features.at(i)) > max) {
	max = std::abs(objects.at(j).features.at(i));
      }
    }
  }

  // Normalize
  for (int i = 1; i < objects.at(0).features.size(); ++i) {
    for (int j = 0; j < objects.size(); ++j) {
      objects.at(j).features.at(i) /= max;
    }
  }

}

double validation
(const std::vector<int> &featureIndicies, const std::vector<Object> &objects) {
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


std::vector<int>
forwardSelection(std::vector<Object> &trainingSet) {
  if (trainingSet.size() == 0) {
    std::cout << "Error: Training set has no objects" << std::endl;
    exit(0);
  }

  std::vector<int> featureList;
  double accuracy = 0;
  int feature = 1;
  bool gain = true; // If we are still gaining accuracy

  while(gain) {
    gain = false;
    for (int i = 1; i < trainingSet.at(0).features.size(); ++i) {
      // Check if i is already in features list
      bool exists = true;
      while (exists) {
	exists = false;
	for (auto it = featureList.begin(); it != featureList.end(); ++it) {
	  if (i == *it) {
	    exists = true;
	    ++i;
	    break;
	  }
	}
      }
      
      featureList.push_back(i);
      double curAccuracy = validation(featureList, trainingSet);
      std::cout << "Acurracy with features: ";
      for (auto temp = featureList.begin(); temp != featureList.end(); ++temp) {
	std::cout << *temp << ' ';
      }
      
      std::cout << "is: " << curAccuracy << std::endl;
      if (curAccuracy > accuracy) {
	accuracy = curAccuracy;
	gain = true;
	feature = i;
      }
      featureList.pop_back();
    }
    if (gain) {
      std::cout << "Best choice is feature: " << feature << std::endl;
      featureList.push_back(feature);
    }
  } 
  
  return featureList;
}

std::vector<int> backwardElimination
(std::vector<Object> &trainingSet) {
  if (trainingSet.size() == 0) {
    std::cout << "Error: Training set has no objects" << std::endl;
    exit(0);
  }

  // Fill feature list with every feature
  std::vector<int> featureList;
  for (int i = 1; i < trainingSet.at(0).features.size(); ++i) {
    featureList.push_back(i);
  }
  double accuracy = 0;
  bool gain = true;

  while (gain) {
    gain = false;

    int featureIndex;
    for (int i = 0; i < featureList.size(); ++i) {
      std::vector<int> temp;
      // Copy all features but one
      for (int j = 0; j < featureList.size(); ++j) {
	if (i == j)
	  continue;
	temp.push_back(featureList.at(j));
      }
      double curAccuracy = validation(temp, trainingSet);
      std::cout << "curAccuracy " << curAccuracy << std::endl;
      if (curAccuracy > accuracy) {
	gain = true;
	accuracy = curAccuracy;
	featureIndex = i;
      }
    }
    if (gain) {
      std::cout << "Best choice is feature: " << featureList.at(featureIndex) 
		<< " accuracy " << accuracy << std::endl;
      std::swap(featureList.at(featureIndex), featureList.back());
      featureList.pop_back();
    }
  }

  return featureList;
}
