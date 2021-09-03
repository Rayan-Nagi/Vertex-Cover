#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <climits>
#include <list>
#include <getopt.h>
#include <unistd.h>
using namespace std;

bool parseLine(const std::string &line, char &cmd, int &vert, int &src, int &dst,vector<int> &points ) {

  istringstream input(line);
  ws(input);
  input >> cmd;

  if ( cmd == 'V' ) {
    input >> vert;
    return true;
  }

   else if (cmd == 'E'){
     string name;
     int x;
     int n;
     bool flag = false;
     
     name = line;
     name.erase(0,3);
     x = name.find('}');
     name.erase(x,1);

     if (name.size() == 0 || name.size() == 1){
       points = {};
       return true;
     }

     else {
       do {
	 x = name.find('<');
	 name.erase(x,1);
       } while (name.find ('<') != -1);
      
       do {
	 x = name.find('>');
	 name.erase(x,1);
       } while (name.find ('>') != -1);

       do {
	 x = name.find(',');
	 if (x != -1){
	   name[x]=' ';
	 }
       } while (name.find (',') != -1);  
  
       stringstream E(name);

       while (E >> n){
	 points.push_back(n);
       }
     
       for (int j=0; j<points.size()-1; j=j+2){
	 if (points[j]>=vert || points[j+1]>= vert || points[j]==points[j+1]){
	   flag = true;
	   break;
	 }
       }
       if (flag) {
	 cerr << "Error: Wrong Input for E" << endl;
	 return false;
       }
       return true;
     }
  }

  else if (cmd == 'S'|| cmd == 's') {

    input >> src;
    input >> dst;

    if (src >= vert || dst >= vert || src <0 || dst <0){
      cerr << "Error: Wrong Input for S" << endl;
      return false;
    }
    return true;
  }

  else {
    std::cerr << "Error: bad command!\n";
    return false;
  }

}


bool BFS(vector<vector<int>> adj, int src, int dest, int v, int pred[], int dist[]) {
  list<int> queue;
  bool visited[v];

  for (int i = 0; i < v; i++) {
    visited[i] = false;
    dist[i] = INT_MAX;
    pred[i] = -1;
  }

  visited[src] = true;
  dist[src] = 0;
  queue.push_back(src);

  while (!queue.empty()) {
    int u = queue.front();
    queue.pop_front();
    for (int i = 0; i < adj[u].size(); i++) {
      if (visited[adj[u][i]] == false) {
	visited[adj[u][i]] = true;
	dist[adj[u][i]] = dist[u] + 1;
	pred[adj[u][i]] = u;
	queue.push_back(adj[u][i]);

	if (adj[u][i] == dest)
	  return true;
      }
    }
  }
  return false;
}

vector<int> shortestPath(vector<vector<int>> adj, int s, int dest, int v) {
  vector<int> path;
  int pred[v], dist[v];

  if (BFS(adj, s, dest, v, pred, dist) == false)	{
    return path;
  }
  int crawl = dest;
  path.push_back(crawl);
  while (pred[crawl] != -1) {
    path.push_back(pred[crawl]);
    crawl = pred[crawl];
  }
  return path;
}


int main() {
 
  string line;
  char cmd;
  int vert;
  int src;
  int dst;
  vector<int> points;
  vector<int> path;
  vector<vector<int>> adj;
  int v1;
  int v2;
  bool sflag;

  while (true) {

    getline(cin, line);

    if (cin.eof()) {
      exit(0);
    }
    
    if (parseLine(line, cmd, vert, src, dst, points)) {

      if ( cmd == 'V' ) {
	adj.clear();
	path.clear();
	points.clear();
	cout << line
	     << endl;
	sflag = true;
      }
      
      else if (cmd == 'E'){
	adj.resize(vert);
	sflag = false;
	int tt = line.size()-2;
	if (line[tt] == ','){
	  line.erase(tt,1);
	    }
	cout << line << endl;
	if (points.size() != 0){
	  for(int i=0; i<points.size()-1; i=i+2){
	    v1=points[i];
	    v2=points[i+1];
	    adj[v1].push_back(v2);
	    adj[v2].push_back(v1);
	  }
	}
      }
      
      else if (cmd == 'S' || cmd == 's') {
	if (sflag) {
	  cerr << "Error: Edges Are Not Valid Can not Calculate Shortest Path" << endl;
	}
	
	else if (src == dst){
	  cout << src << endl;
	}

	else if (points.size()==0){
	  cerr << "Error:Path Doesn't Exist" << endl;
	}
    
	else {
	  path = shortestPath (adj, src, dst, vert);	    
	  if (path.size() != 0) {
	    for (int i = path.size() - 1; i >= 0; i--) {
	      if (i == path.size()-1)
		cout << path[i];
	      else cout << "-" << path[i];
	    }
	    cout << endl;
	  }
	  
	  else cerr << "Error: Path Does Not Exist"<< endl;
	}
      }
    }
  }   

}
