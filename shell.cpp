#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;
namespace fs = std::filesystem;

string findPath(const string& cmd)
{
    const char* path = getenv("PATH");

    if (!path)
        return "";

    stringstream ss(path);
    string dir;

    while (getline(ss, dir, ':'))
    {
        string candidate = dir + "/" + cmd;

        if (access(candidate.c_str(), X_OK) == 0)
            return candidate;
    }

    return "";
}
void echo(string &line,vector<string>&ans)
{
    int n = line.size(); 
    bool inquotes = false; bool doublequot=false;
    string current=""; 
    bool backslash = false;
    bool inbackslashquote= false;

    for(int i=0;i<n;i++)
    {
        char ch = line[i]; 

        if(backslash)
        {
            current+=ch;
            backslash=false;
        }
        else if(doublequot)
        {
            if(ch=='\"' && !inbackslashquote)
            doublequot = false; 
            else if(ch == '\\' || inbackslashquote)
            {
                if(ch=='\\' && !inbackslashquote)inbackslashquote=true;
                else if(inbackslashquote){current+=ch;inbackslashquote=false;}
            }
            else current+=ch;
        }
        else if(inquotes)
        {
            if(ch=='\'')
            {
                inquotes = false;
            }
            else current+=ch;
        }
        else
        {
            if(ch=='\\')
            {
                backslash=true;
            }
            else if(ch=='\"')
            {
                doublequot = true;
            }
            else if(ch=='\'')
            {
                inquotes = true;
            }
            else if(isspace(ch))
            {
                if(!current.empty())
                {ans.emplace_back(current);current.clear();}
                
            }
            else current+=ch;
        }
    }

    if(!current.empty())
    {
        ans.emplace_back(current);
    }

}
int main()
{
    cout << unitbuf;
    cerr << unitbuf;

    unordered_set<string> builtins = {
        "echo",
        "exit",
        "type",
        "pwd",
        "cd"
    };

    while (true)
    {
        cout << "$ ";

        string line;
        getline(cin, line);

        stringstream ss(line);

        string arg;
        ss >> arg;

        if (arg.empty())
            continue;

        if (arg == "exit")
        {
            break;
        }
        else if (arg == "echo")
        {
            vector<string>v; 
            echo(line,v);

            for(int i=1;i<v.size();i++)
            {
                cout << v[i] << " " ;
            }            
            cout << endl;

        }
        else if (arg == "pwd")
        {
            cout << fs::current_path().string() << '\n';
        }
        else if (arg == "type")
        {
            string cmd;
            ss >> cmd;

            if (builtins.count(cmd))
            {
                cout << cmd << " is a shell builtin\n";
            }
            else
            {
                string path = findPath(cmd);

                if (!path.empty())
                    cout << cmd << " is " << path << '\n';
                else
                    cout << cmd << ": not found\n";
            }
        }
        else if (arg == "cd")
        {
            string path;
            ss >> path;

            if (path == "~")
                path = getenv("HOME");

            if (chdir(path.c_str()) != 0)
            {
                cout << "cd: " << path
                     << ": No such file or directory" << endl;
            }
        }
        else
        {
            string path = findPath(arg);

            if (!path.empty())
            {

                vector<string> args;
                echo(line,args); 

                vector<char*> argv;

                for (auto& s : args)
                    argv.push_back(s.data());

                argv.push_back(nullptr);

                pid_t pid = fork();

                if (pid == 0)
                {
                    execv(path.c_str(), argv.data());

                    perror("execv");
                    exit(1);
                }
                
                wait(nullptr);
                
            }
            else
            {
                cout << arg << ": command not found\n";
            }
        }
    }

    return 0;
}