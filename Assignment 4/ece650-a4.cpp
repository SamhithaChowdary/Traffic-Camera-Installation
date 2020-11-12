#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <iterator>
#include <list>
#include <algorithm>
#include <stdlib.h>
#include <cstdlib>
#include <minisat/core/Solver.h>


using namespace std;
class UDGraph
{
	int V;
	list<int> *adj;
	public:
		UDGraph(int V);
		void addedge(int a , int b);
		void MinimumVertexCover();
		
};

UDGraph::UDGraph(int V){
	this->V = V;
	adj = new list<int>[V];
} 

void UDGraph::addedge(int a , int b){
	adj[a].push_back(b);
	adj[b].push_back(a);
}


vector<std::string> split(std::string input_str){
	vector<std::string> ret;
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


vector<int> remove(std::string edges)
{
	vector<int> edgeVertices;
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


bool Buildgraph(vector<int> edgeVertices , int num , UDGraph Graph){
	int N =edgeVertices.size();
	int f,s;
	
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
		
		
		Graph.addedge(f , s);
				
	}
	return true;

}

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
void findvertexcover(int n,vector<int> edgeVertices)
{
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
	std::cout<<answer[i]<<" ";
	
	std::cout<<std::endl;
	
}


int main(){
	
	std::string input_str;
	int num;   
	UDGraph Graph(0);
	std::string edges_input;
	vector<std::string> inputs;
	vector<int> edgeVertices;
	
	while(!std::cin.eof())
	{
		std::getline(std::cin, input_str);
		inputs = split(input_str);
			
		if(inputs.size() == 2)
		{
			if(inputs[0] == "V")
			{
				num = std::stoi(inputs[1]);
				Graph = UDGraph(num);
				
			}
			else if(inputs[0] == "E")
			{
				edges_input = inputs[1];
				edgeVertices = remove(edges_input);
				if(edgeVertices.size()==0)
				{
					std::cout<<std::endl;
					continue;
				}
				if (Buildgraph(edgeVertices,num,Graph))
				{
					findvertexcover(num,edgeVertices);	
				}
				
											
			}
			else
				std::cerr<<"Error: Invalid string!"<<std::endl;
			
		}	
		else
			std::cerr<<"Error: Invalid input"<<std::endl;
				
		
    }   
    return 0;
}