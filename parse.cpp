/*
 * parse.cpp
 */

#include "parse.h"

namespace Parser {
bool pushed_back = false;
Token	pushed_token;

static Token GetNextToken(istream *in, int *line) {
	if( pushed_back ) {
		pushed_back = false;
		return pushed_token;
	}
	return getNextToken(in, line);
}

static void PushBackToken(Token& t) {
	if( pushed_back ) {
		abort();
	}
	pushed_back = true;
	pushed_token = t;
}

}

static int error_count = 0;

void
ParseError(int line, string msg)
{
   cerr << line << ": " << msg << endl;
   error_count++;
}


ParseTree *Prog(istream *in, int *line)
{
	ParseTree *sl = Slist(in, line); //each function has to call previous

	if( sl == 0 ) //returning 0 means that the function failed
      if (Parser::GetNextToken(in, line) != DONE) {
		   ParseError(*line, "No statements in program");
      }

	if( error_count )
		return 0;
      
	   return sl;
}

// Slist is a Statement followed by a Statement List
ParseTree *Slist(istream *in, int *line) {
   Token t = Parser::GetNextToken(in, line);
   if (t == DONE) {
      Parser::PushBackToken(t);
      return 0;
   }
   Parser::PushBackToken(t);
   ParseTree *s = Stmt(in, line);
   if( s == 0 )
     return 0;

   if( Parser::GetNextToken(in, line) != SC ) {
     ParseError(*line, "Missing semicolon");
     delete s;
     return 0;
   }
   
   return new StmtList(s, Slist(in,line));
}

ParseTree *Stmt(istream *in, int *line) {
   ParseTree *st;
   Token t = Parser::GetNextToken(in, line);
   if (t == VAR) {
      st = VarStmt(in, line);
      if (st == 0)
         return 0;
      else
         return st;
   }
   else if (t == SET) {
      st = SetStmt(in, line);
      if (st == 0)
         return 0;
      else
         return st;
   }
   else if (t == PRINT) {
      st = PrintStmt(in, line);
      if (st == 0)
         return 0;
      else
         return st;
   }
   else if (t == REPEAT) {
      st = RepeatStmt(in, line);
      if (st == 0)
         return 0;
      else
         return st;
   }
	else {
      ParseError(*line, "Missing var, set, print, or repeat");
      return 0;
   }
}

ParseTree *VarStmt(istream *in, int *line) {
   Token t = Parser::GetNextToken(in, line);
   if (t != IDENT) {
      ParseError(*line, "Missing identifier after var");
      return 0;
   }
   
   ParseTree *ex = Expr(in, line);
   if (ex == 0) {
      ParseError(*line, "Missing expression after identifier");
      return 0;
   }
   
	return new VarDecl(t, ex);
}

ParseTree *SetStmt(istream *in, int *line) {
   Token t = Parser::GetNextToken(in, line);
   if (t != IDENT) {
      ParseError(*line, "Missing identifier after set");
      return 0;
   }
   
   ParseTree *ex = Expr(in, line);
   if (ex == 0) {
      ParseError(*line, "Missing expressions after identifier");
      return 0;
   }
   
   return new Assignment(t, ex);
}

ParseTree *PrintStmt(istream *in, int *line) { //how does it know print was declared
	int l = *line; //set l equal to linenumber

	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(*line, "Missing expression after print");
		return 0;
	}

	return new Print(l, ex); //each class here returns a function in the parsetree class that also must be made
}  //if print expression is confirmed, return it

ParseTree *RepeatStmt(istream *in, int *line) {
   int l = *line;

	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(*line, "Missing expression after repeat");
		return 0;
	}

	ParseTree *st = Stmt(in, line);
	if( st == 0 ) {
		ParseError(*line, "Missing statement for repeat");
		return 0;
	}

	return new Repeat(l, ex, st); 
}
//Expr is a term followed by an optional plus/minus followed by another term
ParseTree *Expr(istream *in, int *line) {
	ParseTree *t1 = Term(in, line); //points to first term in this line
	if( t1 == 0 ) { //if there is no term, there is no expr
		return 0;
	}

	while ( true ) { 
		Token t = Parser::GetNextToken(in, line);//retreive the next token after term

		if( t != PLUS && t != MINUS ) { //optional +/- term. check here for that
			Parser::PushBackToken(t);  //put the next token back if it is not + or -
			return t1; //simply return the first term and thats it
		}

		ParseTree *t2 = Term(in, line); //if the next token is plus/minus, create second pointer
		if( t2 == 0 ) { //if there is no term after, error
			ParseError(*line, "Missing expression after operator");
			return 0;
		}

		if( t == PLUS )
			t1 = new PlusExpr(t.GetLinenum(), t1, t2); //go to the plus expr if the token was plus
		else
			t1 = new MinusExpr(t.GetLinenum(), t1, t2); //go to minus expr if the token was minus
	} //we have a new t1
}
//Term is a factor followed by an optional multiplication sign followed by another factor
ParseTree *Term(istream *in, int *line) {
   ParseTree *f1 = Factor(in, line);
   if (f1 == 0)
      return 0;
   
   while (true) {
      Token t = Parser::GetNextToken(in, line);
      
      if (t != STAR) {
         Parser::PushBackToken(t);
         return f1;
      }
      
      ParseTree *f2 = Factor(in, line);
      if (f2 == 0) {
         ParseError(*line, "Missing factor after operator");
         return 0;
      }
      
      if (t == STAR)
         f1 = new TimesExpr(t.GetLinenum(), f1, f2);
   }
      
}
//Factor is a primary followed by an optional
ParseTree *Factor(istream *in, int *line) {
   ParseTree *p1 = Primary(in, line);
   if (p1 == 0)
      return 0;
	
   while (true) {
      Token t = Parser::GetNextToken(in, line);
      
      if (t != LSQ) {
         Parser::PushBackToken(t);
         return p1;
      }
      
      ParseTree *e1 = Expr(in, line);
      if (e1 == 0) {
         ParseError(*line, "Missing first expression for slice operand");
         return 0;
      }
      
      Token t2 = Parser::GetNextToken(in, line);
      
      if (t2 != COLON) {
         ParseError(*line, "Missing colon in slice operand");
         return 0;
      }
      
      ParseTree *e2 = Expr(in, line);
      if (e2 == 0) {
         ParseError(*line, "Missing second expression for slice operand");
         return 0;
      }
      
      Token t3 = Parser::GetNextToken(in, line);
      
      if (t3 != RSQ) {
         ParseError(*line, "Missing second expression for slice operand");
         return 0;
      }
      
      ParseTree *p2 = new SliceOperand(t3.GetLinenum(), e1, e2);
      
      p1 = new SliceExpr(t3.GetLinenum(), p1, p2);
  }
      
}

ParseTree *Primary(istream *in, int *line) {
   Token t = Parser::GetNextToken(in, line);
   
   if (t == IDENT)
      return new Ident(t);
	
   else if (t == ICONST)
      return new IConst(t);
   
   else if (t == SCONST)
      return new SConst(t);
   
   else if( t == LPAREN ) {
		ParseTree *ex = Expr(in, line);
		if( ex == 0 ) {
			ParseError(*line, "Missing expression after repeat");
			return 0;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;

		ParseError(*line, "Missing ) after expression");
		return 0;
	}

	ParseError(*line, "Primary expected");
	return 0;    
}

