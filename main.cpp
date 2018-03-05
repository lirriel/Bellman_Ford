//
// Created by USER on 21.11.2016.
//
#include <iostream>
#include <sstream>
#include <fstream>
#include "vector"
#include "climits"
#include "algorithm"
#include "stdexcept"

using namespace std;

/**
* A structure of graph. It looks like: the first node (from), the second node (to) and the weight of this way (cost)
 */
struct Edge {

    /**
     * First node
     */
    int from;

    /**
     * Second node
     */
    int to;

    /**
     * Cost of way between @param from and @param to
     */
    int cost;

};

/**
 * A class representing Bellmman & Ford solution
 */

class BellmanFord {

public:

    /**
     * Default constructor. You should not implement it.
     */
    BellmanFord () {};

    /**
     * Initialization method
     * Note that it can be called multiple times so don't forget about memory deallocation
     * Also, the pointer to the array shuold not be shared between the object of SCCSolver and the exterior program.
     * @param n the number of edges
     * @param m the number of vertixes
     * @param array size of array @param n x 3, first colomn is @param from (struct Edge) node,
     * second colomn is @param to (struct Edge) node, third colomn is @param cost (struct Edge) - weight of way
     * @param startV vertex start
     * @post initialized == true
     */
    void initialize(int** array, int n, int m, int startV);

    /**
    * @return true if the object was initialized; otherwise false
    */
    bool isInitialized();

    /**
     * General method of solving. You should choose between solvePositive and solveNegative
     * @pre initialize == true;
     * @throws logic_error if called before initialization
     */
    void solve();

    /**
     * The solving method with negative cycle(s)
     */
    void solveNegative();

    /**
     * The solving method without negative cycle(s)
     */
    void solvePositive();

    /**
     * @return minimum cost of way from start vertex
     */
    int getCost();

    /**
     * @return array of vectors<int> which paths sorted by the smallest to biggest costs
     */
    std::vector<int>* getPath();

    /**
     * Destructor, don't forget to free all the allocated memory
     */
    ~BellmanFord();

private:

    /**
     * The starting vertex
     */
    int start;

    /**
     * The number of vertexes
     */
    int numVertexes;

    /**
     * The number of edges
     */
    int numEdges;

    /**
     * The representation of the input graph (list of vertex)
     */
    std::vector<Edge> graph;

    /**
     * Flag for negative cycle
     */
    bool isNegative = false;

    /**
     * The ARRAY of vectors<int> - all paths with the lowest cost
     */
    std::vector<int>* path;

    /**
     * The costs minimum after Bellman-Ford implementation
     */
    int minCost;

    /**
     * is the problem initialized
    */
    bool initialized = false;


};



int* directories;

void BellmanFord::initialize(int **array, int n, int m, int startV) {
    numEdges = n;
    numVertexes = m;
    start = startV-1;
    Edge e;
    graph.clear();
    for (int i = 0; i < n; ++i) {
        e.from = array[i][0]-1;
        e.to = array[i][1]-1;
        e.cost = array[i][2];
        graph.push_back(e);
    }
    path = new vector<int>[numVertexes-1];
    directories = new int[numVertexes];
    initialized = true;
}

bool BellmanFord::isInitialized() {
    return initialized;
}

void BellmanFord::solve() {
    if (!isInitialized())
        throw logic_error("Should be initialized first!");
    minCost = 0;
    for (int i = 0; i < graph.size(); ++i) {
        if (graph[i].cost < 0) {
            isNegative = true;
            break;
        }
    }
    if (isNegative)
        solveNegative();
    else
        solvePositive();
}

void BellmanFord::solveNegative() {
    for (int i = 0; i < numVertexes; ++i) {
        directories[i]=INT_MAX;
    }
    directories[start] = 0;
    vector<int> p((unsigned int) numVertexes, -1);
    int s = -1;
    isNegative = false;
    for (int i = 0; i < numVertexes; ++i) {
        s = -1;
        for (int j = 0; j < numEdges; ++j) {
            int u = graph[j].from;
            int v = graph[j].to;
            if (directories[u]<INT_MAX && directories[v]> directories[u]+graph[j].cost){
                int u1 = start<u?u-1:u;
                int v1 = start<v?v-1:v;

                vector<int>().swap(path[v1]);
                for (int k = 0; u!=start && k < path[u1].size(); ++k) {
                    path[v1].push_back(path[u1][k]);
                }
                path[v1].push_back(v);
                directories[v] = max(INT_MIN, directories[u]+graph[j].cost);
                p[v]=u;
                s = v;
            }
        }
    }
    if (s!=-1){
        isNegative = true;
        int t = s;
        for (int i = 0; i < numVertexes; ++i) {
            t = p[t];
        }
        vector<int> nPath;
        for (int i = t;; i = p[i]) {
            if (i == t && nPath.size()>1)
                break;
            nPath.push_back(i);
        }
        reverse(nPath.begin(), nPath.end());
        path = new vector<int>[1];
        path[0] = nPath;
        for (int i = 0; i < nPath.size(); ++i) {
            for (int j = 0; j < graph.size(); ++j) {
                if (graph[j].from == nPath[i] && graph[j].to == nPath[i+1])
                {
                    minCost+=graph[j].cost;
                    break;
                }
            }
        }
        for (int j = 0; j < graph.size(); ++j) {
            if (graph[j].from == nPath[nPath.size()-1] && graph[j].to == nPath[0])
            {
                minCost+=graph[j].cost;
                break;
            }
        }
    }
}

void BellmanFord::solvePositive() {
    for (int i = 0; i < numVertexes; ++i) {
        directories[i]=INT_MAX;
    }
    directories[start] = 0;
    for (int i = 0; i < numVertexes; ++i) {
        for (int j = 0; j < numEdges; ++j) {
            int u = graph[j].from;
            int v = graph[j].to;
            if (directories[u]<INT_MAX){
                if (directories[v] > directories[u] + graph[j].cost)
                {
                    int u1 = start<u?u-1:u;
                    int v1 = start<v?v-1:v;
                    vector<int>().swap(path[v1]);
                    for (int k = 0; u!=start && k < path[u1].size(); ++k) {
                        path[v1].push_back(path[u1][k]);
                    }
                    path[v1].push_back(v);
                    directories[v] = directories[u] + graph[j].cost;
                }
            }
        }
    }
}

int BellmanFord::getCost() {
    return minCost;
}

std::vector<int>* BellmanFord::getPath() {
    if (isNegative){
        vector<int>* p = new vector<int>[1];
        for (int i = 0; i < path[0].size(); ++i) {
            p[0].push_back(path[0][i]+1);
        }
        return p;
    }

    vector<int>* path_1 = new vector<int>[numVertexes-1];

    for (int i = 0; i < numVertexes-1; ++i) {
        path_1[i].push_back(start+1);
        for (int j = 0; j < path[i].size(); ++j) {
            path_1[i].push_back(path[i][j]+1);
        }
    }
    int* d1 = new int[numVertexes-1];
    for (int i = 0; i < numVertexes; ++i) {
        if (start<i){
            d1[i-1] = directories[i];
        } else if (start>i){
            d1[i]=directories[i];
        }
    }
    for (int i = 0; i < numVertexes - 1; ++i) {
        for (int j = i + 1; j < numVertexes; ++j) {
            if (path_1[i].size()>0 && path_1[j].size()>0 && d1[i] > d1[j]){
                path_1[i].swap(path_1[j]);
                swap(d1[i],d1[j]);
            }
        }
    }
    delete[] d1;
    return path_1;
}

BellmanFord::~BellmanFord() {
    delete[] directories;
    vector<Edge>().swap(graph);
    if (!isNegative){
        for (int i = 0; i < numVertexes-1; ++i) {
            vector<int>().swap(path[i]);
        }
    } else{
        vector<int>().swap(path[0]);
    }
    delete[] path;
}

vector<string> split(string s, char delimiter) {
    vector<string> line;
    stringstream ss(s);
    string token;

    while(getline(ss, token, delimiter)) {
        line.push_back(token);
    }

    return line;
}

int main() {
    ifstream input;
    input.open("input.txt");
    string line;
    vector<string> text;
    if(input.is_open()){
        text.clear();
        while (getline(input,line)){
            text.push_back(line);
        }
    }
    input.close();
    vector<string> t = split(text[0],' ');
    int n,m;
    if (t.size()!=2)
    {
        n = stoi(t[0]);
        m = stoi(split(text[1],' ')[0]);
    } else{
        n = stoi(t[0]);
        m=stoi(t[1]);
    }
    int** mas = new int*[m];
    if (t.size()==2){
        for (int i = 1; i < text.size(); ++i) {
            t = split(text[i],' ');
            mas[i-1] = new int[t.size()];
            for (int j = 0; j < t.size(); ++j) {
                mas[i-1][j]=stoi(t[j]);
            }
        }
    } else{
        for (int i = 2; i < text.size(); ++i) {
            t = split(text[i],' ');
            mas[i-2] = new int[t.size()];
            for (int j = 0; j < t.size(); ++j) {
                mas[i-2][j]=stoi(t[j]);
            }
        }
    }
    BellmanFord* bellmanFord = new BellmanFord();
    bellmanFord->initialize(mas,m,n,1);
    bellmanFord->solve();
    string s="";
    for (int k = 0; k < n; ++k) {
        if (directories[k]==INT_MAX)
            directories[k]=30000;
        s+=to_string(directories[k])+" ";
    }
    vector<int> *path_2 = bellmanFord->getPath();
   if (bellmanFord->getCost()==0){
        for (int i = 0; i < n-1; ++i) {
            cout<<i<<": ";
            for (int j = 0; j < path_2[i].size(); ++j) {
                cout<<path_2[i][j]<<" ";
            }
            cout<<endl;
        }
        std::cout<<bellmanFord->getCost();
    } else{
        for (int i = 0; i < path_2[0].size(); ++i) {
            std::cout<<path_2[0][i]<<" ";
        }
        cout<<"\n"<<bellmanFord->getCost()<<endl;
    }
    bellmanFord->~BellmanFord();

    ofstream output("output.txt");
    output<<s;
    output.close();
    for (int l = 0; l < m; ++l) {
        delete[] mas[l];
    }
    delete[] mas;
    //delete[] path_2;
    return 0;
}