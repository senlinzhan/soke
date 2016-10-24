#include <iostream>
#include <string>
#include <stdlib.h>
#include <editline/readline.h>
#include <editline/history.h>
  
using namespace std;

string readInputLine(const string &prompt)
{
    char *input = readline(prompt.c_str());
    string line(input);
    free(input);
    add_history(line.c_str());
    return line;
}

int main(int argc, char *argv[])
{
    cout << "Lispy Version 0.0.0.0.1" << endl;
    cout << "Press Ctrl+c to Exit\n" << endl;

    while (true)
    {
	auto line = readInputLine("lispy> ");
	if (line == "quit") {
	    break;
	}
	cout << "No you're a " << line << endl;
    }
    
    return 0;
}
