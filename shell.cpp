#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <sys/wait.h>
#include <unistd.h>
#include<fcntl.h>
#include<cstring>
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
int red(vector<string>&ar)
{
    for(int i=0;i<ar.size();i++)
    {
        if(ar[i]==">" || ar[i]=="1>" || ar[i]=="2>")
        {
            return i;
        }

    }
    return -1;
}
void execute(vector<string>ar)
{
     string path = findPath(ar[0]);

            if (!path.empty())
            {
                int out = red(ar);
                vector<char*> argv;
                if(out==-1)
                {for (auto& s : ar)
                    argv.push_back(s.data());}
                    else 
                    {
                        for(int i=0;i<out;i++)
                        {
                            argv.push_back(ar[i].data());
                        }
                    }

                argv.push_back(nullptr);

                pid_t pid = fork();

                if (pid == 0)
                {
                    
                    int fd=-1; 
                    if(out !=-1)
                    {
                        string &op = ar[out]; 
                        string &filename =ar[out+1];
                        if(ar[out]==">" || ar[out] == "1>" ||ar[out]== "2>")
                        fd = open(filename.c_str(),O_WRONLY | O_CREAT | O_TRUNC,0644);
                        else if(ar[out]==">>")
                        fd = open(filename.c_str(),O_WRONLY | O_CREAT | O_TRUNC | O_APPEND,0644);
                        if(fd==-1)
                        {
                        perror("open"); 
                        exit(1);
                         }
                         if(op=="2>")
                    {
                        dup2(fd,2); 
                    }
                    else dup2(fd,1);

                    close (fd);
                    }
                    

                    
                    execv(path.c_str(), argv.data());

                    perror("execv");
                    exit(1);
                }
                
                wait(nullptr);
                cout << flush;
               
                
                
        }
        else 
        { 
                cout << ar[0] << ": command not found\n";
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
            vector<string>ar; 
            echo(line,ar);
            int out = red(ar); 
            if(out==-1){
            for(int i=1;i<ar.size();i++)
            {
                cout << ar[i] << " " ;
            }            
            cout << endl;}
            else 
            {   
                int fd =-1;
                 string &op = ar[out]; 
                string &filename =ar[out+1];
                if(ar[out]==">" || ar[out] == "1>" ||ar[out]== "2>")
                fd = open(filename.c_str(),O_WRONLY | O_CREAT | O_TRUNC,0644);
                else if(ar[out]==">>")
                fd = open(filename.c_str(),O_WRONLY | O_CREAT | O_TRUNC | O_APPEND,0644);

                for(int i=1;i<out;i++)
                {
                    write(fd,ar[i].c_str(),strlen(ar[i].data()));
                    write(fd," ",1);
                }
                write(fd,"\n",1); 
                close(fd);
                
            }

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
            vector<string>ar; 
            echo(line,ar); 

            execute(ar);
            
           
        }
    }

    return 0;
}