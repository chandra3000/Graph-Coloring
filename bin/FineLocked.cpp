#include <bits/stdc++.h>

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
    for(int i = 0; i < adjacencyList.size(); ++i) {
        cout << i << ": ";
        for(int j = 0; j < adjacencyList[i].size(); ++j) {     
            cout << adjacencyList[i][j] << " ";
        }
        cout << endl;
    }
}

//Global resources
graph * inputGraph;                 //This pointer to the graph class stores the graph
vector <int> colorVertex {};        //Stores the final color of each vertex
pthread_mutex_t * lockVertex;       //An array of locks to assign locks to each vertex

//Class for the partitions of the graph
class partitions {
    private:
        map <int, bool> partitionedGraph;

    public:
        partitions();
        void insertIntoPartition(int i);
        void checkInternalFunc();
        static void * colorPartition(void * graphPartition);
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
            //Condition for external vertex
            if(searchNumber == this->partitionedGraph.end()) {
                change = 0;
                break;
            }
        }
        //Status changed to internal vertex 
        if(change) {
            iterate->second = 1;
        }
    }
}

//Static class method to color the partitions 
void * partitions::colorPartition(void * graphPartition1) {
    partitions graphPartition = *(partitions *)graphPartition1;
    map <int, bool>::iterator iterate = graphPartition.partitionedGraph.begin();
    for(iterate = graphPartition.partitionedGraph.begin(); iterate != graphPartition.partitionedGraph.end(); ++iterate) {
        map <int, bool> neighbourColors;
        //Colouring internal vertex
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
            //Creating map to enter the elements in an incresing order
            map <int, bool> increasingNeighbours;
            increasingNeighbours.emplace(pair <int, bool> (iterate->first, 0));
            for(int i = 0; i < inputGraph->adjacencyList[iterate->first].size(); ++i) {
                increasingNeighbours.emplace(pair <int, bool> (inputGraph->adjacencyList[iterate->first][i], 0));
            }
            map <int, bool>::iterator iterateList = increasingNeighbours.begin();     
            
            //Accessing the locks in an increasing order of vertex value
            for(iterateList = increasingNeighbours.begin(); iterateList != increasingNeighbours.end(); ++iterateList) {
                pthread_mutex_lock(&lockVertex[iterateList->first]);
            }
            
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

            //Releasing the locks in the decreasing order of vertex values
            iterateList = increasingNeighbours.end();
            iterateList--;
            while(true) {
                pthread_mutex_unlock(&lockVertex[iterateList->first]);
                iterateList--;
                if(iterateList == increasingNeighbours.begin()) {
                    pthread_mutex_unlock(&lockVertex[iterateList->first]);
                    break;
                }
            }
        }
    }

    pthread_exit(NULL);
}

int main() {
    
    std::chrono::time_point<std::chrono::system_clock> start, end;

    srand(time(nullptr));
    int numberOfVertices, numberOfPartitions;
    ifstream input;
    input.open("input_params.txt");
    input >> numberOfPartitions >> numberOfVertices;
    inputGraph = new graph(numberOfVertices);

    //Creating a graph
    for(int i = 0; i < numberOfVertices; ++i) {
        int vertex;
        char character;
        input >> vertex >> character;
        int neighbour;
        while(true) {
            input >> neighbour;
            if(neighbour == -1) {
                break;
            }
            inputGraph->addEdge(vertex, neighbour);
        }
    }

    //Declaring thread and its attributes
    pthread_t numThreads[numberOfPartitions];
    pthread_attr_t attributes[numberOfPartitions];
    
    //Initialising the lock
    lockVertex = new pthread_mutex_t [numberOfVertices];
    
    for(int i = 0; i < numberOfVertices; ++i) {
        pthread_mutex_init(&lockVertex[i], NULL);
    }

    for(int i = 0; i < numberOfPartitions; ++i) {
        pthread_attr_init(&attributes[i]);
    }

    //Partitioning the graph into k partitions
    partitions * graphPartitions = new partitions [numberOfPartitions];

    for(int i = 0, j = 0; i < numberOfVertices; ++i, ++j) {
        colorVertex.push_back(-1);
        graphPartitions[i%numberOfPartitions].insertIntoPartition(i);
    }

    for(int i = 0; i < numberOfPartitions; ++i) {
        graphPartitions[i].checkInternalFunc();
    }

    //Recording start time 
    start = std::chrono::system_clock::now();

    //Calling the function using threads
    for(int i = 0; i < numberOfPartitions; ++i) {
        pthread_create(&numThreads[i], &attributes[i], partitions::colorPartition, &graphPartitions[i]);
    }

    //The parent thread waits for the slave threads to complete
    for(int i = 0; i < numberOfPartitions; ++i) {
        pthread_join(numThreads[i], NULL);
    }

    //Recording the end time
    end = std::chrono::system_clock::now();

    //Calculating the time taken
    std::chrono::duration<double> elapsed_seconds = end - start; 

    //Writing output to output to the output file
    ofstream output; 
    output.open("output.txt");

    output << "Fine Lock" << endl;

    output << "Number of colours used: " << (*max_element(colorVertex.begin(), colorVertex.end())) + 1 << endl;

    output << "Time taken by the algorithm using: " << (elapsed_seconds.count())*1000 << " milliseconds" << endl; 

    output << "Colours:" << endl;

    for(int i = 0; i < numberOfVertices; ++i) {
        output << "v" << i << " - " << colorVertex[i];
        if(i != numberOfVertices - 1) {
            output << ", ";
        }
    }

    output.close();
    input.close();
}
