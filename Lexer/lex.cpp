#include "lex.h"
#include <cctype>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <string>

using namespace std;

LexItem getNextToken(istream& in, int& linenum) {
	enum LexState { START, INID, ININT, INREAL, INSTR, INCOMMENT };
	LexState state = START;
	string lexeme = "";
	int flag = 0;
	char ch;

	while (in.get(ch))
	{
		switch (state)
		{
		case START: {
			if (isspace(ch))
			{
				if (ch == '\n')
				{
					linenum++;
				}
				continue;
			}

			lexeme = ch;

			if (isalpha(ch))
			{
				state = INID;
			}
			else if (isdigit(ch))
			{
				state = ININT;
			}
			else if (ch == '"' || ch == '\'')
			{
				state = INSTR;
			}
			else if (ch == '!')
			{
				state = INCOMMENT;
			}
			else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == '<' || ch == '>')
			{
				switch (ch)
				{
				case '+': // operators
					return LexItem(PLUS, lexeme, linenum);
				case '-':
					return LexItem(MINUS, lexeme, linenum);
				case '*':
					if (in.peek() == '*')
					{
						in.get();
						lexeme += ch;
						return LexItem(POW, lexeme, linenum);
					}
					if (isalpha(in.peek()) || in.peek() == ',')
					{
						return LexItem(DEF, lexeme, linenum);
					}
					return LexItem(MULT, lexeme, linenum);
				case '/':
					if (in.peek() == '/')
					{
						in.get();
						lexeme += ch;
						return LexItem(CAT, lexeme, linenum);
					}
					else
					{
						return LexItem(DIV, lexeme, linenum);
					}
				case '=':
					if (in.peek() == '=')
					{
						in.get();
						lexeme += ch;
						return LexItem(EQ, lexeme, linenum);
					}
					else
					{
						return LexItem(ASSOP, lexeme, linenum);
					}
				case '<':
					return LexItem(LTHAN, lexeme, linenum);
				case '>':
					return LexItem(GTHAN, lexeme, linenum);
				}
			}
			else
			{
				switch (ch)
				{
				case ',':
					return LexItem(COMMA, lexeme, linenum);
				case '(':
					return LexItem(LPAREN, lexeme, linenum);
				case ')':
					return LexItem(RPAREN, lexeme, linenum);
				case ':':
					if (in.peek() == ':')
					{
						in.get();
						lexeme += ch;
						return LexItem(DCOLON, lexeme, linenum);
					}
					return LexItem(ERR, lexeme, linenum);
				case '.':
					if (isdigit(in.peek()))
					{
						state = INREAL;
						flag = 1;
						break;
					}
					return LexItem(DOT, lexeme, linenum);
				}
				if (flag == 0)
				{
					return LexItem(ERR, lexeme, linenum);
				}
			}
		} break;

		case INID: {
			while (isalnum(ch) || ch == '_')
			{
				lexeme += ch;
				if (!in.get(ch) || in.eof())
				{
					return id_or_kw(lexeme, linenum);
				}
			}
			in.putback(ch);

			return id_or_kw(lexeme, linenum);
		} break;

		case ININT: {
			while (isdigit(ch))
			{
				lexeme += ch;
				if (!in.get(ch) || in.eof())
				{
					break;
				}
			}
			in.putback(ch);

			if (in.peek() == '.')
			{
				lexeme += '.';
				state = INREAL;
				break;
			}

			return LexItem(ICONST, lexeme, linenum);
		} break;

		case INREAL: {
			if (ch == '.')
			{
				if (!isdigit(in.peek()))
				{
					return LexItem(ICONST, lexeme, linenum);
				}
				in.get(ch);
			}

			while (isdigit(ch))
			{
				lexeme += ch;
				if (!in.get(ch) || in.eof())
				{
					break;
				}
			}
			if (isalpha(ch) || ch == '.')
			{
				lexeme += ch;
				return LexItem(ERR, lexeme, linenum);
			}

			in.putback(ch);
			return LexItem(RCONST, lexeme, linenum);
		} break;

		case INSTR: {
			char startQuote = lexeme[0];

			while (ch != '"' && ch != '\'' && ch != '\n' && !in.eof())
			{
				lexeme += ch;
				in.get(ch);
			}
			if (ch == '\'')
			{
				if (startQuote == '\'')
				{
					lexeme[0] = '"';
					lexeme += '"';
					return LexItem(SCONST, lexeme, linenum);
				}
			}
			else if (ch == '"')
			{
				if (startQuote == '"')
				{
					lexeme += ch;
					return LexItem(SCONST, lexeme, linenum);
				}
			}
			if (ch != '\n')
			{
				lexeme += ch;
			}
			return LexItem(ERR, lexeme, linenum);
		} break;

		case INCOMMENT: {
			while (ch != '\n' && !in.eof())
			{
				in.get(ch);
			}
			in.putback(ch);
			state = START;
		} break;

		}
	}

	return LexItem(DONE, lexeme, linenum);
}

string toLower(const string& str) {
	string result = str;
	transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
	return result;
}

LexItem id_or_kw(const string& lexeme, int linenum) {
	static unordered_map<string, Token> keywordMap = {
		{"program", PROGRAM},
		{"end", END},
		{"else", ELSE},
		{"if", IF},
		{"integer", INTEGER},
		{"real", REAL},
		{"character", CHARACTER},
		{"print", PRINT},
		{"len", LEN},
		{"then", THEN}
	};

	auto it = keywordMap.find(toLower(lexeme));
	if (it != keywordMap.end())
	{
		return LexItem(it->second, lexeme, linenum);
	}
	return LexItem(IDENT, lexeme, linenum);
}

std::map<Token, std::string> tokenToStringMap = {
	{IF, "IF"},
	{ELSE, "ELSE"},
	{PRINT, "PRINT"},
	{INTEGER, "INTEGER"},
	{REAL, "REAL"},
	{CHARACTER, "CHARACTER"},
	{END, "END"},
	{THEN, "THEN"},
	{PROGRAM, "PROGRAM"},
	{LEN, "LEN"},
	{IDENT, "IDENT"},
	{ICONST, "ICONST"},
	{RCONST, "RCONST"},
	{SCONST, "SCONST"},
	{BCONST, "BCONST"},
	{PLUS, "PLUS"},
	{MINUS, "MINUS"},
	{MULT, "MULT"},
	{DIV, "DIV"},
	{ASSOP, "ASSOP"},
	{EQ, "EQ"},
	{POW, "POW"},
	{GTHAN, "GTHAN"},
	{LTHAN, "LTHAN"},
	{CAT, "CAT"},
	{COMMA, "COMMA"},
	{LPAREN, "LPAREN"},
	{RPAREN, "RPAREN"},
	{DOT, "DOT"},
	{DCOLON, "DCOLON"},
	{DEF, "DEF"},
	{ERR, "ERR"},
	{DONE, "DONE"}
};

string tokenToString(Token token)
{
	auto it = tokenToStringMap.find(token);
	return it->second;
}

ostream& operator<<(ostream& out, const LexItem& tok)
{
	Token token = tok.GetToken();

	switch (token)
	{
	case (IDENT):
		out << "IDENT: '" << tok.GetLexeme() << "'" << '\n';
		break;
	case (SCONST):
		out << "SCONST: " << tok.GetLexeme() << '\n';
		break;
	case (ICONST):
		out << "ICONST: (" << tok.GetLexeme() << ")" << '\n';
		break;
	case (RCONST):
		out << "RCONST: (" << tok.GetLexeme() << ")" << '\n';
		break;
	case (BCONST):
		out << "BCONST: (" << tok.GetLexeme() << ")" << '\n';
		break;
	case (ERR):
		break;
	default:
		out << tokenToString(token) << '\n';
	}

	return out;
}