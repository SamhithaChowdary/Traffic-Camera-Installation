#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <iterator>
#include <list>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <minisat/core/Solver.h>


int countVec = 0;
int s;

clockid_t start_time1, start_time2, start_time3;
clockid_t end_time1, end_time2, end_time3;
timespec s_timespec1, s_timespec2, s_timespec3;

std::vector<long> timeCNF, timeVC1, timeVC2;
std::vector<float> ratioCNF, ratioVC1, ratioVC2;

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

timespec pclock(clockid_t cid)
{
    struct timespec ts;
    if (clock_gettime(cid, &ts) == -1)
    {
        handle_error("clock_gettime");
    }
    return ts;
}

std::vector<std::string> split(std::string input_str){
	std::vector<std::string> ret;
	std::string str = "";
	int  N  = input_str.size();
	for(int i = 0; i<N ; i++ ){
		if(input_str[i] == ' '){
			ret.push_back(str);
			str = "";
		}
		else{
			str = str + input_str[i];
		}
		
	}

	if(str.size() > 0){
		ret.push_back(str);
	}
	return ret;
	
	
}

std::vector<int> remove(std::string edges)
{
	std::vector<int> edgeVertices;
	int n,N = edges.size();
	std::string num =  "";
	for(int i=0;i<N;i++)
	{
		if(edges[i]=='{'|| edges[i]=='<'|| edges[i]==','|| edges[i]=='>'|| edges[i]=='}')
		{
			if(num.size()>0)
			{
				n = std::stoi(num);
				edgeVertices.push_back(n);
				num ="";
			}
			
		}
		else 
		num = num+edges[i];
	}
	return edgeVertices;
	
}


long std_long(std::vector<long> data, long mean)
{
    long  standardDeviation = 0;

    for(unsigned i = 0; i < data.size(); i++)
    {
        standardDeviation += pow(data[i] - mean, 2);
    }

    return sqrt(standardDeviation / data.size());
}

float std_float(std::vector<float> data, float mean)
{
    float standardDeviation = 0;

    for(unsigned i = 0; i < data.size(); i++)
    {
        standardDeviation += pow(data[i] - mean, 2);
    }

    return sqrt(standardDeviation / data.size());
}

class UDGraph
{
	
	public:
		int V;
		std::list<int> *adj;
		int VEmpFlag = 0,EEmpFlag = 0;
		std::vector<int> vertex;

		std::vector<int> CNFresult;
    	std::vector<int> VC1result;
    	std::vector<int> VC2result;

		
		void init(int V);
		void addedge(int a , int b);
		bool Buildgraph(std::vector<int> edgeVertices , int num);
		void approx1();
		void approx2();
		void CNFVertexCover();
		void printResult();
    	bool isEmpty();		
			
};


void UDGraph::init(int V) 
{
    adj = new std::list<int> [V];
}

// UDGraph::UDGraph(int V){
// 	this->V = V;
// 	adj = new list<int>[V];
// } 

void UDGraph::addedge(int a , int b){
	adj[a].push_back(b);
	adj[b].push_back(a);
}
bool UDGraph::isEmpty()
{
     if(this->EEmpFlag && this->VEmpFlag)
        return false;
    return true;  
}


bool UDGraph::Buildgraph(std::vector<int> edgeVertices , int num)
{
	int N =edgeVertices.size();
	int f,s;
	this->vertex = edgeVertices;
	
	for(int i=0;i<N;i = i+2)
	{
	
		f = edgeVertices[i];
		s = edgeVertices[i+1]; 
		if(f>=num||s>=num)
		{
			std::cerr << "Error: vertex ID is larger than the size of graph" << std::endl; 
			return false;
		}
		if(f == s)
		{
			std::cerr<<"Error: Edge has same vertex "<<std::endl;
			return false;
		}
		
		
		addedge(f,s); 
				
	}
	return true;

}

struct graph_data
{
    UDGraph graph;
};


std::vector<int> vertexcover(int n,int k,std::vector<int> first, std::vector<int> second)
{
	std::vector<int> answer;
	
	Minisat::Solver solver;
    std::vector <std::vector<Minisat::Lit>> atomic_props(n);

	Minisat::Lit literal;
    for (int i = 0; i < n; i++) 
	{
        for (int j = 0; j < k; j++) 
		{
            literal = Minisat::mkLit(solver.newVar());
            atomic_props[i].push_back(literal);
        }
    }
    
    Minisat::vec<Minisat::Lit> clause;
    for (int j = 0; j < k; j++)
	 {
        for (int i = 0; i < n; i++) 
		{ 
            clause.push(atomic_props[i][j]);
        }
        solver.addClause(clause);
        clause.clear();
    }
    
    
    for (int i = 0; i < n; i++)
	 {
        for (int p = 0; p < k - 1; p++)
		 {
            for (int q = p + 1; q < k; q++) 
			{
                solver.addClause(~atomic_props[i][p], ~atomic_props[i][q]);
            }
        }
    }

    for (int j = 0; j < k; j++)
	 {
        for (int p = 0; p < n - 1; p++)
		 {
            for (int q = p + 1; q < n; q++)
			{
                solver.addClause(~atomic_props[p][j], ~atomic_props[q][j]);
            }
        }
    }
    
    clause.clear();
    for (unsigned int i = 0; i < first.size(); ++i) 
	{
        for (int j = 0; j < k; j++)
		{
            clause.push(atomic_props[first[i]][j]);
        }
        
        for (int j = 0; j < k; j++)
		 {
            clause.push(atomic_props[second[i]][j]);
        }
        solver.addClause(clause);
        clause.clear();
    }

    auto result = solver.solve();
    
    if(result)
    {
    	
    	 for (int i = 0; i < n; i++)
		  {
            for (int j = 0; j < k; j++)
			 {
                if (Minisat::toInt(solver.modelValue(atomic_props[i][j])) == 0)
                    answer.push_back(i);
            }
        }
        sort(answer.begin(),answer.end());
	}
    
    return answer;
	
}

void UDGraph::CNFVertexCover()
{
	std::vector<int> edgeVertices = vertex;
	int n = this->V;
	int N =edgeVertices.size();
	std::vector<int> first;
	std::vector<int> second;
	int f,s;
	
	for(int i=0;i<N;i = i+2)
	{
	
		f = edgeVertices[i];
		s = edgeVertices[i+1];
		first.push_back(f);
		second.push_back(s);				
	}
	
	int lo =1,hi = n,mid;
	std::vector<int> answer;
	std::vector<int> temp;
	
	while(lo<=hi)
	{
		mid = (lo+hi)/2;
		temp = vertexcover(n,mid,first,second);
		if(temp.size()!=0)
		{
			answer = temp;
			hi = mid-1;
		}
		else
		lo = mid +1;
	}
	for(unsigned int  i= 0;i<answer.size();i++)
	{
		this->CNFresult.push_back(answer[i]);
	}

	countVec++;
	
	
}

int highestDegreeVertex(std::vector<int> vertices, int N)
{
	std::vector<int> countArray(N,0);
	
	int max = 0;
	int ans;
	
	for(unsigned int i= 0;i<vertices.size();i++)
	{
		int v = vertices[i];
		countArray[v]++;
		if(countArray[v]>max)
		{
			max = countArray[v];
			ans  = v;
		}
		
	}
	return ans;
	
}

void UDGraph::approx1()
{	
	std::vector<int> edges = vertex;
	int v;
	int N = this->V;
	std::vector<int> edgedup;

	
	while(edges.size()!=0)
	{
		
		v = highestDegreeVertex(edges,N); 
		this->VC1result.push_back(v);
		//ans.push_back(v);
		for (unsigned int i =0;i<edges.size()-1;i = i+2)
		{
			if((edges[i]!=v)&&(edges[i+1]!=v))
			{
				edgedup.push_back(edges[i]);
				edgedup.push_back(edges[i+1]);
			}	
		}
		
		edges = edgedup;
		edgedup.clear();
	}
	sort(this->VC1result.begin(), this->VC1result.end());
	countVec++;
	
}


void UDGraph::approx2()
{	std::vector<int> edges = vertex;
	std::vector<int> edgedup;

	
	while(edges.size()!=0)
	{
		int f = edges[0];
		int s = edges[1];
		//ans.push_back(f);
		//ans.push_back(s);
		this->VC2result.push_back(f);
		this->VC2result.push_back(s);
		for (unsigned int i =2;i<edges.size()-1;i = i+2)
		{
			if((edges[i]!=f)&&(edges[i]!=s)&&(edges[i+1]!=f)&&(edges[i+1]!=s))
			{
				edgedup.push_back(edges[i]);
				edgedup.push_back(edges[i+1]);
			}	
		}
		edges = edgedup;
		edgedup.clear();
		
	}
	sort(this->VC2result.begin(), this->VC2result.end());
	countVec++;

 	
}
void UDGraph::printResult() {
    std::string resCNF = "CNF-SAT-VC: ";
    std::string resVC1 = "APPROX-VC-1: ";
    std::string resVC2 = "APPROX-VC-2: ";

    for (unsigned i = 0; i < this->CNFresult.size(); i++)
    {
    	resCNF.append(std::to_string(this->CNFresult[i]));
    	if (i != this->CNFresult.size() - 1)
    		resCNF.append(",");
    }
    ratioCNF.push_back(CNFresult.size());

    for (unsigned i = 0; i < this->VC1result.size(); i++) 
    {
    	resVC1.append(std::to_string(this->VC1result[i]));
    	if (i != this->VC1result.size() - 1)
    		resVC1.append(",");
    }
    ratioVC1.push_back(VC1result.size());

    for (unsigned i = 0; i < this->VC2result.size(); i++) 
    {
    	resVC2.append(std::to_string(this->VC2result[i]));
    	if (i != this->VC2result.size() - 1)
    		resVC2.append(",");
    }
    ratioVC2.push_back(VC2result.size());

    this->CNFresult.clear();
    this->VC1result.clear();
    this->VC2result.clear();
    
    std::cout << resCNF << std::endl;
    std::cout << resVC1 << std::endl;
    std::cout << resVC2 << std::endl;

}


void *InputThreadFun(void *vargp) 
{
	struct graph_data *data = (struct graph_data *)vargp;
	bool flag = true,vflag = false,eflag = false;
	
	if(!data->graph.isEmpty())
    {
        while(true)
        {
            if(countVec == 3)
                break;
        }
        countVec = 0;
        data->graph.printResult();
    }
    while(flag)
    {
    	if(std::cin.eof())
    	{
    		long CNFaverage = accumulate( timeCNF.begin(), timeCNF.end(), 0.0) / timeCNF.size(); 
	        long sdCNF = std_long(timeCNF,CNFaverage);
//	         std::cout << "CNF mean is: " << CNFaverage << std::endl;
//	         std::cout << "CNF std is: " << sdCNF << std::endl;   
	        
	        long VC1average = accumulate( timeVC1.begin(), timeVC1.end(), 0.0) / timeVC1.size(); 
	        long sdVC1 = std_long(timeVC1,VC1average);
//	         std::cout << "VC1 mean is: " << VC1average << std::endl;
//	         std::cout << "VC1 std is: " << sdVC1 << std::endl;   
	        
	        long VC2average = accumulate( timeVC2.begin(), timeVC2.end(), 0.0) / timeVC2.size(); 
	        long sdVC2 = std_long(timeVC2,VC2average);
//	         std::cout << "VC2 mean is: " << VC2average << std::endl;
//	         std::cout << "VC2 std is: " << sdVC2 << std::endl;   
	        
	        std::vector<float> ratio1, ratio2;
	        
	        for(unsigned i = 0; i < ratioCNF.size(); i++)
	         {
	            ratio1.push_back(float(ratioVC1[i]) / float(ratioCNF[i]));
	        	ratio2.push_back(float(ratioVC2[i]) / float(ratioCNF[i]));    
	        }
	        
	        float averageRatioVC1 = accumulate( ratio1.begin(), ratio1.end(), 0.0) / ratio1.size();
	        float averageRatioVC2 = accumulate( ratio2.begin(), ratio2.end(), 0.0) / ratio2.size(); 
	        
	        float sdratioVC1 = std_float(ratio1,averageRatioVC1);
	        float sdratioVC2 = std_float(ratio2, averageRatioVC2);

	        // uncomment this if you want to check the approximation ratio result
	        
//	        std::cout << "ratio VC1 average is: " << averageRatioVC1 << std::endl;
//	        std::cout << "ratio VC1 std is: " << sdratioVC1 << std::endl;
//	        
//	        std::cout << "ratio VC2 average is: " << averageRatioVC2 << std::endl;
//	        std::cout << "ratio VC2 std is: " << sdratioVC2 << std::endl;
	        

    		exit(0);
    	}

		while(!std::cin.eof())
		{
			std::string input_str;
			int num;   
			std::string edges_input;
			std::vector<std::string> inputs;
			std::vector<int> edgeVertices;

			std::getline(std::cin, input_str);
			inputs = split(input_str);
				
			if(inputs.size() == 2)
			{
				if(inputs[0] == "V")
				{
					num = std::stoi(inputs[1]);
					data->graph.init(num);
                    vflag = true;
                    data->graph.V = num;
                    data->graph.VEmpFlag = 1;
					
				}
				else if(inputs[0] == "E")
				{
					edges_input = inputs[1];
					//std::cout<<edges_input<<std::endl;
					edgeVertices = remove(edges_input);
					if(edgeVertices.size()==0)
					{
						std::cout<<std::endl;
						continue;
					}
					data->graph.Buildgraph(edgeVertices, num);
					data->graph.vertex = edgeVertices;

                    eflag = true;
                    data->graph.EEmpFlag = 1;
					
												
				}
				else
					std::cerr<<"Error: Invalid string!"<<std::endl;
				
			}	
			else
				std::cerr<<"Error: Invalid input"<<std::endl;
					
			
			if (vflag && eflag ) 
			{
                flag = false;
                break;
            }
	    }
	}   
    return 0;
}



void *CNFthreadFun(void *vargp) 
{ 
    struct graph_data *data = (struct graph_data *)vargp;
    //std::cout<<"insidecnf";
    if(data->graph.isEmpty())
    {
    	//std::cout<<"insidecnf1";
    	return NULL;
	}
        
    //std::cout<<"insidecnf2";
    data->graph.CNFVertexCover();
    s = pthread_getcpuclockid(pthread_self(), &start_time3);        
    if (s != 0)
        handle_error_en(s, "pthread_getcpuclockid");
    s_timespec3 = pclock(start_time3);

    timeCNF.push_back(s_timespec3.tv_sec * 1000000 + s_timespec3.tv_nsec / 1000); 

    return  NULL;

} 

void *approx1ThreadFun(void *vargp) 
{ 
    struct graph_data *data = (struct graph_data *)vargp;
    if(data->graph.isEmpty())
        return NULL;

    data->graph.approx1();

    s = pthread_getcpuclockid(pthread_self(), &start_time1);        
    if (s != 0)
        handle_error_en(s, "pthread_getcpuclockid");
    
    s_timespec1 = pclock(start_time1);
    timeVC1.push_back(s_timespec1.tv_sec * 1000000 + s_timespec1.tv_nsec / 1000);
    
    return  NULL;
} 

void *approx2ThreadFun(void *vargp) 
{
	struct graph_data *data = (struct graph_data *)vargp;
	if(data->graph.isEmpty())
        return NULL;
    data->graph.approx2();

    s = pthread_getcpuclockid(pthread_self(), &start_time2);        
    if (s != 0)
        handle_error_en(s, "pthread_getcpuclockid");
    s_timespec2 = pclock(start_time2);
    timeVC2.push_back(s_timespec2.tv_sec * 1000000 + s_timespec2.tv_nsec / 1000);
    
    return  NULL;



    
} 
int main()
{

	int returncode;
	struct graph_data data;
	pthread_t input_thread; 
	pthread_t cnf_thread; 
	pthread_t approx1_thread; 
	pthread_t approx2_thread; 

	while(true)
	{

	    returncode = pthread_create(&input_thread, NULL, InputThreadFun, (void *)&data);
	    if (returncode != 0) 
            std::cerr << "Error when creating Input Thread !" << std::endl;
        
	    
	    returncode = pthread_create(&cnf_thread, NULL, CNFthreadFun, (void *)&data);
	    if (returncode != 0) 
            std::cerr << "Error when creating CNF Thread !" << std::endl;
	     
	    returncode = pthread_create(&approx1_thread, NULL, approx1ThreadFun, (void *)&data); 
	    if (returncode != 0) 
            std::cerr << "Error when creating approx VC1 Thread !" << std::endl;
	      
	    returncode = pthread_create(&approx2_thread, NULL, approx2ThreadFun, (void *)&data);
	    if (returncode != 0) 
            std::cerr << "Error when creating approx VC2 Thread !" << std::endl;
	    
	    pthread_join(input_thread, NULL);
	    pthread_join(cnf_thread, NULL);
	    pthread_join(approx1_thread, NULL);
	    pthread_join(approx2_thread, NULL);

	} 
     
    
}