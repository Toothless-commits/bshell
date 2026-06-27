#ifndef TERMINAL_H
#define TERMINAL_H
#include<termios.h>
class Terminal{

  private : struct termios oldt,newt;

  public : 
     Terminal()
  {
    tcgetattr(STDIN_FILENO,&oldt); 
    newt = oldt;
  }

  void enableDirectInput()
  {
    newt.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO,TCSANOW,&newt);
  }

  void normalmode()
  {
    tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
  }

  char getchar()
  {
    char c; 
    read(STDIN_FILENO,&c,1); 
    return c;
  }



};

#endif
