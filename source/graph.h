
#ifndef GDBD_GRAPH_H
#define GDBD_GRAPH_H


#include <iostream>
#include <map>
#include <string>
#include <list>
#include <deque>

using namespace std;

template <class T>
class Graph {
public:

    T root;
    int level;

    map<T, bool> visited;
    map<T, list<T> > adj;

    void addEdge(T v, T w);

    void auxDFS(T v,int,int,string , deque<T>& );
    void DFS(T ,int , deque<T>&);
    void print();

    void clear(){
        visited.clear();
        adj.clear();
    }

};

template <class T>
void Graph<T>::addEdge(T v, T w)
{
    adj[v].push_back(w);
}

template <class T>
void Graph<T>::auxDFS(T v ,int i , int level , string path , deque<T>& res)
{

    visited[v] = true;
    path += " -> " + v ;

    if(level == i ){
//        path += "\n";
        res.push_back(path);
        return ;
    }


    for (auto iter = adj[v].begin(); iter != adj[v].end(); ++iter){
        if (!visited[*iter] ){
            auxDFS(*iter,i+1,level,path,res);
        }
    }

}

template <class T>
void Graph<T>::DFS(T _root ,int level , deque<T> & paths )
{

    for (int i = 1; i <= level ; ++i) {
        auxDFS(_root,0,i,"",paths );
        visited.clear();
    }
}


template <class T>
void Graph<T>::print()
{
    for (auto i = adj.begin() ; i != adj.end() ; ++i) {
        cout<<i->first<<" |\t";
        for (auto j = adj[i->first].begin() ; j != adj[i->first].end() ; ++j) {
            cout<<" -> "<<*j;
        }
        cout<<"\n";
    }
}




#endif //GDBD_GRAPH_H
