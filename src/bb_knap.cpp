 // Example program
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;

struct Item {
  float value;
  float weight;
  int id;
} ;

std::ostream& operator << (std::ostream& o, const Item& a)
{
  o << "(weight: " << a.weight << ", value: " << a.value << ")";
  return o;
}

// Sort by descending order (best value first)
bool sortByDensity(const Item& lhs, const Item& rhs) {
  return (lhs.value/lhs.weight) > (rhs.value/rhs.weight);
}

struct Problem {
  int n;
  int capacity;
  std::vector<Item> items;
};

struct Node {
  int room;
  std::vector<int> local_constraint; // indexes of the items to take
  std::vector<int> constraints; // local constraints too but relative to estimation
  int estimation = 0;
  int value = 0;
};


std::ostream& operator << (std::ostream& o, const Node& a) {
  o << "Optimistic estimation: " << a.estimation << endl;
  o << "Value: " << a.value << endl;
  o << "Capacity left: " << a.room << endl;

  return o;
}

Problem readProblem(std::string fp) {
  std::vector<Item> items;
  std::ifstream infile(fp);
  int n, capacity;

  infile >> n >> capacity;
  
  for (int i = 0; i < n; ++i) {
    float v, w;
    infile >> v >> w;
    items.push_back(Item{v,w,i});
  }
  return Problem{n, capacity, items};
 
}

template<typename T>
void printVector(std::vector<T> vect) {
  for (int i = 0; i < vect.size(); ++i)
    std::cout << vect[i] << " ";
  std::cout << std::endl;
}


void optimisticEstimation(Node* node, const std::vector<Item>& sortedItems, int capacity) {
  node->estimation = 0;
  node->value = 0;
  node->room = capacity;
  int room = capacity;

  for(int i : node->local_constraint) {
    room -= sortedItems[i].weight;
  }
  for(int i : node->local_constraint) {
    node->estimation += sortedItems[i].value;
    node->constraints[i] = 0; // prevent from using the item twice
  }
  
  for(int i = 0; i < sortedItems.size(); ++i) {
    if (room == 0) break;

    if (node->constraints[i] != 0) {
	if (sortedItems[i].weight <= room) {
	  room -= sortedItems[i].weight;
	  node->estimation += sortedItems[i].value;
	} else {
	  node->estimation += sortedItems[i].value * (room/sortedItems[i].weight);
	  room = 0;
      } 
    }
  }
  for(int i : node->local_constraint) {
    node->room -= sortedItems[i].weight;
  }
  for(int i : node->local_constraint) {
    node->value += sortedItems[i].value;
    node->constraints[i] = 1; // restoring value
  }
}


int main(int argc, char**argv){

  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " PROBLEM" << std::endl;
    return 1;
  }

  auto problem = readProblem(argv[1]);
  auto items = problem.items;
  int capacity = problem.capacity;

  int best_value = 0;
  vector<int> best_solution(items.size(), 0);

  //printVector(items);

  std::sort(items.begin(), items.end(), sortByDensity);

  //  printVector(items);

  vector<Node> treeFront;
  
  // Init
  Node root;
  root.constraints = vector<int>(items.size(), 1);
  optimisticEstimation(&root, items, capacity);

  treeFront.push_back(root);

  //for (auto  n : treeFront)
    //    cout << n;

  vector<Node> newTreeFront;

  int depth = 0;

  for(int i = 0; i < items.size(); ++i) {
    if (treeFront.size() == 0) break;
    depth++;
    //cout << "======= " << depth << " level =======" << endl; 
    // second level
    newTreeFront.clear();
    for (auto n : treeFront) {
      Node left, right;

      if (n.room > 0 && n.estimation > best_value) {
	left.local_constraint = n.local_constraint;
	left.constraints = n.constraints;
	left.local_constraint.push_back(i);
	optimisticEstimation(&left, items, capacity);
	newTreeFront.push_back(left);
      
	right.local_constraint = n.local_constraint;
	right.constraints = n.constraints;
	right.constraints[i] = 0;
	optimisticEstimation(&right, items, capacity);
	newTreeFront.push_back(right);
      }
    }

    treeFront.assign(newTreeFront.begin(), newTreeFront.end());
  
    for (auto  n : treeFront) {
      if (n.room >= 0) {
	if (n.value > best_value) {
	  best_value = n.value;
	  best_solution = n.local_constraint;
	  //	  cout << n;
	}
      }
    }
  }

 vector<int> temp(items.size(),0);

 for (int i = 0; i < items.size(); ++i) {
   if(std::find(best_solution.begin(), best_solution.end(), i) != best_solution.end()) {
     /* v contains x */
     temp[i] = 1;
   } else {
     /* v does not contain x */
     temp[i] = 0;
   }
 }
      vector<int> sol(items.size(),0);
  cout << best_value << " " << 1 << endl;
  for (int i = 0; i < items.size(); ++i) {
    sol[items[i].id] = temp[i]; 
  }

  printVector(sol);

 

}
