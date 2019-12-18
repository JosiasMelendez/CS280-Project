/*
 * parsetree.h //a binary tree. nodes
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "projlex.h"
#include "value.h"
using std::vector;
using std::map;
class ParseTree {
	int			linenum;
	ParseTree	*left;
	ParseTree	*right; ///create a pointer in each subclass
public:
	ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
		: linenum(linenum), left(l), right(r) {}

	virtual ~ParseTree() {
		delete left;
		delete right;
	}
   
   Value SymbolTable(Value v, string id, int add) {
      static map<string, Value> idMap;
      if (add == 1) {
         idMap[id] = v; 
         return v;
      }
   
      if (add == 0) {
         if (idMap.find(id) != idMap.end())
            return idMap[id];
         else
            return 0;
      }
      else
         return 0;
   }   
    
   ParseTree* GetLeft() { return left; }         
   
   ParseTree* GetRight() { return right; }
          
	int GetLineNumber() const { return linenum; }

	virtual NodeType GetType() { return ERRTYPE; }
   
   virtual Value Eval() { return 0; }
   
   Value lEval() {
      auto lv = left->Eval();
      auto rv = right->Eval();
      if (lv.GetIntValue() > rv.GetIntValue()) {
         cout << linenum << ": left value greater than right";
         return 0;
      }
      if (lv.GetIntValue() < 0) {
         cout << "RUNTIME ERROR: left value cannot be negative";
         return 0;
      }
      return lv;
   }
   
   Value rEval() { 
      auto lv = left->Eval();
      auto rv = right->Eval();
      if (lv.GetIntValue() > rv.GetIntValue()) {
         cout << linenum << ": left value greater than right";
         return 0;
      }
      return rv;
   }
};

class StmtList : public ParseTree {
   ParseTree *left;
   ParseTree *right;
public:
	StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {
      left = l;
      right = r;
   }
   
    NodeType GetType() {
      NodeType lt = left->GetType();
      NodeType rt = right->GetType();
      
      if (lt == rt && lt != ERRTYPE)
         return lt;
      cout << 0 << ": types are not correct";   
      return ERRTYPE;
   }
   
   Value Eval() {
      if (left) {
         auto a1 = left->Eval();
         if (right) {
            auto a2 = right->Eval();
            return a2;
         }
         return a1;
      }
      return 0;
  }
};

class VarDecl : public ParseTree {
   string id;
   ParseTree *expr;
   int line;
public:
   VarDecl(Token& t, ParseTree *e) : ParseTree(t.GetLinenum(), e) {
      id = t.GetLexeme();
      expr = e;
      line = t.GetLinenum();
   }
   
   NodeType GetType() {
      NodeType et = expr->GetType();
      return et;
   }
   
   Value Eval() {
      auto ev = expr->Eval();
      return SymbolTable(ev, id, 1);
   }     
};

class Assignment : public ParseTree {
   string id;
   ParseTree *expr;
public:
   Assignment(Token& t, ParseTree *e) : ParseTree(t.GetLinenum(), e) {
      id = t.GetLexeme();
      expr = e;
   }
   
   NodeType GetType() {
      NodeType et = expr->GetType();
      return et;
   }
   
   Value Eval() {
      auto ev = expr->Eval();
      return SymbolTable(ev, id, 1);
   }
};

class Print : public ParseTree {
   ParseTree *expr;
public:
	Print(int line, ParseTree *e) : ParseTree(line, e) {
      expr = e;
   }
   
   NodeType GetType() {
      NodeType et = expr->GetType();
      return et;
   }
   
   Value Eval() {
      auto ev = expr->Eval();
      cout << ev;
      return ev;
   }  
}; 

class Repeat : public ParseTree {
   ParseTree *expr;
   ParseTree *stmt;
public:
   Repeat(int line, ParseTree *e, ParseTree *st) : ParseTree(line, e, st) {
      expr = e;
      stmt = st;
   }
   
   NodeType GetType() {
      NodeType st = stmt->GetType();
      return st;
   }
   
   Value Eval() {
      auto ev = expr->Eval();
      int rep = ev.GetIntValue();
      if (rep > 0) {
         for (int i = 0; i < rep - 1; i++) {
            stmt->Eval();
         }
      }
      else {
         cout << "RUNTIME ERROR: repeat int is less than 0";
         return 0;
      }
      auto sv = stmt->Eval();   
      return sv;
   }
};

class PlusExpr : public ParseTree {
   ParseTree *left;
   ParseTree *right;
   int ln;
public:
   PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line, l, r) {
      left = l;
      right = r;
      ln = line;
   }
   
   NodeType GetType() {
      NodeType lt = left->GetType();
      NodeType rt = right->GetType();
      
      if (lt == rt && lt != ERRTYPE)
         return lt;
      cout << ln << ": types are not correct";   
      return ERRTYPE;
   } 
   
   Value Eval() {
      auto a1 = left->Eval();
      auto a2 = right->Eval();
      
      return a1 + a2;
   }   
};

class MinusExpr : public ParseTree {
   ParseTree *left;
   ParseTree *right;
   int ln;
public:
   MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line, l, r){
      left = l;
      right = r;
      ln = line;
   }
   
   NodeType GetType() {
      NodeType lt = left->GetType();
      NodeType rt = right->GetType();
      
      if (lt == rt && lt != ERRTYPE)
         return lt;
      cout << ln << ": types are not correct";
      return ERRTYPE;
   }
   
   Value Eval() {
      auto a1 = left->Eval();
      auto a2 = right->Eval();
      
      return a1 - a2;
   }
};

class TimesExpr : public ParseTree {
   ParseTree *left;
   ParseTree *right;
   int ln;
public:
   TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line, l, r){
      left = l;
      right = r;
      ln = line;
   }
   
   NodeType GetType() {
      NodeType lt = left->GetType();
      NodeType rt = right->GetType();
      
      if (lt == rt && lt != ERRTYPE && lt == INTTYPE)
         return lt;
         
      if (lt != rt && ((lt == INTTYPE && rt == STRTYPE) || (lt == STRTYPE && rt == INTTYPE)))
         return STRTYPE;
      cout << ln << ": types are not correct";   
      return ERRTYPE;
   }
   
   Value Eval() {
      auto a1 = left->Eval();
      auto a2 = right->Eval();
      
      return a1 * a2;
   }
};

class SliceExpr : public ParseTree {
   ParseTree *left;
   ParseTree *right;
   int ln;
public:
   SliceExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line, l, r){
      left = l;
      right = r;
      ln = line;
   }
   
   NodeType GetType() {
      NodeType lt = left->GetType();
      return lt;
   }
   
   Value Eval() {
      auto lv = left->Eval();
      if (lv.GetType() == STRTYPE) {
         string str1 = lv.GetStrValue();
         int lslice = (right->lEval()).GetIntValue();
         int rslice = (right->rEval()).GetIntValue();
         string str2 = str1.substr(lslice, (rslice - lslice));
         return str2;
      }
      cout << ln << ": cannot slice an integer";
      return 0;
   }
};

class SliceOperand : public ParseTree {
   ParseTree *left;
   ParseTree *right;
   int ln;
public:
   SliceOperand(int line, ParseTree *l, ParseTree *r) : ParseTree(line, l, r) {
      left = l;
      right = r;
      ln = line;
   }
   
   NodeType GetType() {
      NodeType lt = left->GetType();
      NodeType rt = right->GetType();
      if ((lt == rt) && (lt == INTTYPE))
         return INTTYPE;
      cout << ln << ": types are not correct";
      return ERRTYPE;
   }
};

class IConst : public ParseTree {
	int val;

public:
	IConst(Token& t) : ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}

	NodeType GetType() { return INTTYPE; }
   
   Value Eval() { return Value(val); }
};

class SConst : public ParseTree {
   string val;
   
public:
   SConst(Token& t) : ParseTree(t.GetLinenum()) {
      val = t.GetLexeme();
   }
   
   NodeType GetType() { return STRTYPE; }
   
   Value Eval() { return Value(val); }
};

class Ident : public ParseTree { 
   string id;
 
public:
   Ident (Token& t) : ParseTree(t.GetLinenum()) {
      id = t.GetLexeme();
   }
   
   NodeType GetType() {
      return SymbolTable(0, id, 0).GetType();
   }
   
   Value Eval() {
      return SymbolTable(0, id, 0);
   } 
};
#endif /* PARSETREE_H_ */

