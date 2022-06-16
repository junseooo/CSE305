#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char** argv){
    vector<string> input_arguments;

    for(int i=0; i<argc; i++){
        string str(argv[i]);
        input_arguments.push_back(str);
    }

    
}