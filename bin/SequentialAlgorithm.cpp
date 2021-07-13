#include <bits/stdc++.h>
#include <sys/time.h>

using namespace std;

class graph {
    private:
        friend class partitions;
        int vertices;   //Number of vertices
        int edges;  //Number of edges    
    public:
        //The index of the vector adjacncyList represents the vertex, I am assuming the vertices to be numbered form 0 to n-1
        vector <vector <int>> adjacencyList {};
        //Constructors for the graph class
        graph();
        graph(int i);
        //Method to add edge in the graph
        void addEdge(int a, int b);
        //Returns the number of vertices in the graph
        int size();
        //Prints the graph
        void print();
};

//Defining the methods in graph class
graph::graph() {}
vector <int> colorVertex {};

graph::graph(int i) {
    vertices = i;
    for(int j = 0; j < i; ++j) {
        adjacencyList.push_back({});
    }
}

void graph::addEdge(int a, int b) {
    bool insert = 1;
    //Checking if the vertex is already a neighbour 
    for(int i = 0; i < adjacencyList[a].size(); ++i) {
        if(adjacencyList[a][i] == b) {
            insert = 0;
            break;
        }
    }
    //Insert if not neighbour 
    if(insert) {
        adjacencyList[a].push_back(b);
        adjacencyList[b].push_back(a);
        edges++;
    }
}

int graph::size() {
    return vertices;
}

void graph::print() {
    ofstream output;
    int k = -1;
    output.open("input_params.txt");
    for(int i = 0; i < adjacencyList.size(); ++i) {
         output << i << ": ";
        for(int j = 0; j < adjacencyList[i].size(); ++j) {     
            output << adjacencyList[i][j] << " ";
        }
        output << k << endl;
    }
    output.close();
}

graph * inputGraph;

//Class for the partitions of the graph
class partitions {
    private:
        //vector <int> partitionedGraph {};
        map <int, bool> partitionedGraph;
    public:
        partitions();
        //static void partitionGraph();
        
        void insertIntoPartition(int i);
        void checkInternalFunc();
        static void colorPartition(partitions graphPartition);
};

//Defining constructors for partitions class
partitions::partitions() {}

void partitions::insertIntoPartition(int i) {
    this->partitionedGraph.emplace(pair <int, bool> (i, 0));
}

void partitions::checkInternalFunc() {
    //Check if the vertex is an internal vertex or external vertex 
    map <int, bool>::iterator iterate = this->partitionedGraph.begin();
    for(iterate = this->partitionedGraph.begin(); iterate != this->partitionedGraph.end(); ++iterate) {
        bool change = 1;
        for(int i = 0; i < inputGraph->adjacencyList[iterate->first].size(); ++i) {    
            map <int, bool>::iterator searchNumber = this->partitionedGraph.find(inputGraph->adjacencyList[iterate->first][i]);
            if(searchNumber == this->partitionedGraph.end()) {
                change = 0;
                break;
            }
        }
        if(change) {
            iterate->second = 1;
        }
    }
}


//Coloring the Partitions
void partitions::colorPartition(partitions graphPartition) {
    map <int, bool>::iterator iterate = graphPartition.partitionedGraph.begin();
    for(iterate = graphPartition.partitionedGraph.begin(); iterate != graphPartition.partitionedGraph.end(); ++iterate) {
        map <int, bool> neighbourColors;
        if(iterate->second) {
            for(int i = 0; i < inputGraph->adjacencyList[iterate->first].size(); ++i) {
                neighbourColors.emplace(pair <int, bool> (colorVertex[inputGraph->adjacencyList[iterate->first][i]], 0));
            }
            int color = 0;
            while(true) {
                map <int, bool>::iterator searchElement = neighbourColors.find(color);
                if(searchElement == neighbourColors.end()) {
                    colorVertex[iterate->first] = color;
                    break;
                }
                color++;
            }
        }

        else {
            for(int i = 0; i < inputGraph->adjacencyList[iterate->first].size(); ++i) {
                neighbourColors.emplace(pair <int, bool> (colorVertex[inputGraph->adjacencyList[iterate->first][i]], 0));
            }
            int color = 0;
            while(true) {
                map <int, bool>::iterator searchElement = neighbourColors.find(color);
                if(searchElement == neighbourColors.end()) {
                    colorVertex[iterate->first] = color;
                    break;
                }
                color++;
            }
        }
    }
}

int main() {
    
    std::chrono::time_point<std::chrono::system_clock> start, end;

    srand(time(nullptr));
    int numberOfVertices, numberOfPartitions = 1;
    cin >> numberOfVertices;
    inputGraph = new graph(numberOfVertices);
    //graph a (numberOfVertices);
    //Creating a graph
    for(int i = 0; i < numberOfVertices; ++i) {
        for(int j = i; j < numberOfVertices; ++j) {
            if(j == i) {
                continue;
            }
            if(rand()%2) {
                //Adding an edge in the graph
                inputGraph->addEdge(i, j);
                //a.addEdge(i,j);
            }
        }
    }
    //a.print();
    inputGraph->print();

    //Partitioning the graph into k partitions
    //For sequential algorithm the number of partitions is 1
    partitions * graphPartitions = new partitions [numberOfPartitions];

    //int remainingVertices = numberOfVertices%numberOfPartitions;

    for(int i = 0, j = 0; i < numberOfVertices; ++i, ++j) {
        colorVertex.push_back(-1);
        graphPartitions[i%numberOfPartitions].insertIntoPartition(i);
    }

    for(int i = 0; i < numberOfPartitions; ++i) {
        graphPartitions[i].checkInternalFunc();
    }

    start = std::chrono::system_clock::now();

    for(int i = 0; i < numberOfPartitions; ++i) {
        partitions::colorPartition(graphPartitions[i]);
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start; 

    ofstream output; 
    output.open("output.txt");

    output << "Number of colours used: " << (*max_element(colorVertex.begin(), colorVertex.end())) + 1 << endl;

    output << "Time taken by the algorithm using: " << (elapsed_seconds.count())*1000 << " milliseconds" << endl; 

    output << "Colours:" << endl;

    for(int i = 0; i < numberOfVertices; ++i) {
        output << "v" << i << " - " << colorVertex[i];
        if(i != numberOfVertices - 1) {
            output << ", ";
        }
    }
}
