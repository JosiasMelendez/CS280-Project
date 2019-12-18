/*
 * value.h
 */

#ifndef VALUE_H_
#define VALUE_H_

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "projlex.h"
using namespace std;

enum NodeType { ERRTYPE, INTTYPE, STRTYPE };

class Value {
	int	ival;
	string sval;
	NodeType type;

public:
	Value() : ival(0),type(ERRTYPE) {}
	Value(int i) : ival(i),type(INTTYPE) {}
	Value(string s) : ival(0),sval(s),type(STRTYPE) {}

	NodeType GetType() const { return type; }

	int GetIntValue() const {
		if( type != INTTYPE )
			throw std::runtime_error("RUNTIME ERROR: using GetIntValue on a Value that is not an INT");
		return ival;
	}

	string GetStrValue() const {
		if( type != STRTYPE )
			throw std::runtime_error("RUNTIME ERROR: using GetStrValue on a Value that is not a STRING");
		return sval;
	} 
};

inline Value operator+(const Value& a, const Value& b) {
   NodeType at = a.GetType();
   NodeType bt = b.GetType();
   if ((at == bt) && (at == INTTYPE))
      return Value(a.GetIntValue() + b.GetIntValue());
     
   else if ((at == bt) && (at == STRTYPE))
      return Value(a.GetStrValue() + b.GetStrValue());
      
   throw std::runtime_error("RUNTIME ERROR: adding a string Value and an int Value");
}

inline Value operator-(const Value& a, const Value& b) {
   NodeType at = a.GetType();
   NodeType bt = b.GetType();
   if ((at == bt) && (at == INTTYPE))
      return Value(a.GetIntValue() - b.GetIntValue());
      
   else if ((at == bt) && (at == STRTYPE)) {
      string s1 = a.GetStrValue();
      string s2 = b.GetStrValue();
      size_t found = s1.find(s2);
      if (found != string::npos) {
         string s3 = s1.erase(found, s2.length());
         return Value(s3);
      }
      else
         return Value(s1);
   }
   
   throw std::runtime_error("RUNTIME ERROR: subtracting a string Value and an int Value");       
}

inline Value operator*(const Value& a, const Value& b) {
   NodeType at = a.GetType();
   NodeType bt = b.GetType();
   if ((at == bt) && (at == INTTYPE))
      return Value(a.GetIntValue()* b.GetIntValue());
      
   else if ((at != bt) && ((at == INTTYPE && bt == STRTYPE) || (at == STRTYPE && bt == INTTYPE))) {
      if (at == INTTYPE) {
         string s1 = b.GetStrValue();
         int mul = a.GetIntValue();
         string s2;
         for (int i = 0; i < mul; i++)
            s2 += s1;
         return s2;
      }
      if (bt == INTTYPE) {
         string s1 = a.GetStrValue();
         int mul = b.GetIntValue();
         string s2;
         for (int i = 0; i < mul; i++)
            s2 += s1;
         return s2;
     }
   }
   throw std::runtime_error("RUNTIME ERROR: multipying incompatible types");  
}

inline ostream& operator<<(ostream& out, const Value& a) {
   NodeType at = a.GetType();
   if (at == INTTYPE) {
      out << a.GetIntValue() << "\n";
      return out;
   }
   if (at == STRTYPE) {
      out << a.GetStrValue() << "\n";
      return out;
   }
   throw std::runtime_error("RUNTIME ERROR: cannot print");
}

   
#endif /* VALUE_H_ */