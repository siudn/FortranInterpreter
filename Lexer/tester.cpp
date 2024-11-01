#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include "lex.h"

using namespace std;

int main(int argc, char* argv[]) 
{
    if (argc < 2 || argv[1][0] == '-')
    {
        cout << "NO SPECIFIED INPUT FILE." << endl;
        return 1;
    }

    bool showAll = false, showInt = false, showReal = false, showStr = false, showId = false, showKw = false;

    for (int i = 2; i < argc; i++) 
    {
        string arg = argv[i];

        if (arg == "-all") 
        {
            showAll = true;
        }
        else if (arg == "-int") 
        {
            showInt = true;
        }
        else if (arg == "-real") 
        {
            showReal = true;
        }
        else if (arg == "-str") 
        {
            showStr = true;
        }
        else if (arg == "-id")
        {
            showId = true;
        }
        else if (arg == "-kw") 
        {
            showKw = true;
        }
        else {
            if (arg[0] != '-')
            {
                cout << "ONLY ONE FILE NAME IS ALLOWED." << endl;
                return 1;
            }
            cout << "UNRECOGNIZED FLAG {" << arg << "}\n";
            return 1;
        }
    }

    ifstream inFile(argv[1]);
    if (!inFile.is_open()) 
    {
        cout << "CANNOT OPEN THE FILE " << argv[1] << endl;
        return 1;
    }

    int totalTokens = -1;
    int numIdents = 0;
    int numInts = 0;
    int numReals = 0;
    int numStrings = 0;

    set<string> uniqueIds;
    set<string> uniqueKws;
    set<int> uniqueInts;
    set<double> uniqueReals;
    set<string> uniqueStrs;

    map<string, int> idCount;
    map<string, int> kwCount;

    static set<Token> keywordTokens = {
        PROGRAM,
        END,
        ELSE,
        IF,
        INTEGER,
        REAL,
        CHARACTER,
        PRINT,
        LEN,
        THEN
    };

    int lineNum = 1;
    LexItem token;
    do 
    {
        token = getNextToken(inFile, lineNum);

        if (showAll && token.GetToken() != DONE && token.GetToken() != ERR) {
            cout << token;
        }

        if (token.GetToken() == IDENT) {
            uniqueIds.insert(token.GetLexeme());
            idCount[token.GetLexeme()]++;
            numIdents++;
        }
        else if (token.GetToken() == ICONST) {
            uniqueInts.insert(stoi(token.GetLexeme()));
            numInts++;
        }
        else if (token.GetToken() == RCONST) {
            uniqueReals.insert(stod(token.GetLexeme()));
            numReals++;
        }
        else if (token.GetToken() == SCONST) {
            uniqueStrs.insert(token.GetLexeme());
            numStrings++;
        }
        else if (keywordTokens.find(token.GetToken()) != keywordTokens.end())
        {
            uniqueKws.insert(token.GetLexeme());
            kwCount[token.GetLexeme()]++;
        }
        totalTokens++;

    } while (token.GetToken() != DONE && token.GetToken() != ERR);

    if (totalTokens == 0)
    {
        cout << "Empty File." << endl;
        return 1;
    }

    if (token.GetToken() == DONE) 
    {
        cout << '\n';
        cout << "Lines: " << lineNum - 1 << endl;
        cout << "Total Tokens: " << totalTokens << endl;
        cout << "Identifiers: " << uniqueIds.size() << endl;
        cout << "Integers: " << uniqueInts.size() << endl;
        cout << "Reals: " << uniqueReals.size() << endl;
        cout << "Strings: " << uniqueStrs.size() << endl;

        if (showId)
        {
            cout << "IDENTIFIERS:" << endl;
            auto it = idCount.begin();
            while (it != idCount.end())
            {
                cout << it->first << " (" << it->second << ")";
                it++;
                if (it != idCount.end())
                {
                    cout << ", ";
                }
            }
            cout << endl;
        }
        if (showKw)
        {
            cout << "KEYWORDS:" << endl;
            auto it = kwCount.begin();
            while (it != kwCount.end())
            {
                cout << it->first << " (" << it->second << ")";
                it++;
                if (it != kwCount.end())
                {
                    cout << ", ";
                }
            }
            cout << endl;
        }
        if (showInt)
        {
            cout << "INTEGERS:" << endl;
            auto it = uniqueInts.begin();
            while (it != uniqueInts.end())
            {
                cout << *it;
                it++;
                if (it != uniqueInts.end())
                {
                    cout << ", ";
                }
            }
            cout << endl;
        }
        if (showReal)
        {
            cout << "REALS:" << endl;
            auto it = uniqueReals.begin();
            while (it != uniqueReals.end())
            {
                cout << *it;
                it++;
                if (it != uniqueReals.end())
                {
                    cout << ", ";
                }
            }
            cout << endl;
        }
        if (showStr)
        {
            cout << "STRINGS:" << endl;
            auto it = uniqueStrs.begin();
            while (it != uniqueStrs.end())
            {
                cout << *it;
                it++;
                if (it != uniqueStrs.end())
                {
                    cout << ", ";
                }
            }
            cout << endl;
        }
    }
    else if (token.GetToken() == ERR) 
    {
        cout << "Error in line " << lineNum << ": Unrecognized Lexeme {" << token.GetLexeme() << "}" << endl;
    }

    inFile.close();
    return 0;
}
