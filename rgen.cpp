#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;


// To be able to write the below code, I have refered to:
//https://github.com/ganesh292/Optimized-Traffic-Camera-Placement/blob/master/Project/rgen.cpp
// I have just made small modifications, I have mostly used the online code

struct point
{
  int x;
  int y;
};

struct path
{
  point src;
  point dst;
};

string cvalue;
int cint_value;
int nstreet=2;
int nlseg=3;
int waitsec=5;
int c=20;
int co;
int ovtry=0;
vector<string> streetName;
vector<point> newseg;
vector<point> streetseg;


void check_urand();
bool inter_same_st(vector<point> streetseg,int nlseg);
void rgen_gatherip(int argc, char **argv);
vector<point> add_newseg(int c,vector<point> newseg,int ovtry);
bool check_overlap(vector<point> streetseg, vector<point> newseg);
void send_stdout(vector<point> streetseg,int nlseg,vector<string> streetName);
bool doIntersect(point p1, point q1, point p2, point q2);
bool onSegment(point p, point q, point r);
int orientation(point p, point q, point r);


ifstream urandom("/dev/urandom");


int main (int argc, char *argv[]) {

  check_urand();
  rgen_gatherip(argc,argv);

  while(true)
    {

      //Check if Street Vector has Values if so pass r to remove everything
      if (streetName.size()!= 0)
	{
	  for(int i=0;i<streetName.size();i++)
	    {
	      cout << "r" << " " << streetName[i] << endl;
	      cout << flush;
	    }
	}

      //Clear OldStreet Names before starting
      streetName.clear();

      //Street Names Generation and stored in Vector
      int tempst = 0;
      string sname;
      char lowercase = 97;
      while(tempst< nstreet)
	{
	  for(int i=0;i<5;i++)
	    {
	      unsigned int num = 42;
	      urandom.read((char*)&num, sizeof(int));
	      num %= 26;
	      lowercase+=num;
	      sname+=lowercase;
	      lowercase = 97;
	    }

	  if(find(streetName.begin(), streetName.end(), sname) == streetName.end()) {
	    sname = "\"" + sname + "\"";
	    streetName.push_back(sname);
	    tempst = tempst + 1;
	    sname.clear();

	  } else {
	    sname.clear();
	    tempst = tempst-1;
	  }


	}

      streetseg = add_newseg(c,streetseg,ovtry);
      bool overlap_ck,overlap_same;
      int loop = streetName.size();
      loop = (loop * nlseg)+ loop;


      while(streetseg.size()<loop)

	{
	  int size = streetseg.size();
	  if ( size % (nlseg+1) == 0)
	    {
	      overlap_same = inter_same_st(streetseg,nlseg);
	      if (overlap_same == true)
		{
		  ovtry = ovtry + 1;
		  if (ovtry >= 25)
		    {
		      cerr<< "Error: Failed to generate correct streets for more than 25 attempts"
			  << endl;
		      exit(1);
		    }

		}

	    }

	  newseg = add_newseg(c,newseg,ovtry);
	  overlap_ck = check_overlap(newseg,streetseg);
	  while(overlap_ck == true)
	    {
	      ovtry = ovtry + 1;
	      if (ovtry >= 25)
		{
		  cerr<< "Error: Failed to generate correct streets for more than 25 attempts"
		      << endl;
		  exit(1);
		}
	      newseg.clear();
	      newseg = add_newseg(c,newseg,ovtry);
	      overlap_ck = check_overlap(newseg,streetseg);

	    }

	  streetseg.push_back(newseg[0]);
	  streetseg.push_back(newseg[1]);
	  newseg.clear();

	}

      send_stdout(streetseg,nlseg,streetName);
      streetseg.clear();
      usleep(waitsec*1000000);

    }

  return 0;
}


//Urandom File Check
void check_urand()
{

  // check that it did not fail
  if (urandom.fail()) {
    cerr << "Error: cannot open /dev/urandom\n";
    exit(1);
  }
}


//Rgen Input picking for s,n.l,c
void rgen_gatherip(int argc,char* argv[])
{
  // expected options are '-s','-n','l',-cvalue'
  while ((co = getopt (argc, argv, "s:n:l:c:")) != -1)
    switch (co)
      {
      case 's':
	cvalue = optarg;
	nstreet = atoi(cvalue.c_str());
	nstreet = ((nstreet + 2) / 2);
	if (nstreet < 2 or optarg[0] == '-')
	  {
	    nstreet = 2;
	  }
	break;
      case 'n':
	cvalue = optarg;
	nlseg = atoi(cvalue.c_str());
	nlseg = 1 + (nlseg / 2);
	if (nstreet < 1 or optarg[0] == '-')
	  {
	    nlseg = 3;
	  }
	break;
      case 'l':
	cvalue = optarg;
	waitsec = atoi(cvalue.c_str());
	waitsec = (waitsec + 5) / 2;
	if (nstreet < 5 or optarg[0] == '-')
	  {
	    waitsec = 5;
	  }
	break;
      case 'c':
	cvalue = optarg;
	c = atoi(cvalue.c_str());
	c = c * 1 ;
	if (nstreet < 1 or optarg[0] == '-')
	  {
	    c = 20;
	  }
	break;
      default:
	break;
      }

}


vector<point> add_newseg(int c,vector<point> newseg,int ovtry)

{
  if (c == -1)
    {
      c = 20;
    }


  point sp,np;
  int numc = 1;
  int flag = 0;
  urandom.read((char*)&numc, sizeof(int));
  numc %= c+1;
  sp.x = int(numc);
  urandom.read((char*)&numc, sizeof(int));
  numc %= c+1;
  sp.y = int(numc);
  newseg.push_back(sp);
  while(flag ==0)
    {
      numc = 21;
      ovtry = ovtry + 1;

      if (ovtry > 25)
	{
	  cerr<< "Error: Failed to generate correct streets for more than 25 attempts"
	      << endl;
	  exit(1);
	}
      urandom.read((char*)&numc, sizeof(int));
      numc %= c+1;
      np.x = int(numc);
      urandom.read((char*)&numc, sizeof(int));
      numc %= c+1;
      np.y = int(numc);
      newseg.push_back(np);
      if (np.x == sp.x && sp.y == np.y)
	{
	  continue;
	}

      flag = 1;


    }

  return newseg;

}


bool check_overlap(vector<point> newseg,vector<point> streetseg)
{

  for(int i=0;i<(streetseg.size()-1);i++)
    {
      int x1,x2,x3,x4,y1,y2,y3,y4;
      float slope1,slope2,intercept1,intercept2;

      x1 = streetseg[i].x;
      y1 = streetseg[i].y;
      x2 = streetseg[i+1].x;
      y2 = streetseg[i+1].y;
      x3 = newseg[0].x;
      y3 = newseg[0].y;
      x4 = newseg[1].x;
      y4 = newseg[1].y;

      slope1 = float (y2 - y1) / float (x2 - x1);
      slope2 = float (y4-y3) / float (x4-x3);
      if (slope1 == slope2)
	{
	  intercept1 = float (y1) - float (slope1 * x1);
	  intercept2 = float (y3) - float (slope2 * x3);
	  if 	(intercept1 == intercept2)
	    {
	      if ( min(x1,x2) <= min(x3,x4) && min(y1,y2) <= min(y3,y4) && max(x1,x2) >= max(x3,x4) && max(y1,y2) >= max(y3,y4))
		{
		  return true;
		}
	      if ( min(x3,x4) <= min(x1,x2) && min(y3,y4) <= min(y1,y2) && max(x3,x4) >= max(x1,x2) && max(y3,y4) >= max(y1,y2))
		{
		  return true;
		}
	    }
	}


    }
  return false;
}


void send_stdout(vector<point> streetseg,int nlseg,vector<string> streetName)
{
  string coord,coordj,input;
  int count = 1;
  int multiplier=1;
  int j = 0;
  for (int i=0;i<streetName.size();i++)
    {
      while(j < ((nlseg+1)*multiplier))
	{
	  coord = '(' + to_string(streetseg[j].x) + ',' + to_string(streetseg[j].y) + ')';
	  coordj += ' ' + coord + ' ';
	  input = "a " + streetName[i] + coordj;
	  j = j + 1;
	}
      multiplier = multiplier + 1;
      cout << input<< endl;
      cout << flush;
      coord.clear();
      coordj.clear();
      input.clear();

    }
  cout << "g"<< endl;
  cout << flush;

}



bool inter_same_st(vector<point> streetseg,int nlseg)

{
  bool oput;
  point p1,p2,q1,q2;
  for(int i=(streetseg.size()-nlseg-1);i<(streetseg.size()-1);i++)
    {
      p1 = streetseg[i];
      q1 = streetseg[i+1];
      for(int j=i+1;j< (streetseg.size()-1);j++)
	{

	  p2 = streetseg[j];
	  q2 = streetseg[j+1];
	  oput = doIntersect(p1,q1,p2,q2);
	  if(oput == true)
	    { 
	      streetseg.erase(streetseg.begin()+(streetseg.size()-nlseg-1), streetseg.end());
	      return true;
	    }

	}}

  return false;
}




// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(point p, point q, point r)
{
  if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
      q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
    return true;

  return false;
}



int orientation(point p, point q, point r)
{
 
  int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

  if (val == 0) return 0;  // colinear

  return (val > 0)? 1: 2; // clock or counterclock wise
}



bool doIntersect(point p1, point q1, point p2, point q2)
{

  if ( (q1.x==p2.x) && (q1.y == p2.y) )
    {
      int o2 = orientation(p1, q1, q2);

      if (o2==0)   // p2, q2 and p1 are colinear and p1 lies on segment p2q2
	{ 	if ((onSegment(p2, p1, q2)==true) or (onSegment(p1, q2, q1) == true ))
	    return true;
	}}
  else
    {
      int o1 = orientation(p1, q1, p2);
      int o2 = orientation(p1, q1, q2);
      int o3 = orientation(p2, q2, p1);
      int o4 = orientation(p2, q2, q1);

      if (o1!=o2 && o3!=o4)
	{
	  return true;
	}
      // Special Cases
      // p1, q1 and p2 are colinear and p2 lies on segment p1q1
      if (o1 == 0 && onSegment(p1, p2, q1)) return true;

      // p1, q1 and q2 are colinear and q2 lies on segment p1q1
      if (o2 == 0 && onSegment(p1, q2, q1)) return true;

      // p2, q2 and p1 are colinear and p1 lies on segment p2q2
      if (o3 == 0 && onSegment(p2, p1, q2)) return true;

      // p2, q2 and q1 are colinear and q1 lies on segment p2q2
      if (o4 == 0 && onSegment(p2, q1, q2)) return true;
    }


  return false; // Doesn't fall in any of the above cases
}
