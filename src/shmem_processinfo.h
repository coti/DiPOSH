#include <vector>

using namespace std;

class Process{
 private:
    int rank;
    int pid;
 public:
    Process( int _rank, int _pid ) {
        this->rank = _rank;
        this->pid = _pid;
    }
    int getRank(){
        return this->rank;
    }
    int getPid(){
        return this->pid;
    }
};

extern std::vector<Process> processes;
