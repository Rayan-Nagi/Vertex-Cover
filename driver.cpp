#include <vector>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

using namespace std;


int userInput(void) {
  
  while (!cin.eof()) {
    string line;
    getline(cin, line);
    if (line.size()>0){
    cout << line
	 << endl;
    }
  }
  return 0;
}



int main(int argc, char *argv[]) {

  vector<pid_t> kids;
  
  // Create 2 pipes
  
  int RtoA1[2];
  int A1toA2[2];
  pipe(RtoA1);
  pipe(A1toA2);

  
  pid_t child_pid;
  
  
  // First fork to call random generator (rgen)
  
  child_pid = fork();
  
  if (child_pid == 0) {
    
    dup2(RtoA1[1], STDOUT_FILENO);
    close(RtoA1[0]);
    close(RtoA1[1]);

    return execv ("./rgen", argv);
  }

  kids.push_back(child_pid);
  

  // Second fork to call A1

  child_pid = fork();

  if (child_pid == 0) {

    dup2(RtoA1[0], STDIN_FILENO);
    close(RtoA1[1]);
    close(RtoA1[0]);

    dup2(A1toA2[1], STDOUT_FILENO);
    close(A1toA2[0]);
    close(A1toA2[1]);    

    char *argv_list[3];
    argv_list[0] = (char*) "python3";
    argv_list[1] = (char*) "graph.py";
    argv_list[2] = NULL;

    return execvp ("python3", argv_list);
  }

  kids.push_back(child_pid);
  

  // Third fork to call A2

  child_pid = fork();

  if (child_pid == 0) {

    dup2(A1toA2[0], STDIN_FILENO);
    close(A1toA2[1]);
    close(A1toA2[0]); 

    return execv ("./spath", argv);
  }

  kids.push_back(child_pid);
  child_pid = 0;
  

  dup2(A1toA2[1], STDOUT_FILENO);
  close(A1toA2[0]);
  close(A1toA2[1]);


  int res = userInput();


  // // Kill all children processes  
  
  for (pid_t k : kids) {
    int status;
    kill(k, SIGTERM);
    waitpid(k, &status, 0);
  }

  return res;
}
