#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <random>
#include <vector>
#include <string>
#include <sys/wait.h>
#include <math.h>
#include <signal.h>
#include <algorithm>


unsigned int RandomGenerator()
{

    std::ifstream urandom("/dev/urandom");

    if (urandom.fail()) {
        std::cerr << "Error: cannot open /dev/urandom\n";
        return 1;
    }
    
    // read a random unsigned int
    unsigned int number;
    urandom.read((char*)&number, sizeof(int));
    
    urandom.close();
    return number;
}

struct Point 
{
   int x, y; //represents cartesian co-ordinate
};


bool onLine(Point p, Point q, Point r) 
{ 
    return (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && 
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y));
       
}

int orientation(Point p1, Point p2, Point p3) 
{ 
    int value = (p2.y - p1.y) * (p3.x - p2.x) - 
              (p2.x - p1.x) * (p3.y - p2.y); 
  
    if (value == 0) 
	{
		return 0;  // colinear 
    }
    else if(value>0)
    {
    	return 1; //clock wise
	}
	return 2;// counterclock wise 
	 

}

bool SegmentOverlap(Point a, Point b, Point c, Point d)
{
	int ori1, ori2, ori3, ori4;

    ori1 = orientation(a, b, c); 
    ori2 = orientation(a, b, d); 
    ori3 = orientation(c, d, a); 
    ori4 = orientation(c, d, b);

    std::vector<int> first , second;

    if (a.x == c.x && c.x == b.x && b.x == d.x) 
	{
        first.push_back(a.y);
        first.push_back(b.y);
        second.push_back(c.y);
        second.push_back(d.y);

    }
	else 
	{
        first.push_back(a.x);
        first.push_back(b.x);
        second.push_back(c.x);
        second.push_back(d.x);
    }
    
    sort(second.begin(), second.end());
    sort(first.begin(), first.end());
    

    if (ori1 == 0 && ori2 == 0 && ori3 == 0 && ori4 == 0) 
	{
        if ((first[0] >= second[1] && second[0] <= first[0])) 
            return false; // no overlap
        if ((second[0] >= first[1] && first[0] <= second[0]))
            return false;
        return true; // overlap
    }
    return false;
}

bool doIntersect(Point p1, Point q1, Point p2, Point q2) 
{ 
	int ori1, ori2, ori3, ori4;

    ori1 = orientation(p1, q1, p2); 
    ori2 = orientation(p1, q1, q2); 
    ori3 = orientation(p2, q2, p1); 
    ori4 = orientation(p2, q2, q1); 

    if (ori1 != ori2 && ori3 != ori4) 
        return true; 
    
    if((ori1 == 0 && onLine(p1, p2, q1)) ||(ori2 == 0 && onLine(p1, q2, q1)) ||(ori3 == 0 && onLine(p2, p1, q2))||(ori4 == 0 && onLine(p2, q1, q2)) )
    	return true;
  
    return false; // Dont intersect
}


bool Overlap(std::vector<int> x, std::vector<int> y, std::vector< std::vector<int> > all_streets_x, std::vector< std::vector<int> > all_streets_y) {
	
	// check if any line-segment overlaps with another line-segment (across all streets)
	
    for (unsigned int i = 1; i<x.size(); i++) 
	{

         struct Point a = {x[i], y[i]};
		 struct Point b = {x[i-1], y[i-1]};

        for (unsigned int k = 0; k < all_streets_x.size(); k++) 
		{

            for (unsigned j = 1; j < all_streets_x[k].size(); j++) 
			{
                struct Point c = {all_streets_x[k][j], all_streets_y[k][j]};
				struct Point d = {all_streets_x[k][j-1], all_streets_y[k][j-1]};
				
                if (SegmentOverlap(a,b,c,d)==true)
                    return true; //There is  an overlap, line segment is invalid,  return true
            }
        }
    }
    return false;// //There is  no overlap, return true

}

bool Nozerolength(std::vector<int> x, std::vector<int> y)
{
	//two adjacent points should not be same 
	for(unsigned int i = 0;i<x.size()-1;i++)
	{
		if((x[i]==x[i+1])&&(y[i]==y[i+1]))
		return false;
	}
	return true;
}

bool NoIntersection(std::vector<int> x, std::vector<int> y)
{
	
	struct Point a, b, c, d;
	
    for (unsigned int i=0;i<x.size()-1;i++) {
        
        a = {x[i], y[i]};
        b = {x[i+1], y[i+1]}; 
        
        for (unsigned int j=0;j<i;j++){
            c = {x[j], y[j]};
            d = {x[j+1], y[j+1]}; 
    
            if (!SegmentOverlap(a, b, c, d) && doIntersect(a, b, c, d) )
			 { 
                if (d.x != a.x && d.y != a.y) // d not equal to a case.
                    return false;    
            }
        }
    }
    return true;
}


std::vector< std::vector<int> > generate_coordinates(unsigned n,int cmax,std::vector< std::vector<int> > all_streets_x, std::vector< std::vector<int> > all_streets_y)
{
	int cmin = -1*cmax;
	std::vector< std::vector<int> > street_points;
	unsigned int c;
	int a;
	for(int t = 0;t<25;t++)
	{
		std::vector<int> x;
		std::vector<int> y;
		
		for(unsigned int j =0;j<=n;j++)
		{
	    	c = RandomGenerator();
	  		a = (c % (cmax + 1 - cmin) + cmin);
	  		x.push_back(a);
	  		
	  		c = RandomGenerator();
	        a = (c % (cmax + 1 - cmin) + cmin);
	        y.push_back(a);  
		}
	
		if(Nozerolength(x,y)&&NoIntersection(x,y)&&(!Overlap(x,y, all_streets_x, all_streets_y)))
		{
			street_points.push_back(x);
			street_points.push_back(y);
			return street_points;
		}
	
	}
	
	std::cerr << "Error: failed to generate valid input even after 25 simultaneous attempts" << std::endl;
	return street_points;
}

int main (int argc, char **argv)
{
	int smin = 2;
	int nmin = 1;
	int lmin = 5;
	std::string smax,nmax,lmax,cmax;
	int smax_value = 10,nmax_value = 5,lmax_value = 5,cmax_value = 20;
    int u;
    opterr = 0;
    while ((u = getopt (argc, argv, "s:n:l:c:")) != -1){
	
        switch (u)
        {
        case 's':
            smax = optarg;
            smax_value = atoi(smax.c_str());
            break;
        case 'n':
            nmax = optarg;
            nmax_value = atoi(nmax.c_str());
            break;
        case 'l':
            lmax = optarg;
            lmax_value = atoi(lmax.c_str());
            break;
        case 'c' :
        	cmax = optarg;
            cmax_value = atoi(cmax.c_str());
        	//cmin = -1 * cmax_value ; 
			break;
        case '?':
            if (optopt == 'c')
                std::cerr << "Error: option -" << optopt
                          << " requires an argument." << std::endl;
            else
                std::cerr << "Error: unknown option: " << optopt << std::endl;
            return 1;
        
            default:
            	return 0;
        }
    }
        

        for (; optind< argc; optind++){
		
            std::cout << "Extra-option argument: " << argv[optind] << "\n";
        }
    
		
		unsigned int l = RandomGenerator();
		l = (l % (lmax_value + 1 - lmin) + lmin);
			
		
    
    while(true)
	{
		unsigned int s = RandomGenerator();
        s = (s%(smax_value + 1 - smin) + smin) ;


		std::vector<std::string> StreetNames;
		std::vector< std::vector<int> > all_street_x;
		std::vector< std::vector<int> > all_street_y;
		
		
	    //no.of streets
		for(unsigned int i=0; i < s ; i++)
		{
			std::string name = "street";
			name = name+std::to_string(i+1);
			
			std::vector< std::vector<int> > street_points;
			
		    unsigned int n = RandomGenerator();
		    n = (n % (nmax_value + 1 - nmin) + nmin);
		    
		    street_points = generate_coordinates(n,cmax_value,all_street_x,all_street_x);
		    
		    if(street_points.size()>0)
		    {
			
		    StreetNames.push_back(name);
			all_street_x.push_back(street_points[0]);
			all_street_y.push_back(street_points[1]);
			std::string points= "";
			
		    for(unsigned int j =0;j<=n;j++)
            {
		    	points = points +" (";
		    	points = points + std::to_string(street_points[0][j]);
		    	points = points+",";
		    	points = points + std::to_string(street_points[1][j]);
		    	points = points + ")";
		    	
		    }
			
			std::cout<<"a \""<<name<<"\""<<points<<std::endl;   
		}
		}

		std::cout<<"g"<<std::endl;
		for(unsigned int i=0 ; i<StreetNames.size() ; i++)
		{
			std::cout<<"r \""<< StreetNames[i]<<"\""<<std::endl;
		}
    	sleep(l);
    	
	}
	return 0;

}