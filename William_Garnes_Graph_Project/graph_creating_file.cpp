#include<Rcpp.h>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<queue>
#include<set>
#include<cmath>
#include<map>
#include<utility>
#include<iostream>
#include<stdlib.h>
#include<minheap.cpp>


#define MINIMUM -100000
#define TAB '\t'

using namespace Rcpp;


//holds information for a node's neighbor  
typedef struct nbr{
  unsigned int number; //holds neighbor's index in the graph
  unsigned int weight; //weight between a node and its neighbors
}neighbor;

//holds information for a node
typedef struct nd{
  double positionX; //node's X position
  double positionY; //node's Y position
  unsigned int number; //node's index int the graph 
  unsigned int size;
  std::vector<neighbor> neighbors; //holds all of the nodes a node is connected to
  std::vector<unsigned int> madeOf; //holds all of the nodes from the original graph that have been merged into this node
}node;

//holds all of the nodes 
static std::vector<node> graph;

//creates the initial graph with no coersening
void CreateGraph(CharacterVector x);

//reads a file at the selected file name and creates a graph from it
void ReadFile(CharacterVector fileName);

//coersens the graph
void CoersenGraph();

//tells which nodes will be merged to which
//if a node does not merge then at its index it will contain itself
std::vector<int> GetCombinations();

//sends which neighbor a node is best connected to
neighbor CheckNeigbors(std::set<unsigned int>& checked, std::vector<int>& combined, unsigned int index, int& max, int originalPoint = -1);

//sends a value back of how close nodes are, to see if they should be merged
double NodeConnectionValue(const node& node1, const node& node2, unsigned int weight);

//returns 1/distance between the nodes
double GeometricProximity(const node& node1, const node& node2);

//returns 1/the combined sizes of the nodes added together
double ClusterSize(const node& node1, const node& node2);

//returns the weight of the nodes / the square root of sizes 
//of the nodes multiplied by one another
double NormalizedConnectionStrength(const node& node1, const node& node2, unsigned int weight);

//returns the size of the intersections of the nodes neighbours / 
//the size of the union of the nodes neighbours
double NeighborhoodSimilarity(const node& node1, const node& node2);

//returns 1 / the degrees of the nodes multiplied
double Degree(const node& node1, const node& node2);

//m, More, and GiveNextNode are used to send nodes to an R file so that it can create a graph
static int m = 1;
NumericVector More();
CharacterVector GiveNextNode();

static std::ofstream writeFile;
static std::string writeFileName = "origin_file";
void ChangeWriteFileName(std::string fileName);

//this holds the distance between all of the nodes
static std::vector<std::vector<unsigned int> > distanceMatrix;
void CreateDistanceMatrix();

// [[Rcpp::export]]
void CreateGraph(CharacterVector fileName){
  m = 1;
  ReadFile(fileName);
}

//reads a file at the selected file name and creates a graph from it
void ReadFile(CharacterVector fileName){
  //opens a textfile to read
  std::ifstream readFile;
  std::string name = "textfiles\\"+fileName[0];
  readFile.open(name.c_str());
  
  //reads the first line which contains the number of nodes and fills the graph vector
  int nodeCount; readFile >> nodeCount;
  graph = std::vector<node>(nodeCount,(node){0.0,0.0,0,1,std::vector<neighbor>(),std::vector<unsigned int>()});
  
  //reads the nexts line which has the edges and their nodes
  std::string textLine;
  readFile.ignore();
  //loops and get lines until the node positions in the textfile are reached
  while(true){
    std::getline(readFile, textLine);
    if(textLine == "node positions")
      break;
    std::stringstream numbers(textLine);
    unsigned int dummy; numbers >> dummy;
    unsigned int node1; numbers >> node1;
    unsigned int node2; numbers >> node2;
    //places each node as a neighbor of each other
    graph[node1].neighbors.push_back((neighbor){node2,1});
    graph[node2].neighbors.push_back((neighbor){node1,1});
  }
  
  //loops and gets the remaining lines which have the nodes positions
  while(std::getline(readFile, textLine)){
    std::stringstream numbers(textLine);
    unsigned int node; numbers >> node;
    double x; numbers >> x;
    double y; numbers >> y;
    graph[node].positionX = x;
    graph[node].positionY = y;
    graph[node].number = node;
    graph[node].madeOf.push_back(node);
  }
  readFile.close();
  CreateDistanceMatrix();
}

//coersens the graph
// [[Rcpp::export]]
void CoersenGraph(){
  //holds all which nodes will be connected to which
  //if a node does not merge with another then it is sent with itself
  std::vector<int> combinations = GetCombinations();
  //will hold all of the information for the new graph
  std::vector<node> newGraph;
  //holds were all of the nodes in the old graph are located in the new graph
  std::vector<int> locations(graph.size(),-1);
  
  int length = combinations.size();
  
  //goes through the combinations and makes nodes out of them
  for(int index = 0; index < length; index++){
    int partner = combinations[index];
    if(partner < index)
      continue;
    
    node node1 = graph[index];
    node node2 = graph[partner];
    
    //this map holds the neighbors and weights of the two nodes
    std::map<unsigned int,unsigned int> connections;
    std::map<unsigned int,unsigned int>::iterator it;
    
    unsigned int count = (unsigned)node1.neighbors.size();
    //puts node1's neighbors into the connections map
    for(unsigned int x = 0; x < count; x++){
      neighbor n = node1.neighbors[x];
      if(locations[n.number] != -1 && n.number != node2.number)
        connections.insert(std::pair<unsigned int,unsigned int>(n.number,n.weight));
    }
    
    //puts node2's neighbors into the connections map if node1 and node2 are different
    if(partner != index){
      count = node2.neighbors.size();
      for(unsigned int x = 0; x < count; x++){
        neighbor n = node2.neighbors[x];
        if(locations[n.number] == -1 || n.number == node1.number)
          continue;
        it = connections.find(n.number);
        if(it == connections.end())
          connections.insert(std::pair<unsigned int, unsigned int>(n.number,n.weight));
        
        else
          it->second+=n.weight;
      }
    }
    
    node newNode;
    //if there are not two nodes being merged then the newNodes information matches the old nodes
    if(partner == index){
      newNode.positionX = node1.positionX;
      newNode.positionY = node1.positionY;
      newNode.number = newGraph.size();
      newNode.size = node1.size;
      newNode.madeOf = node1.madeOf;
      
      locations[index] = newNode.number;
    }
    
    //if there are two nodes being merged then the newNodes information needs to come from both nodes
    else{
      newNode.number = newGraph.size();
      newNode.size = node1.size + node2.size;
	  
      //newNode.positionX = (node1.size * node1.positionX + node2.size * node2.positionX) / newNode.size;
      //newNode.positionY = (node1.size * node1.positionY + node2.size * node2.positionY) / newNode.size;
	  newNode.positionX = (node1.positionX + node2.positionX) / 2;
	  newNode.positionY = (node1.positionY + node2.positionY) / 2;
	  
      newNode.madeOf = node1.madeOf;
      
      unsigned int length = node2.madeOf.size();
      for(unsigned int x = 0; x < length; x++)
        newNode.madeOf.push_back(node2.madeOf[x]);
      
      locations[partner] = newNode.number;
	  locations[index] = newNode.number;
    }
    
    //tells which nodes in the connection map have been looked at already
    std::set<unsigned int> reviewed;
    
    //connect new nodes to their new neighbors
    for(it = connections.begin(); it != connections.end(); it++){
      unsigned int connection1 = it->first;
      if(reviewed.count(connection1) == 1)
        continue;
      reviewed.insert(connection1);
      
      //this is the node that the newNode is connected to
      node* newNodeConnection = &(newGraph[locations[connection1]]);
      unsigned int weight;
      
      //checks to see if newNodeConnection is merged to any node that newNode is connected to
      std::map<unsigned int,unsigned int>::iterator it2;
      it2 = connections.find(combinations[connection1]);
      
      //if newNodeConnection is not merged to any node that newNode is connected to set weight equal to
      //the weight of newNodeConnection
      if(it2 == connections.end()){
        weight = it->second;
      }
      
      //if newNodeConnection is merged to any node that newNode is connected to set weight equal to
      //the weight of those two combined nodes
      else{
        weight = it->second + it2->second;
        reviewed.insert(it2->first);
      }
      
      //make each of the nodes a neighbor of each other
      newNodeConnection->neighbors.push_back((neighbor){newNode.number,weight});
      newNode.neighbors.push_back((neighbor){newNodeConnection->number,weight});
      
      
    }
    newGraph.push_back(newNode);
  }
  graph = newGraph;
  CreateDistanceMatrix();
}

//tells which nodes will be merged to which
//if a node does not merge then at its index it will contain itself
std::vector<int> GetCombinations(){
  unsigned int graphSize = (unsigned)graph.size();
  //tells which nodes have been combined so far
  std::vector<int> combined(graphSize,-1);
  
  //loops through all nodes in the graph and merges them
  for(unsigned int index = 0; index < graphSize; index++){
    if(combined[index] != -1)
      continue;
    
    std::set<unsigned int> checked;
    std::set<unsigned int> ::iterator it;
    //loop through nodes neighbors and tests finds most suitable to combined
    int max = MINIMUM;
    neighbor best = CheckNeigbors(checked, combined, index, max);
    
    std::queue<unsigned int> neighbors;
    for(it = checked.begin(); it != checked.end(); it++)
      neighbors.push(*it);
    
    //loop through neighbors neighbors and tests finds most suitable to combine
    while(!neighbors.empty()){
      int potenitalMax = 0;
      neighbor potenital = CheckNeigbors(checked,combined,neighbors.front(),potenitalMax,(signed)index);
      if(potenitalMax > max){
        max = potenitalMax;
        best = potenital;
      }
      neighbors.pop();
    }
    
    if(max > MINIMUM){
      combined[index] = best.number;
      combined[best.number] = index;
    }
    
    else
      combined[index] = index;
    
  }
  return combined;
}

//sends which neighbor a node is best connected to
neighbor CheckNeigbors(std::set<unsigned int>& checked, std::vector<int>& combined, unsigned int index, int& max, int originalPoint){
  node uncombined = graph[index];
  std::vector<neighbor>* unCombinedNeighbors = &uncombined.neighbors;
  unsigned int size = unCombinedNeighbors->size();
  
  neighbor best = (neighbor){0,0};
  
  for(unsigned int x = 0; x < size; x++){
    neighbor unCombinedNeighbor = (*unCombinedNeighbors)[x];
    unsigned int number = unCombinedNeighbor.number;
    if(combined[number] != -1)
      continue;
    checked.insert(number);
    node otherUncombined = graph[number];
    
    int value; 
    if(originalPoint == -1)
      value = NodeConnectionValue(uncombined,otherUncombined,unCombinedNeighbor.weight);
    else
      value = NodeConnectionValue(graph[originalPoint],otherUncombined,0);
    
    if(value > max){
      max = value;
      best = unCombinedNeighbor;
    }
  }
  
  return best;
}

//sends a value back of how close nodes are, to see if they should be merged
double NodeConnectionValue(const node& node1, const node& node2, unsigned int weight){
  return (GeometricProximity(node1,node2) + ClusterSize(node1,node2) + 
          NormalizedConnectionStrength(node1,node2,weight) + NeighborhoodSimilarity(node1,node2) +
          Degree(node1,node2));
}


//returns 1/distance between the nodes
double GeometricProximity(const node& node1, const node& node2){
  double xValue = std::pow(node2.positionX-node1.positionX,2);
  double yValue = std::pow(node2.positionY-node1.positionY,2);
  double distance = std::sqrt(xValue+yValue);
  return 1.0/distance;
}

//returns 1/the combined sizes of the nodes added together
double ClusterSize(const node& node1, const node& node2){
  unsigned int totalSize = node1.size + node2.size;
  return 1.0/(double)totalSize;
}

//returns the weight of the nodes / the square root of sizes 
//of the nodes multiplied by one another
double NormalizedConnectionStrength(const node& node1, const node& node2, unsigned int weight){
  double denominator = std::sqrt(node1.size*node2.size);
  return (double)weight/denominator;
}

//returns the size of the intersections of the nodes neighbours / 
//the size of the union of the nodes neighbours
double NeighborhoodSimilarity(const node& node1, const node& node2){
  std::set<unsigned int> neighbors;
  std::set<unsigned int>::iterator it;
  
  unsigned int length = node1.neighbors.size();
  for(unsigned int x = 0; x < length; x++)
    neighbors.insert(node1.neighbors[x].number);
  
  int interesectCount = 0;
  length = node2.neighbors.size();
  for(unsigned int x = 0; x < length; x++)
    if(neighbors.count(node2.neighbors[x].number) == 1)
      interesectCount++;
    
    for(unsigned int x = 0; x < length; x++)
      neighbors.insert(node2.neighbors[x].number);
    
    return (double)interesectCount/(double)neighbors.size();
}

//returns 1 / the degrees of the nodes multiplied
double Degree(const node& node1, const node& node2){
  double degree = node1.neighbors.size() * node2.neighbors.size();
  return 1.0/degree;
}

// [[Rcpp::export]]
NumericVector More(){
  NumericVector m1(1);
  m1[0] = m;
  if(m == 0){
    m = 1;
  }
  
  return m1;
}

// [[Rcpp::export]]
CharacterVector GiveNextNode(){
  static unsigned int index = 0;
  if(index == 0){
    writeFile.open(("textfiles\\"+writeFileName+".txt").c_str(),std::ios::out | std::ios::trunc);
  }
  node n = graph[index++];
  std::stringstream transform; transform << "number: " << n.number << " size:" << n.size;
  transform << " position: (" << n.positionX << "," << n.positionY << ") ";
  transform << "neighbors: ";
  unsigned int length = n.neighbors.size();
  for(unsigned int x = 0; x < length; x++){
    transform << "(" << n.neighbors[x].number << "," << n.neighbors[x].weight << ") ";
  }
  
  transform << "made of: ";
  length = n.madeOf.size();
  for(unsigned int x = 0; x < length; x++){
    transform << n.madeOf[x] << " ";
  }
  
  
  std::string data;
  std::getline(transform,data);
  writeFile << data << std::endl;
  if(index >= graph.size()){
    m = 0;
    index = 0;
    writeFile.close();
  }
  return data;
}

// [[Rcpp::export]]
void ChangeWriteFileName(CharacterVector fileName){
  writeFileName = fileName[0];
}

// [[Rcpp::export]]
void CreateTulipFile(){
  std::ofstream tulipFile;
  std::stringstream edges;
  std::stringstream positions;
  std::string printLine;
  unsigned int size = (unsigned)graph.size();
  
  tulipFile.open(("tulipfiles\\"+writeFileName+".tlp").c_str(),std::ios::out | std::ios::trunc);
  
  int edgeNumber = 0;
  for(unsigned int x = 0; x < size; x++){
	  node* nextNode = &graph[x];
	  positions << TAB << TAB << "(node " << x << " \"(" << nextNode->positionX << "," << nextNode->positionY << ",0)\")\n";
	  unsigned int count = (unsigned)nextNode->neighbors.size();
	  for(unsigned int y = 0; y < count; y++){
		if(nextNode->neighbors[y].number > x){
			edges << TAB << "(edge " << edgeNumber++ << " " << nextNode->number << " " << nextNode->neighbors[y].number << ")\n";
		}
	  }
		  
  }

  
  
  //creates tulip format
  tulipFile << "(tlp \"2.0\"" << std::endl;
  tulipFile << ";" << std::endl;
  tulipFile << TAB << "(nodes";
  
  //adds the nodes
  for(unsigned int x = 0; x < size; x++)
    tulipFile << " " << x;
  tulipFile << ")" << std::endl;
  
  //adds the edges
  while(std::getline(edges,printLine)){
	  tulipFile << printLine << std::endl;
  }
  
  //creates the viewColor property
  tulipFile << TAB << "(property 0 color \"viewColor\"" << std::endl;
  tulipFile << TAB << TAB << "(default \"(0,0,255,255)\" \"(0,0,0,255)\")" << std::endl;
  tulipFile << TAB << ")" << std::endl;
  
  //creates the viewLabel property
  tulipFile << TAB << "(property 0 string \"viewLabel\"" << std::endl;
  tulipFile << TAB << TAB << "(default \"\" \"\")" << std::endl;
  for(unsigned int x = 0; x < size; x++)
	  tulipFile << TAB << TAB << "(node " << x << " \"" << x << "\"" << ")" << std::endl;
  tulipFile << TAB << ")" << std::endl;
  
  //creates the viewSelection property
  tulipFile << TAB << "(property 0 bool \"viewSelection\"" << std::endl;
  tulipFile << TAB << TAB << "(default \"false\" \"false\")" << std::endl;
  tulipFile << TAB << ")" << std::endl;
  
  //creates the viewSize property
  tulipFile << TAB << "(property 0 size \"viewSize\"" << std::endl;
  tulipFile << TAB << TAB << "(default \"(5,5,5)\" \"(1,1,1)\")" << std::endl;
  tulipFile << TAB << ")" << std::endl;
  
  //creates the viewShape property
  tulipFile << TAB << "(property 0 int \"viewShape\"" << std::endl;
  tulipFile << TAB << TAB << "(default \"2\" \"0\")" << std::endl;
  tulipFile << TAB << ")" << std::endl;
  
  //creates the viewLayout property
  tulipFile << TAB << "(property 0 layout \"viewLayout\"" << std::endl;
  tulipFile << TAB << TAB << "(default \"(0,0,0)\" \"()\")" << std::endl;
  while(std::getline(positions,printLine)){
	  tulipFile << printLine << std::endl;
  }
  tulipFile << TAB << ")" << std::endl;
  
  //ends the file
  tulipFile << ")";
  tulipFile.close();
  
}

void CreateDistanceMatrix(){
  distanceMatrix.clear();
  unsigned int size = (unsigned)graph.size();
  
  distanceMatrix = std::vector<std::vector<unsigned int> >(size,std::vector<unsigned int>(size,100000));
  
  for(unsigned int x = 0; x < size; x++){
    minheap heap;
    heap.Add(x,0);
    distanceMatrix[x][x] = 0;
    while(!heap.IsEmpty()){
      std::pair<unsigned int, unsigned int> next = heap.Top();
      heap.RemoveTop();
      node next_node = graph[next.first];
      unsigned int neighbor_count = next_node.neighbors.size();
      for(unsigned int index = 0; index < neighbor_count; index++){
        neighbor n = next_node.neighbors[index];
        if(distanceMatrix[x][n.number] > n.weight + next.second){
          distanceMatrix[x][n.number] = n.weight + next.second;
          heap.Add(n.number,n.weight+next.second);
        }
      }
      
    }
  }
}

// [[Rcpp::export]]
NumericVector GiveDistanceMatrix(){
  unsigned long size = distanceMatrix.size();
  NumericVector m1(size*size);
  int number = 0;
  for(unsigned long x = 0; x < size; x++){
    for(unsigned long y = 0; y < size; y++){
      m1[number++] = distanceMatrix[x][y];
    }
  }
  return m1;
}

// [[Rcpp::export]]
NumericVector GiveMatrixLength(){
  NumericVector m1(1);
  m1[0] = distanceMatrix.size();
  return m1;
}
// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically 
// run after the compilation.
//


/*** R

*/
