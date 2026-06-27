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
#include "trie.h"
#include "terminal.h"
#include<dirent.h>
#include<set>
#include<unordered_set>
using namespace std;
namespace fs = std::filesystem;
Trie root;
set<string>commands;
set<string>files;
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
        if(ar[i]==">" || ar[i]=="1>" || ar[i]=="2>" || ar[i]==">>" || ar[i]=="1>>" || ar[i]=="2>>")
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
                        else if(ar[out]==">>" || ar[out]=="1>>" || ar[out]=="2>>")
                        fd = open(filename.c_str(),O_WRONLY | O_CREAT | O_APPEND,0644);
                        if(fd==-1)
                        {
                        perror("open"); 
                        exit(1);
                         }
                         if(op=="2>" || op=="2>>")
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
void redraw(string line)
{
  cout << "\r"; 
  cout << "\033[K" ; 
  cout << "$ "<<line; 
  cout.flush();
}
void load_path_commands()
{
  const char* path_env = getenv("PATH"); 
  if(!path_env)return  ;

  stringstream ss(path_env); 
  string dir ;

  while(getline(ss,dir,':'))
  {
    DIR* di = opendir(dir.c_str()); 
    if(!di)continue;

    struct dirent* entry; 
    while((entry=readdir(di))!=nullptr)
    {
      string name = entry->d_name;
      if(name=="." || name =="..")continue;
      commands.insert(name);
    }
  }

}
vector<string> match(string &line)
{
  vector<string>m;
  auto it = commands.lower_bound(line); 

  while(it!=commands.end()&& it->starts_with(line))
  {
    m.push_back(*it);
    it++;
  }

  return m;

}
string longest_common_prefix(vector<string>matches)
{
  string lcp = matches[0]; 
  for(auto it : matches)
  {
    size_t i =0; 
    while(i<lcp.size() && i<it.size() && lcp[i]==it[i])i++; 
    lcp = lcp.substr(0,i);
  }

  return lcp;
}
void load_files()
{
    string path = fs::current_path().string();
    DIR* di = opendir(path.c_str()); 
    if(!di)return ; 

    struct dirent* entry; 
    while((entry=readdir(di))!=nullptr)
    {
        string name = entry->d_name; 
        if(name=="."||name=="..")continue; 
        files.insert(name);
    }
}
string file_autocomplete(string name)
{
    string m=""; 
    auto it = files.lower_bound(name); 
    if(it->starts_with(name))
    m=name; 
    return m;
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
    int tab_counter = 0;
    load_path_commands();
    for(auto it : builtins)
    {
      root.insert(it);
    }

    Terminal term; 
    term.enableDirectInput();
    bool u1= false;
    while (true)
    {
        cout << "$ ";

        string line;
     
        while(true)
        {
          char ch = term.getchar(); 
          if((int)ch==9)
          { 

            if(line.find(' ')!=string::npos)
           { string temp = line;
            line = root.autocomplete(line);
           redraw(line);
           
           if(temp!=line){
           cout << "\033[1C";
           line+=" ";}

           else if(temp==line)
           {  
            vector<string>matches=match(line); 
            tab_counter++; 
             if(tab_counter==1)
            {
              if(matches.empty())
              {
                cout << "\x07";
              }
              else {
              string lcp = longest_common_prefix(matches);
              if(lcp.size()>line.size())
              {
                line = lcp; 
                redraw(line);

                if(matches.size()==1)
                {
                  cout << "\033[1C"; 
                  line+=" ";
                }
                tab_counter=0;
              }
              else 
                cout << "\x07";

            }}
            else if(tab_counter==2)
            {
              if(matches.empty())
              cout << '\a' << flush; 
              else {
              cout << '\n' ; 
              for(auto it : matches)
                cout << it << " "; 

              cout << '\n';

              redraw(line);}
              tab_counter=0;
            }
           }
           else 
            {
              cout << "\x07" ;
              tab_counter=0;
            }
           cout << flush;
        }
            else 
            {
                load_files(); 
                if(!files.empty())
                {
                stringstream ww(line); 
                vector<string>rec;
                string word; 
                while(ww >> word)
                {
                    if(ww.peek() ==EOF || ww.eof()){
                        break;
                    }
                    rec.push_back(word);
                }
                string cmpt = file_autocomplete(word); 
                if(cmpt.empty())
                {
                    cout << "\0x7" << flush ; 
                   

                }
                else 
                {
                    string ne =""; 
                    for(auto it : rec)
                    {ne+=it;ne+=" ";}
                    ne+=cmpt; 
                    line = ne ;
                    redraw(line); 
                     cout << "\033[1C";
                     line+=" ";
                }

                }
                

            }
          }

          else if((int)ch==127)
          {
            if(!line.empty())
            line.pop_back(); 
            redraw(line);
          }
          else if((int)ch==10){cout << '\n' << flush;break;}
          else 
          {line+=ch;redraw(line);}
        }

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
                else if(ar[out]==">>" || ar[out] == "1>>" || ar[out]=="2>>")
                fd = open(filename.c_str(),O_WRONLY | O_CREAT | O_APPEND,0644);
                if(ar[out] ==">" || ar[out] =="1>" || ar[out]==">>" || ar[out] == "1>>")
                {
                    for(int i=1;i<out;i++)
                    {
                    write(fd,ar[i].c_str(),strlen(ar[i].data()));
                    write(fd," ",1);
                    }
                    write(fd,"\n",1); 
                    close(fd);
                }
                else
                {
                    dup2(fd,STDERR_FILENO); 
                    close(fd);
                     for(int i=1;i<out;i++)
                    {
                        cout << ar[i] << " " ;
                    }
                    cout << endl;

                }
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
