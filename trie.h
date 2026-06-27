#ifndef TRIE_H
#define TRIE_H

#include<vector>
#include<string>
using namespace std;
class Trie{
  struct Node{
    Node * links[128] ={nullptr}; 
    bool flag = false;

    bool containskey(char ch)
    {
      return (links[ch]!=NULL);
    }
    
    void put(char ch ,Node* node)
    {
      links[ch]=node;
    }

    Node* get(char ch)
    {
      return links[ch];
    }

    void setEnd()
    {
      flag = true;
    }

    bool isEnd()
    {
      return flag;
    }
  };

  public :

  struct Node* root = new Node();

    void insert(string word)
    {
      Node* node = root;
      for(int i=0;i<word.size();i++)
      {
        if(!node->containskey(word[i]))
        {
            node->put(word[i],new Node());
        } 
          node = node->get(word[i]);
      }

      node ->setEnd();
    }

    bool search (string word)
    {
      Node* node = root;
      for(int i=0;i<word.size();i++)
      {
        if(!node->containskey(word[i]))return false ; 
       node = node->get(word[i]);
      }

      return node->isEnd();
    }

    bool startswith(string word)
    {
      Node* node = root;
      for(int i=0;i<word.size();i++)
      {
        if(!node->containskey(word[i]))return false;
      }

      return true;
    }

    bool dfs(Node*node,string& final)
    {
      if(node->isEnd())return true;
      for(int i=0;i<128;i++)
      {
        if(node ->links[i])
        {
          final+=(char)(i); 
          if(dfs(node->links[i],final))return true; 

          final.pop_back();
        }
      }

      return false;
    }

    string autocomplete(string cur)
    {
      Node* node = root;
      string final="";
     for(int i=0;i<cur.size();i++)
     {
       if(!node->containskey(cur[i]))return cur;
       else 
       {
         final+=cur[i];
         node = node->get(cur[i]); 

       }
     }

      if(dfs(node,final))return final;
      else return cur;

      
    }
};

#endif
