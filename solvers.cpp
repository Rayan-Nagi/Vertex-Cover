#include <bits/stdc++.h>
#include <iostream>
#include <thread>
#include <sstream>
#include <fstream>
#include <string>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
#include <pthread.h>
#include <time.h>

using namespace std;

string v;
int vertices;
vector < vector<int>> adj;
vector<int> ijdet;


//Input Parsing
string input_parse(int vertices, string s)
{
	string delimiter1 = "}";
	string delimiter2 = "<";
	string delimiter3 = ">";
	string comma = ",";
	size_t pos = 0;
	size_t pos2 = 0;
	string token;
	int vint;

	if (s.find("E") == 0) {
		s.erase(0,3);
		pos2 = s.find(delimiter1);
		s.erase(pos2,1);
	}
	if(s=="")return"empty"; 
  	string scheck = s;
  	while ((pos = s.find(delimiter2)) != string::npos) {
    	s.erase(pos,1);
    	pos2 = s.find(delimiter3);
    	s.erase(pos2,1);
  	}

  	while ((pos = s.find(comma)) != string::npos) {
    	token = s.substr(0,pos);
    	vint = atoi(token.c_str());
    	if(vint >= vertices)
      	{
			scheck = " ";
			return scheck;
      	}
    	else
      	{
			s.erase(0,token.length()+1);
      	}
  	}

  	vint = atoi(s.c_str());
  	if(vint >= vertices)
    {
    	scheck = " ";
      	return scheck;
    }

  	return scheck;
}


///Function call
void* vertex_coversat(void* args)
{
	vector<int> ijdet1;
	ijdet1 = ijdet;

	vector<int> answer;

	int k,low,high;
	low = 0;
	high = vertices;
	int n = vertices;
	if(n>=20){
		cout<<"CNF-SAT-VC: timeout"<<endl;
		return NULL;
	}
  	while(low<=high)
    {
      	k = int (low + high )/ 2;
      	vector<vector<Minisat::Lit> > x(n);

      	Minisat::Solver solver;
      	for(int i = 0; i < n; i++)
        {
	  		for(int j = 0; j < k; j++)
            {
	      		Minisat::Lit li = Minisat::mkLit(solver.newVar());
	      		x[i].push_back(li);
            }
        }

      //At least one vertice is the ith vertex in the vertex cover
      	for(int i = 0; i < k; i++)
        {
	  		Minisat::vec<Minisat::Lit> temp;
	  		for(int j = 0; j < n; j++)
            {
	      		temp.push(x[j][i]);
            }
	  		solver.addClause( temp );
	  		temp.clear();
        }

      // No one vertex can  appear twice in vertex cover
      	for(int i=0;i< n;i++)
        {
	  		for(int j=0;j<k-1;j++)
            {
	      		for (int ab=j+1;ab<k;ab++)
            	{
		  			solver.addClause(~x[i][j],~x[i][ab]);
                }
            }
        }
 
      //No more than one vertex in two pos in v.cover
    	for(int i=0;i< k;i++)
    	{
	  		for(int j=0;j<n-1;j++)
            {
	      		for (int ab=j+1;ab<n;ab++)
                {
		  			solver.addClause(~x[j][i],~x[ab][i]);
                }
            }
        }
	
      //Every edge is incident on at least one vertex in vertex cover
      	int len_size = ijdet1.size();
      	for(int j=0;j<len_size;j+=2)
		{       
			int q = ijdet1[j];
	  		int rs = ijdet1[j+1];
                    
	  		Minisat::vec<Minisat::Lit> temp1;
	  		for(int i=0;i< k;i++)
	    	{
	      		temp1.push(x[q][i]);
	      		temp1.push(x[rs][i]);        
	    	}
	  		solver.addClause(temp1);
	  		temp1.clear();
		}

      // Check for solution and retrieve model if found
      	auto sat = solver.solve();
      	if (sat) {
			high = k-1;
			answer.clear();
			for(int i = 0; i < n; i++)
	  		{
	    		for(int j = 0; j < k; j++)
	      		{
					Minisat::lbool tf=solver.modelValue(x[i][j]);
					if(tf==Minisat::l_True)
		  			{   
		    			answer.push_back(i);
		  			}
	      		}
		  	}
		}
      	else
		{
	  		low = k+1;
        }
    }

  	int xl = answer.size();
           
  	for (int i = 0;i<xl;i++)
    {
    	if (i==0)
		{	 
			std::cout<<"CNF-SAT-VC: ";
		}
      	if(i == xl-1)
		{
	  		std::cout<<answer[i];
	  		break;
		}
      	std::cout<<answer[i]<<",";
    }
  	std::cout<<std::endl;
	// std:cout<<"CNF-SAT-VC size: "<<xl<<std::endl; 
  	answer.clear();


	clockid_t clk_id;
    struct timespec tspec;
    pthread_t tid;
	tid = pthread_self();

	// if (pthread_getcpuclockid( tid, &clk_id) == 0)
    // {
    //     if (clock_gettime( clk_id, &tspec ) != 0)
    //     {
    //         perror ("clock_gettime():");
    //     }
    //     else
    //     {
    //         printf ("CPU time for sat %d is %d seconds, %ld nanoseconds.\n",
    //                 tid, tspec.tv_sec, tspec.tv_nsec);
    //     }
    // }
    // else
    // {
    //     printf ("pthread_getcpuclockid(): no thread with ID %d.\n", tid);
    // }

  	return NULL;
}

void* vertex_approx(void* args)
{
	vector<int> ijdet2;
	vector< vector<int> > adj1;
	ijdet2 = ijdet;
	adj1 = adj;

	int maxv;
	int degree;
	maxv = ijdet2[0];
	degree = adj1[maxv].size();
	vector<int> vertexcover;
	int eras;

  	for(int i=0;i<vertices;i++)
    {
      	if (adj1[i].size()!=0)
		{
	  		degree = adj1[i].size();
	  		for(int j=0;j<vertices;j++)
	    	{
	      		if(adj1[j].size() >= degree )
				{	
		  			maxv = j;
		  			degree = adj1[j].size();
				}
	    	}
		
	  		vertexcover.push_back(maxv);
	  		for(int k=0;k<adj1[maxv].size();k++)
	    	{
	      		eras = adj1[maxv][k];
	      		adj1[eras].erase(remove(adj1[eras].begin(), adj1[eras].end(), maxv), adj1[eras].end());
	    	}
	  		adj1[maxv].clear();
	  		i = 0;
   		
		}
    }

	sort(vertexcover.begin(),vertexcover.end());
	int len = vertexcover.size();
			
	for (int i = 0;i<len;i++)
    {
      	if (i==0)
		{
	  		cout<<"APPROX-VC-1: ";
		}
      	if(i== len-1)
		{
			cout<<vertexcover[i];
			break;
		}
      	cout<<vertexcover[i]<<",";
    }
	cout<<endl; 
	// std:cout<<"CNF-SAT-VC 1 size: "<<vertexcover.size()<<std::endl; 

	vertexcover.clear();
	adj1.clear();
	ijdet2.clear();

	clockid_t clk_id;
    struct timespec tspec;
    pthread_t tid;
	tid = pthread_self();

	// if (pthread_getcpuclockid( tid, &clk_id) == 0)
    // {
    //     if (clock_gettime( clk_id, &tspec ) != 0)
    //     {
    //         perror ("clock_gettime():");
    //     }
    //     else
    //     {
    //         printf ("CPU time for sat 1 %d is %d seconds, %ld nanoseconds.\n",
    //                 tid, tspec.tv_sec, tspec.tv_nsec);
    //     }
    // }
    // else
    // {
    //     printf ("pthread_getcpuclockid(): no thread with ID %d.\n", tid);
    // }

  	return NULL;
}

void* vertex_approx2(void* args)
{

	vector<int> ijdet3;
	vector< vector<int> > adj2;
	ijdet3 = ijdet;
	adj2 = adj;

	vector<int> vertexcover1;
	int u1,u2;

	int randedge = rand()%(ijdet3.size());
	if(randedge%2!=0)randedge=randedge-1;
	u1 = ijdet3[randedge];
	u2 = ijdet3[randedge+1];
	if (adj2[u1].size()!=0 && adj2[u2].size()!=0)
	{
		vertexcover1.push_back(u1);
		vertexcover1.push_back(u2);
		adj2[u1].clear();
		adj2[u2].clear();
	}
	ijdet3.erase(ijdet3.begin() +randedge,ijdet3.begin() +randedge+2 );

	while(ijdet3.size()!=0){
		int randedge = rand()%(ijdet3.size());
		if(randedge%2!=0)randedge=randedge-1;
		u1 = ijdet3[randedge];
		u2 = ijdet3[randedge+1];
		if (adj2[u1].size()!=0 && adj2[u2].size()!=0)
		{
			vertexcover1.push_back(u1);
			vertexcover1.push_back(u2);
			adj2[u1].clear();
			adj2[u2].clear();	
		}
		ijdet3.erase(ijdet3.begin() +randedge,ijdet3.begin() +randedge+2 );
	}
	
	// for(int i = 0;i<ijdet3.size();i+=2)
    // {
	// 	u1 = ijdet3[i];
	// 	u2 = ijdet3[i+1];
	// 	if (adj2[u1].size()!=0 && adj2[u2].size()!=0)
	// 	{
	// 		vertexcover1.push_back(u1);
	// 		vertexcover1.push_back(u2);
	// 		adj2[u1].clear();
	// 		adj2[u2].clear();
	// 	}
    // }

	sort(vertexcover1.begin(),vertexcover1.end());
	int len = vertexcover1.size();
	for (int i = 0;i<len;i++)
    {
      	if (i==0)
		{
	  		std::cout<<"APPROX-VC-2: ";
		}
      	if(i == len-1)
		{
	  		std::cout<<vertexcover1[i];
	  		break;
		}
      	cout<<vertexcover1[i]<<",";
    }
	cout<<endl; 
	// std:cout<<"CNF-SAT-VC 2 size: "<<vertexcover1.size()<<std::endl; 
	vertexcover1.clear();

	adj2.clear();
	ijdet3.clear();
    

	clockid_t clk_id;
    struct timespec tspec;
    pthread_t tid;
	tid = pthread_self();

	// if (pthread_getcpuclockid( tid, &clk_id) == 0)
    // {
    //     if (clock_gettime( clk_id, &tspec ) != 0)
    //     {
    //         perror ("clock_gettime():");
    //     }
    //     else
    //     {
    //         printf ("CPU time for sat 2 %d is %d seconds, %ld nanoseconds.\n",
    //                 tid, tspec.tv_sec, tspec.tv_nsec);
    //     }
    // }
    // else
    // {
    //     printf ("pthread_getcpuclockid(): no thread with ID %d.\n", tid);
    // }

	return NULL;
}

void* io_handle(void* args)
{

	using Minisat::mkLit;
	using Minisat::lbool;

	string s;
	string scheck;

	//Parsing Operator Variables
	string vdelimiter = "V ";
	size_t vpos = 0;
	string spath;
	string delimiter2 = "<";
	string delimiter3 = ">";
	string comma = ",";
	size_t pos = 0;
	size_t pos2 = 0;
	size_t spos = 0;
	string token;
	int sr,des;
	ijdet.clear();
	adj.clear();
   	
  	//Prepare Adjacency Matrix
  	//Pick Vertice Number
  	if (v.find("V ") == 0)
  	{
      	v.erase(vpos,2);
     	vertices = atoi(v.c_str());
      	scheck.clear();
  	}
  	//Edge Input
  	if (v.find("E ") == 0)
    {   
      	s = v;
      	scheck = input_parse(vertices,s);
      	if(scheck=="empty" || vertices==0 || vertices==1){
			  cout<<"CNF-SAT-VC: "<<endl;
			  cout<<"APPROX-VC-1: "<<endl;
			  cout<<"APPROX-VC-2: "<<endl;
			  return NULL;
		}
      	if (scheck!= " ")
		{
			adj.resize(vertices);
	  		while ((pos = scheck.find(delimiter2)) != string::npos)
	    	{
	      		pos2 = scheck.find(delimiter3);
	      		token = scheck.substr(pos+1, pos2-1);
	      		spos = token.find(comma);
	      		sr = atoi(token.substr(0,spos).c_str());
	      		ijdet.push_back(sr);
	      		token.erase(0,spos+1);
	     		des = atoi(token.c_str());

	      		adj[sr].push_back(des);
	      		adj[des].push_back(sr);
	      		ijdet.push_back(des);
	      		scheck.erase(pos, pos2+2);
	    	}

			// std::cout<<vertices<<std::endl;
	    	
	  		pthread_t t2,t3,t4;
	  		pthread_create(&t2,NULL,&vertex_coversat,NULL);
	  		pthread_join(t2,NULL);
	  		pthread_create(&t3,NULL,&vertex_approx,NULL);
	  		pthread_join(t3,NULL);
	  		pthread_create(&t4,NULL,&vertex_approx2,NULL);
	  		pthread_join(t4,NULL);
			// std::cout<<std::endl;
		}
    	else cerr << "Error: Invalid Entry"<< endl;
    }
  	return NULL;
}


//Main Program
int main(int argc, char** argv)
{
      
	struct timespec currTime;
	int app1_id,app2_id;

  	while(std::getline(cin,v))
    {
    	pthread_t t1;
      	pthread_create(&t1,NULL,&io_handle,NULL);
      	pthread_join(t1,NULL);		 
    }

  	return 0;
}
