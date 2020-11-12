#include <cstdlib>
#include <iostream>
#include <sstream>
#include <ctime>
#include <vector>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>



int procShorPath(void)
{
	int ret;
	char *cpp_argv[2];
	cpp_argv[0]= (char*)"ece650-a2"; 
	cpp_argv[1] = 0;
	ret = execv(cpp_argv[0],cpp_argv);
	if(ret == -1)
	std::cerr<< "Error: In Shortest Path Assignment" << std::endl;
	return ret;
	
}
int procPyScript(void)
{
	int ret;
	char *py_argv[3];
	py_argv[0] = (char*)"python";
    py_argv[1] = (char*)"ece650-a1.py";
    py_argv[2] = 0;
	ret = execvp(py_argv[0],py_argv);
	if(ret == -1)
	std::cerr << "Error: In Python File" <<std::endl;
	return ret;
}

int procRanGen(char **argv)
{
	int ret;
	argv[0] = (char*)"rgen";
	ret = execv(argv[0],argv);
	if(ret == -1)
	std::cerr << "Error: In Random Generator" <<std::endl;
	return ret;
}
int main(int argc,char **argv)
{	
     std::vector<pid_t> processid;
     int pipeGen1[2], pipeGen2[2];
     pipe(pipeGen1);
     pipe(pipeGen2);
     
     pid_t kid_pid;
     kid_pid = fork();
     if(kid_pid == 0)
     {
     	dup2(pipeGen1[1], STDOUT_FILENO);
        close(pipeGen1[0]);
        close(pipeGen1[1]);
        
        
		int ret = procRanGen(argv);
		return ret;
	 }
	 else if (kid_pid < 0)
	 {
        std::cerr << "Error: can't fork \n";
        return 1;
     }
      
     processid.push_back(kid_pid);
      
     kid_pid = fork();
    if (kid_pid == 0)
    {
        dup2(pipeGen1[0], STDIN_FILENO);
        close(pipeGen1[1]);
        close(pipeGen1[0]);
        
        
        dup2(pipeGen2[1] , STDOUT_FILENO);
    	close(pipeGen2[0]);
    	close(pipeGen2[1]);
    	
        int ret =  procPyScript();
        return ret;
    }
    
    else if (kid_pid < 0) 
	{
        std::cerr << "Error: can't fork\n";
        return 1;
    }
    
    processid.push_back(kid_pid);
    	
	kid_pid = fork();
	
	if(kid_pid == 0)
	{
		
		dup2(pipeGen2[0], STDIN_FILENO);
		close(pipeGen2[1]);
		close(pipeGen2[0]);
		
		int ret = procShorPath(); 
		return ret;
	}
	else if (kid_pid < 0)
	{
		std::cerr << "Error: can't fork\n";
		return 1;
	}
	
	processid.push_back(kid_pid);
	
	dup2(pipeGen2[1] , STDOUT_FILENO);
    close(pipeGen2[0]);
    close(pipeGen2[1]);
    
    
    std::string UserInput;
    
    while(!std::cin.eof())
    {
    	  
    	std::getline (std::cin,UserInput);
    	std::cout<<UserInput<<"\n";
	}
	
	int statusptr;
	
	for(unsigned int i=0; i<processid.size(); i++)
	{
		kill(processid[i], SIGTERM);
		waitpid(processid[i],&statusptr,0);
	}
	
	return 0;
	
}