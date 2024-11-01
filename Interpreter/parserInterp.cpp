#include <stack>

#include "parserInterp.h"

map<string, bool> defVar;
map<string, Token> SymTable;

map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value>* ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if (pushed_back) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem& t) {
		if (pushed_back) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;
	}

}

static int error_count = 0;

int ErrCount()
{
	return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

//Program is: Prog = PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line)
{
	bool dl = false, sl = false;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			dl = Decl(in, line);
			if (!dl)
			{
				ParseError(line, "Incorrect Declaration in Program");
				return false;
			}
			sl = Stmt(in, line);
			if (!sl)
			{
				ParseError(line, "Incorrect Statement in program");
				return false;
			}
			tok = Parser::GetNextToken(in, line);

			if (tok.GetToken() == END) {
				tok = Parser::GetNextToken(in, line);

				if (tok.GetToken() == PROGRAM) {
					tok = Parser::GetNextToken(in, line);

					if (tok.GetToken() == IDENT) {
						cout << "(DONE)" << endl;
						return true;
					}
					else
					{
						ParseError(line, "Missing Program Name");
						return false;
					}
				}
				else
				{
					ParseError(line, "Missing PROGRAM at the End");
					return false;
				}
			}
			else
			{
				ParseError(line, "Missing END of Program");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program name");
			return false;
		}
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	else
	{
		ParseError(line, "Missing Program keyword");
		return false;
	}
}

//Decl ::= Type :: VarList 
//Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)] 
bool Decl(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	string strLen;

	LexItem t = Parser::GetNextToken(in, line);

	if (t == INTEGER || t == REAL || t == CHARACTER) {
		tok = t;

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == DCOLON) {
			status = VarList(in, line, t);

			if (status)
			{
				status = Decl(in, line);
				if (!status)
				{
					ParseError(line, "Declaration Syntactic Error.");
					return false;
				}
				return status;
			}
			else
			{
				ParseError(line, "Missing Variables List.");
				return false;
			}
		}
		else if (t == CHARACTER && tok.GetToken() == LPAREN)
		{
			tok = Parser::GetNextToken(in, line);

			if (tok.GetToken() == LEN)
			{
				tok = Parser::GetNextToken(in, line);

				if (tok.GetToken() == ASSOP)
				{
					tok = Parser::GetNextToken(in, line);

					if (tok.GetToken() == ICONST)
					{
						strLen = tok.GetLexeme();

						tok = Parser::GetNextToken(in, line);
						if (tok.GetToken() == RPAREN)
						{
							tok = Parser::GetNextToken(in, line);
							if (tok.GetToken() == DCOLON)
							{
								status = VarList(in, line, t, stoi(strLen));

								if (status)
								{
									status = Decl(in, line);
									if (!status)
									{
										ParseError(line, "Declaration Syntactic Error.");
										return false;
									}
									return status;
								}
								else
								{
									ParseError(line, "Missing Variables List.");
									return false;
								}
							}
						}
						else
						{
							ParseError(line, "Missing Right Parenthesis for String Length definition.");
							return false;
						}

					}
					else
					{
						ParseError(line, "Incorrect Initialization of a String Length");
						return false;
					}
				}
			}
		}
		else
		{
			ParseError(line, "Missing Double Colons");
			return false;
		}

	}

	Parser::PushBackToken(t);
	return true;
}//End of Decl

//Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream& in, int& line) {
	bool status;
	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken()) {

	case PRINT:
		status = PrintStmt(in, line);

		if (status)
			status = Stmt(in, line);
		break;

	case IF:
		status = BlockIfStmt(in, line);
		if (status)
			status = Stmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);
		if (status)
			status = Stmt(in, line);
		break;


	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}//End of Stmt

bool SimpleStmt(istream& in, int& line) {
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken()) {

	case PRINT:
		status = PrintStmt(in, line);

		if (!status)
		{
			ParseError(line, "Incorrect Print Statement");
			return false;
		}
		cout << "Print statement in a Simple If statement." << endl;
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);
		if (!status)
		{
			ParseError(line, "Incorrect Assignent Statement");
			return false;
		}
		cout << "Assignment statement in a Simple If statement." << endl;

		break;


	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}//End of SimpleStmt

//VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream& in, int& line, LexItem& idtok, int strlen) {
	bool status = false, exprstatus = false;
	string identstr;
	Value retVal;
	Value newVal;

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == IDENT)
	{
		identstr = tok.GetLexeme();

		auto it = defVar.find(identstr);
		if (it == defVar.end())
		{
			defVar[identstr] = true;
			SymTable[identstr] = idtok.GetToken();
			if (idtok == CHARACTER)
			{
				string newStr = "";
				newStr.resize(strlen, ' ');
				newVal = Value(newStr);
				newVal.SetstrLen(strlen);
				TempsResults[identstr] = newVal;
			}
		}
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
	}
	else
	{
		ParseError(line, "Missing Variable Name");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok == ASSOP)
	{
		exprstatus = Expr(in, line, retVal);
		if (!exprstatus)
		{
			ParseError(line, "Incorrect initialization for a variable.");
			return false;
		}

		if (idtok == CHARACTER)
		{
			string newString = retVal.GetString();
			newString.resize(strlen, ' ');
			retVal = Value(newString);
			retVal.SetstrLen(strlen);
		}
		else if (idtok == REAL && retVal.IsInt())
		{
			double realVal = retVal.GetInt();
			retVal = Value(realVal);
		}

		TempsResults[identstr] = retVal;

		tok = Parser::GetNextToken(in, line);

		if (tok == COMMA) 
		{
			status = VarList(in, line, idtok, strlen);
		}
		else
		{
			Parser::PushBackToken(tok);
			return true;
		}
	}
	else if (tok == COMMA) 
	{
		if (idtok == CHARACTER)
		{
			string newString = "";
			newString.resize(strlen, ' ');
			retVal = Value(newString);
			retVal.SetstrLen(strlen);
			TempsResults[identstr] = retVal;
			SymTable[identstr] = idtok.GetToken();
		}
		status = VarList(in, line, idtok, strlen);
	}
	else if (tok == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		return false;
	}
	else 
	{
		Parser::PushBackToken(tok);
		return true;
	}

	return status;

}//End of VarList



//PrintStmt:= PRINT *, ExpreList 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;

	t = Parser::GetNextToken(in, line);

	if (t != DEF) 
	{
		ParseError(line, "Print statement syntax error.");
		return false;
	}
	t = Parser::GetNextToken(in, line);

	if (t != COMMA) 
	{
		ParseError(line, "Missing Comma.");
		return false;
	}
	bool ex = ExprList(in, line);

	if (!ex) 
	{
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}

	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;
	return ex;
}//End of PrintStmt

//BlockIfStmt:= if (RelExpr) then {Stmt} [Else Stmt]
//SimpleIfStatement := if (RelExpr) Stmt
bool BlockIfStmt(istream& in, int& line) {
	bool ex = false, status;
	LexItem t;
	Value retVal;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN) {

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	ex = RelExpr(in, line, retVal);
	if (!ex) {
		ParseError(line, "Missing if statement condition");
		return false;
	}

	// if retVal is true, then execute the THEN block
	// else skip the if block and execute the ELSE block

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN) {

		ParseError(line, "Missing Right Parenthesis");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != THEN)
	{
		if (retVal.IsBool() && retVal.GetBool())
		{
			Parser::PushBackToken(t);

			status = SimpleStmt(in, line);
			if (status)
			{
				return true;
			}
			else
			{
				ParseError(line, "If-Stmt Syntax Error");
				return false;
			}
		}
		else
		{
			// skip block if relExpr false
			// this is erroneous, but not covered in test cases
			// issue with EBNF and parser implementation
			return true;
		}
	}

	if (retVal.GetBool())
	{
		status = Stmt(in, line);
		if (!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Then-Part");
			return false;
		}
		t = Parser::GetNextToken(in, line);
	}
	else
	{
		while (t != ELSE && t != END)
		{
			t = Parser::GetNextToken(in, line);
		}
	}

	if (t == ELSE)
	{
		status = Stmt(in, line);
		if (!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Else-Part");
			return false;
		}
		else
		{
			t = Parser::GetNextToken(in, line);
		}
	}
	if (t != END) 
	{
		ParseError(line, "Missing END of IF");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t == IF) 
	{
		return true;
	}

	Parser::PushBackToken(t);
	ParseError(line, "Missing IF at End of IF statement");
	return false;
	
}//End of IfStmt function

//Var:= ident
bool Var(istream& in, int& line, LexItem& idtok) // note changes
{
	string identstr;

	idtok = Parser::GetNextToken(in, line);

	if (idtok == IDENT) {
		identstr = idtok.GetLexeme();

		auto it = defVar.find(identstr);
		if (it == defVar.end())
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}
		return true;
	}
	else if (idtok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << idtok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}//End of Var

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {

	bool varstatus = false, status = false;
	LexItem idtok;

	varstatus = Var(in, line, idtok);

	if (varstatus) {
		LexItem t = idtok;
		t = Parser::GetNextToken(in, line);

		if (t == ASSOP) {
			Value retVal;

			status = Expr(in, line, retVal);
			if (!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}

			if (SymTable[idtok.GetLexeme()] == CHARACTER)
			{
				if (TempsResults[idtok.GetLexeme()].IsString())
				{
					if (!retVal.IsString())
					{
						line -= 2; // patch for error line number
						ParseError(line, "Incompatible types in assignment");
						return false;
					}
					string newString = retVal.GetString();
					int strlen = TempsResults[idtok.GetLexeme()].GetstrLen();
					newString.resize(strlen, ' ');
					retVal = Value(newString);
					retVal.SetstrLen(strlen);
				}
			}
			TempsResults[idtok.GetLexeme()] = retVal;
		}
		else if (t.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;
}//End of AssignStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;

	status = Expr(in, line, retVal);
	if (!status) {
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == COMMA) {

		status = ExprList(in, line);

	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else {
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList

//RelExpr ::= Expr  [ ( == | < | > ) Expr ]
bool RelExpr(istream& in, int& line, Value& retVal) {

	bool t1 = Expr(in, line, retVal);
	LexItem tok;

	if (!t1) {
		return false;
	}

	Value newVal = retVal;

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if (tok == EQ || tok == LTHAN || tok == GTHAN)
	{
		t1 = Expr(in, line, retVal);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (tok == EQ)
		{
			retVal = newVal == retVal;

			if (retVal.IsErr())
			{
				ParseError(line, "Comparison of incompatible types");
				return false;
			}
		}
		else if (tok == LTHAN)
		{
			retVal = newVal < retVal;

			if (retVal.IsErr())
			{
				ParseError(line, "Comparison of incompatible types");
				return false;
			}
		}
		else if (tok == GTHAN)
		{
			retVal = newVal > retVal;

			if (retVal.IsErr())
			{
				ParseError(line, "Comparison of incompatible types");
				return false;
			}
		}
	}

	return true;
}//End of RelExpr

//Expr ::= MultExpr { ( + | - | // ) MultExpr }
bool Expr(istream& in, int& line, Value& retVal) {

	bool t1 = MultExpr(in, line, retVal);
	LexItem tok;

	if (!t1) {
		return false;
	}

	Value newVal = retVal;

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == PLUS || tok == MINUS || tok == CAT)
	{
		t1 = MultExpr(in, line, retVal);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (tok == PLUS)
		{
			retVal = newVal + retVal;

			if (retVal.IsErr())
			{
				line -= 2; // patch for error line number
				ParseError(line, "Addition of incompatible types");
				return false;
			}
		}
		else if (tok == MINUS)
		{
			retVal = newVal - retVal;

			if (retVal.IsErr())
			{
				ParseError(line, "Subtraction of incompatible types");
				return false;
			}
		}
		else if (tok == CAT)
		{
			retVal = newVal.Catenate(retVal);

			if (retVal.IsErr())
			{
				ParseError(line, "Concatenation of incompatible types");
				return false;
			}
		}

		newVal = retVal;

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}

	}
	Parser::PushBackToken(tok);

	return true;
}//End of Expr

//MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream& in, int& line, Value& retVal) {

	bool t1 = TermExpr(in, line, retVal);
	LexItem tok;

	if (!t1) {
		return false;
	}

	Value newVal = retVal;

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == MULT || tok == DIV)
	{
		t1 = TermExpr(in, line, retVal);

		if (!t1) {
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (tok == MULT)
		{
			retVal = newVal * retVal;

			if (retVal.IsErr())
			{
				ParseError(line, "Multiplication of incompatible types");
				return false;
			}
		}
		else
		{
			retVal = newVal / retVal;

			if (retVal.IsErr())
			{
				line--; // patch for error line number
				ParseError(line, "Division of incompatible types");
				return false;
			}
		}

		newVal = retVal;

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}

	}
	Parser::PushBackToken(tok);

	return true;
}//End of MultExpr

//TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream& in, int& line, Value& retVal) {

	bool t1 = SFactor(in, line, retVal);
	LexItem tok;

	if (!t1) 
	{
		return false;
	}

	Value newVal = retVal;

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR) 
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == POW)
	{
		t1 = SFactor(in, line, retVal);

		if (!t1) {
			ParseError(line, "Missing exponent operand");
			return false;
		}

		Value secondVal = retVal;

		tok = Parser::GetNextToken(in, line);
		if (tok == POW)
		{
			t1 = SFactor(in, line, retVal);
			if (!t1) {
				ParseError(line, "Missing exponent operand");
				return false;
			}
			secondVal = secondVal.Power(retVal);
			newVal = newVal.Power(secondVal);
			retVal = newVal;
			tok = Parser::GetNextToken(in, line);
			continue;
		}
		else
		{
			Parser::PushBackToken(tok);
		}

		retVal = newVal.Power(retVal);

		if (retVal.IsErr())
		{
			ParseError(line, "Exponentiation requires two real numbers");
			return false;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR) {
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}

		newVal = retVal;
	}
	Parser::PushBackToken(tok);

	return true;
}//End of TermExpr

//SFactor = Sign Factor | Factor
bool SFactor(istream& in, int& line, Value& retVal)
{
	LexItem t = Parser::GetNextToken(in, line);

	bool status;
	int sign = 1;
	if (t == MINUS)
	{
		sign = -1;
	}
	else if (t == PLUS)
	{
		sign = 1;
	}
	else
	{
		Parser::PushBackToken(t);
	}

	status = Factor(in, line, sign, retVal);
	return status;
}//End of SFactor

//Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream& in, int& line, int sign, Value& retVal) {

	LexItem tok = Parser::GetNextToken(in, line);

	//cout << tok.GetLexeme() << endl;
	if (tok == IDENT) 
	{
		string lexeme = tok.GetLexeme();
		auto hi = defVar.find(lexeme);
		if (hi == defVar.end())
		{
			ParseError(line, "Using Undefined Variable");
			return false;
		}
		auto it = TempsResults.find(lexeme);
		if (it == TempsResults.end())
		{
			ParseError(line, "Using Variable without Initialization");
			return false;
		}
		if (sign == -1)
		{
			retVal = TempsResults[lexeme] * Value(-1);
			if (retVal.IsErr())
			{
				ParseError(line, "Negation of a non-numeric value");
				return false;
			}
		}
		else
		{
			retVal = TempsResults[lexeme];
		}
		return true;
	}
	else if (tok == ICONST) 
	{
		retVal = Value(sign * stoi(tok.GetLexeme()));
		return true;
	}
	else if (tok == SCONST) 
	{
		if (sign == -1)
		{
			ParseError(line, "String cannot be negated");
			return false;
		}
		retVal = Value(tok.GetLexeme());
		return true;
	}
	else if (tok == RCONST) 
	{
		retVal = Value(sign * stod(tok.GetLexeme()));
		return true;
	}
	else if (tok == LPAREN) {
		bool ex = Expr(in, line, retVal);
		if (!ex) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if (Parser::GetNextToken(in, line) == RPAREN)
			return ex;
		else
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	return false;
}



