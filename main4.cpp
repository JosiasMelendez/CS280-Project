#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include "projlex.h"
#include "parse.h"
#include "parsetree.h"
#include "value.h"
using namespace std;

int main (int argc, char *argv[]) {

   int lineNumber = 0;
   istream *in = &cin;
   ifstream file;
   ParseTree *pt;
      
   if (argc == 2) {
      string arg(argv[1]);
      file.open(arg);
      if (file.is_open() == false) {
         cerr << "COULD NOT OPEN " << arg << endl;
         return 0;
      }
      
      in = &file;
   }
   
   else if (argc > 2) {
      cerr << "TOO MANY FILENAMES" << endl;
      return 0;
   }
   
   pt = Prog(in, &lineNumber);
   pt->Eval();
}