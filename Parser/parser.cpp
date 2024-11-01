#include "parser.h"

map<string, bool> defVar;
map<string, Token> SymTable;

int length = 0;
int nestingLevel = 0;

namespace Parser 
{
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) 
	{
		if (pushed_back) 
		{
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem& t) 
	{
		if (pushed_back) 
		{
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

bool Prog(istream& in, int& line)
{
	bool status = true;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok != PROGRAM)
	{
		ParseError(line, "Keyword PROGRAM expected");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok != IDENT)
	{
		ParseError(line, "Identifier expected");
		return false;
	}

	while (true)
	{
		tok = Parser::GetNextToken(in, line);
		if (tok != INTEGER && tok != REAL && tok != CHARACTER)
		{
			Parser::PushBackToken(tok);
			break;
		}

		Parser::PushBackToken(tok);
		status = Decl(in, line);

		if (!status)
		{
			ParseError(line, "Declaration error");
			return false;
		}
	}

	while (true)
	{
		tok = Parser::GetNextToken(in, line);
		if (tok != IDENT && tok != IF && tok != PRINT)
		{
			Parser::PushBackToken(tok);
			break;
		}

		Parser::PushBackToken(tok);
		status = Stmt(in, line);

		if (!status)
		{
			ParseError(line, "Statement error");
			return false;
		}
	}

	tok = Parser::GetNextToken(in, line);
	if (tok != END)
	{
		ParseError(line, "END expected");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if (tok != PROGRAM)
	{
		ParseError(line, "PROGRAM expected");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok != IDENT)
	{
		ParseError(line, "IDENT expected");
		return false;
	}

	cout << "(DONE)" << endl;
	return status;
}

bool Stmt(istream& in, int& line)
{
	bool status = true;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == IDENT)
	{
		Parser::PushBackToken(tok);
		status = AssignStmt(in, line);
	}
	else if (tok == IF)
	{
		tok = Parser::GetNextToken(in, line);
		if (tok != LPAREN)
		{
			ParseError(line, "LPAREN expected");
			return false;
		}
		status = RelExpr(in, line);
		if (!status)
		{
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if (tok != RPAREN)
		{
			ParseError(line, "RPAREN expected");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if (tok == THEN)
		{
			Parser::PushBackToken(tok);
			status = BlockIfStmt(in, line);
			if (!status)
			{
				return false;
			}
		}
		else if (tok == IDENT || tok == PRINT)
		{
			Parser::PushBackToken(tok);
			status = SimpleIfStmt(in, line);
			if (!status)
			{
				return false;
			}
		}
		else
		{
			ParseError(line, "IFStmt incorrectly formatted");
			return false;
		}
	}
	else if (tok == PRINT)
	{
		Parser::PushBackToken(tok);
		status = PrintStmt(in, line);
	}

	return status;
}

bool AssignStmt(istream& in, int& line)
{
	bool status = true;
	LexItem tok = Parser::GetNextToken(in, line);

	if (defVar[tok.GetLexeme()] == false)
	{
		ParseError(line, "Variable not declared");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok != ASSOP)
	{
		ParseError(line, "ASSOP expected");
		return false;
	}

	status = Expr(in, line);
	if (!status)
	{
		return false;
	}

	return status;
}

bool BlockIfStmt(istream& in, int& line)
{
	bool status = true;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok != THEN)
	{
		ParseError(line, "THEN expected");
		return false;
	}

	++nestingLevel;

	while (true)
	{
		tok = Parser::GetNextToken(in, line);
		if (tok != IDENT && tok != IF && tok != PRINT)
		{
			Parser::PushBackToken(tok);
			break;
		}

		Parser::PushBackToken(tok);
		status = Stmt(in, line);
		if (!status)
		{
			return false;
		}
	}

	tok = Parser::GetNextToken(in, line);
	if (tok == ELSE)
	{
		while (true)
		{
			tok = Parser::GetNextToken(in, line);
			if (tok != IDENT && tok != IF && tok != PRINT)
			{
				Parser::PushBackToken(tok);
				break;
			}

			Parser::PushBackToken(tok);
			status = Stmt(in, line);
			if (!status)
			{
				return false;
			}
		}
	}
	else
	{
		Parser::PushBackToken(tok);
	}

	tok = Parser::GetNextToken(in, line);
	if (tok != END)
	{
		ParseError(line, "END expected");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok != IF)
	{
		ParseError(line, "IF expected");
		return false;
	}

	cout << "End of Block If statement at nesting level " << nestingLevel << endl;
	--nestingLevel;

	return status;
}

bool SimpleIfStmt(istream& in, int& line)
{
	bool status = true;

	status = SimpleStmt(in, line);

	return status;
}

bool SimpleStmt(istream& in, int& line)
{
	bool status = true;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == IDENT)
	{
		Parser::PushBackToken(tok);
		status = AssignStmt(in, line);
		if (status)
		{
			cout << "Assignment statement in a Simple If statement." << endl;
		}
	}
	else if (tok == PRINT)
	{
		Parser::PushBackToken(tok);
		status = PrintStmt(in, line);
		if (status)
		{
			cout << "Print statement in a Simple If statement." << endl;
		}
	}

	return status;
}

bool PrintStmt(istream& in, int& line)
{
	bool status = true;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok != PRINT)
	{
		ParseError(line, "PRINT expected");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok != DEF)
	{
		ParseError(line, "DEF expected");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	status = ExprList(in, line);
	if (!status)
	{
		return false;
	}

	return status;
}

bool RelExpr(istream& in, int& line)
{
	bool status = Expr(in, line);
	if (!status)
	{
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok != EQ && tok != LTHAN && tok != GTHAN)
	{
		Parser::PushBackToken(tok);
		return true;
	}

	status = Expr(in, line);
	if (!status)
	{
		return false;
	}

	return status;
}

bool ExprList(istream& in, int& line)
{
	bool status = Expr(in, line);
	if (!status)
	{
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok != COMMA)
	{
		Parser::PushBackToken(tok);
		return true;
	}

	status = ExprList(in, line);
	return status;
}

bool Decl(istream& in, int& line)
{
	LexItem tok = Parser::GetNextToken(in, line);
	if (tok != INTEGER && tok != REAL && tok != CHARACTER)
	{
		ParseError(line, "Type expected");
		return false;
	}

	Token type = tok.GetToken();

	Parser::PushBackToken(tok);
	bool status = Type(in, line);
	if (!status)
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok != DCOLON)
	{
		ParseError(line, "DCOLON expected");
		return false;
	}

	status = VarList(in, line, type);
	if (!status)
	{
		return false;
	}

	return status;
}

bool Type(istream& in, int& line)
{
	bool status = true;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == CHARACTER)
	{
		tok = Parser::GetNextToken(in, line);

		if (tok == LPAREN)
		{
			tok = Parser::GetNextToken(in, line);
			if (tok != LEN)
			{
				ParseError(line, "LEN expected");
				return false;
			}

			tok = Parser::GetNextToken(in, line);
			if (tok != ASSOP)
			{
				ParseError(line, "ASSOP expected");
				return false;
			}

			tok = Parser::GetNextToken(in, line);
			if (tok != ICONST)
			{
				ParseError(line, "ICONST expected");
				return false;
			}
			length = stoi(tok.GetLexeme());

			tok = Parser::GetNextToken(in, line);
			if (tok != RPAREN)
			{
				ParseError(line, "RPAREN expected");
				return false;
			}
		}
		else
		{
			Parser::PushBackToken(tok);
		}
	}

	return status;
}

bool VarList(istream& in, int& line, Token type)
{
	LexItem tok = Parser::GetNextToken(in, line);
	if (tok != IDENT)
	{
		ParseError(line, "IDENT expected");
		return false;
	}

	string var = tok.GetLexeme();

	Parser::PushBackToken(tok);
	bool status = Var(in, line);
	if (!status)
	{
		return false;
	}
	SymTable[var] = type;

	tok = Parser::GetNextToken(in, line);
	if (tok == ASSOP)
	{
		status = Expr(in, line);
		if (!status)
		{
			return false;
		}
		
		cout << "Initialization of the variable " << var << " in the declaration statement." << endl;
		if (length > 0)
		{
			cout << "Definition of Strings with length of " << length << " in declaration statement." << endl;
		}
	}
	else
	{
		Parser::PushBackToken(tok);
	}

	tok = Parser::GetNextToken(in, line);
	if (tok != COMMA)
	{
		Parser::PushBackToken(tok);
		return true;
	}

	status = VarList(in, line, type);
	return status;
}

bool Var(istream& in, int& line)
{
	bool status = true;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok != IDENT)
	{
		ParseError(line, "IDENT expected");
		return false;
	}

	if (defVar[tok.GetLexeme()] == true)
	{
		ParseError(line, "Variable already declared");
		return false;
	}
	defVar[tok.GetLexeme()] = true;

	return status;
}

bool Expr(istream& in, int& line)
{
	bool status = MultExpr(in, line);
	if (!status)
	{
		return false;
	}

	while (true)
	{
		LexItem tok = Parser::GetNextToken(in, line);
		if (tok != PLUS && tok != MINUS && tok != CAT)
		{
			Parser::PushBackToken(tok);
			break;
		}

		status = MultExpr(in, line);
		if (!status)
		{
			return false;
		}
	}

	return status;
}

bool MultExpr(istream& in, int& line)
{
	bool status = TermExpr(in, line);
	if (!status)
	{
		return false;
	}

	while (true)
	{
		LexItem tok = Parser::GetNextToken(in, line);
		if (tok != MULT && tok != DIV)
		{
			Parser::PushBackToken(tok);
			break;
		}

		status = TermExpr(in, line);
		if (!status)
		{
			return false;
		}
	}

	return status;
}

bool TermExpr(istream& in, int& line)
{
	bool status = SFactor(in, line);
	if (!status)
	{
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);
	if ((tok == IDENT && SymTable[tok.GetLexeme()] == REAL) || tok == RCONST)
	{
		while (true)
		{
			tok = Parser::GetNextToken(in, line);
			if (tok != POW)
			{
				Parser::PushBackToken(tok);
				break;
			}

			status = SFactor(in, line);
			if (!status)
			{
				return false;
			}

			tok = Parser::GetNextToken(in, line);
			if ((tok == IDENT && SymTable[tok.GetLexeme()] != REAL) || tok != RCONST)
			{
				ParseError(line, "Real expected");
				return false;
			}
		}
	}

	return status;
}

bool SFactor(istream& in, int& line)
{
	bool status = true;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == PLUS)
	{
		status = Factor(in, line, 1);
	}
	else if (tok == MINUS)
	{
		status = Factor(in, line, -1);
	}
	else
	{
		Parser::PushBackToken(tok);
		status = Factor(in, line, 1);
		if (!status)
		{
			return false;
		}
	}

	if (!status)
	{
		return false;
	}

	return status;
}

bool Factor(istream& in, int& line, int sign)
{
	bool status = true;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == ICONST || tok == RCONST || tok == SCONST)
	{
		status = true;
	}
	else if (tok == IDENT)
	{
		if (defVar[tok.GetLexeme()] == false)
		{
			ParseError(line, "Variable not declared");
			return false;
		}
	}
	else if (tok == LPAREN)
	{
		status = Expr(in, line);
		if (!status)
		{
			ParseError(line, "Expression expected");
			return false;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok != RPAREN)
		{
			ParseError(line, "RPAREN expected");
			return false;
		}
	}
	else
	{
		ParseError(line, "Factor expected");
		return false;
	}

	Parser::PushBackToken(tok);
	return status;
}


