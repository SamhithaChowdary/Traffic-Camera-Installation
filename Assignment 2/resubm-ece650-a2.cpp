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


using namespace std;
class UDGraph
{
	int V;
	list<int> *adj;
	public:
		UDGraph(int V);
		void addedge(int a , int b);
		std::list<int> shortestPath(int s, int e);
		
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
			std::cerr << "Error: Invalid vertice" << std::endl;
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


std::list<int> UDGraph::shortestPath(int s, int e)
{
    bool *visited = new bool[V];
    std::fill_n(visited, V, 0);

    std::list<int> result;
    std::list<int> path;
    std::list<int> new_path;
    std::list<int> neighbours;

    path.push_back(s);

    std::vector<std::list<int> > queue;
    int node;
    int tmp;

    if (s == e)
        return result;

    queue.push_back(path);

    std::list<int>::iterator i;

    while (!queue.empty()) // queue is a vector
    {
        path = queue.front();
        queue.erase(queue.begin());

        node = path.back();

        if (!visited[node])
        {
            neighbours = adj[node];

            for (auto& neighbour : neighbours)
            {
                new_path = path;
                tmp = neighbour;
                new_path.push_back(tmp);
                queue.push_back(new_path);

                if (tmp == e)
                {
                    return new_path;
                }
            }
            visited[node] = 1;
        }
    }
    return result;
}

void generatePath(std::vector<std::string> inputs, UDGraph Graph) {
    std::list<int> result_path;
    std::list<int>::const_iterator iterator;
    
    int start;
    int end;
    
    start = std::stoi(inputs[1]);
    end = std::stoi(inputs[2]);
    result_path = Graph.shortestPath(start, end);

    std::string res;
    
    std::vector<int> v{ std::begin(result_path), std::end(result_path) };
    
    for(unsigned i = 0; i < v.size(); ++i)
    {
      if(i != 0)
        res.append("-");
      res.append(std::to_string(v[i]));
    }

    if (!result_path.empty()) {
        std::cout << res << std::endl;
    }else{
        std::cerr << "Error: No path between given vertices!" << std::endl;
    }    
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
				Buildgraph(edgeVertices , num , Graph);
				
											
			}
			else
				std::cerr<<"Error: Invalid string!"<<std::endl;
			
		}
		else if(inputs.size() == 3 && inputs[0] == "s")
		{
			if(num == 0) 
			{
               std::cerr << "Error: empty graph!" << std::endl;
            }
            else if(std::stoi(inputs[1]) >= num || std::stoi(inputs[2]) >= num)
			{
            	std::cerr << "Error: no path exists between these two vertices" << std::endl;
			}
			else
			{
				generatePath(inputs , Graph);
			}
		}	
		else
			std::cerr<<"Error: Invalid input"<<std::endl;
				
		
    }   
    return 0;
}

