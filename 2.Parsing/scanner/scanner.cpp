//------------------------------------------------------------------------------
/// @brief SnuPL/0 scanner
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2013/03/07 Bernhard Egger adapted to SnuPL/0
/// 2014/09/18 Bernhard Egger bugfix (*/ accepted as //)
///
/// @section license_section License
/// Copyright (c) 2012-2014, Bernhard Egger
/// All rights reserved.
///
/// Redistribution and use in source and binary forms,  with or without modifi-
/// cation, are permitted provided that the following conditions are met:
///
/// - Redistributions of source code must retain the above copyright notice,
///   this list of conditions and the following disclaimer.
/// - Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY  AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER  OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSE-
/// QUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE
/// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
/// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT
/// LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY
/// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
/// DAMAGE.
//------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdio>

#include "scanner.h"
using namespace std;

//------------------------------------------------------------------------------
// token names
//
#define TOKEN_STRLEN 16

char ETokenName[][TOKEN_STRLEN] = {
  "tIdent",                         ///< ident
  "tNumber",                        ///< number
  "tBoolConst",                     ///< boolean constant
  "tPlusMinus",                     ///< '+' or '-'
  "tMulDiv",                        ///< '*' or '/'
  "tOr",                            ///< '||'
  "tAnd",                           ///< '&&'
  "tNot",                           ///< '!'
  "tRelOp",                         ///< relational operator
  "tAssign",                        ///< assignment operator
  "tComma",                         ///< a comma
  "tSemicolon",                     ///< a semicolon
  "tColon",                         ///< a colon
  "tDot",                           ///< a dot
  "tLBrak",                         ///< a left bracket
  "tRBrak",                         ///< a right bracket

  "tModule",                        ///< 'module'
  "tProcedure",                     ///< 'procedure'
  "tFunction",                      ///< 'function'
  "tVarDecl",                       ///< 'var'
  "tInteger",                       ///< 'integer'
  "tBoolean",                       ///< 'boolean'
  "tBegin",                         ///< 'begin'
  "tEnd",                           ///< 'end'
  "tIf",                            ///< 'if'
  "tThen",                          ///< 'then'
  "tElse",                          ///< 'else'
  "tWhile",                         ///< 'while'
  "tDo",                            ///< 'do'
  "tReturn",                        ///< 'return'

  "tComment",                       ///< comment ('// .... \n')
  "tEOF",                           ///< end of file
  "tIOError",                       ///< I/O error
  "tUndefined",                     ///< undefined
};


//------------------------------------------------------------------------------
// format strings used for printing tokens
//

char ETokenStr[][TOKEN_STRLEN] = {
  "tIdent (%s)",                    ///< ident
  "tNumber (%s)",                   ///< number
  "tBoolConst (%s)",                ///< boolean constant
  "tPlusMinus (%s)",                ///< '+' or '-'
  "tMulDiv (%s)",                   ///< '*' or '/'
  "tOr",                            ///< '||'
  "tAnd",                           ///< '&&'
  "tNot",                           ///< '!'
  "tRelOp (%s)",                    ///< relational operator
  "tAssign",                        ///< assignment operator
  "tComma",                         ///< a comma
  "tSemicolon",                     ///< a semicolon
  "tColon",                         ///< a colon
  "tDot",                           ///< a dot
  "tLBrak",                         ///< a left bracket
  "tRBrak",                         ///< a right bracket

  "tModule",                        ///< 'module'
  "tProcedure",                     ///< 'procedure'
  "tFunction",                      ///< 'function'
  "tVarDecl",                       ///< 'var'
  "tInteger",                       ///< 'integer'
  "tBoolean",                       ///< 'boolean'
  "tBegin",                         ///< 'begin'
  "tEnd",                           ///< 'end'
  "tIf",                            ///< 'if'
  "tThen",                          ///< 'then'
  "tElse",                          ///< 'else'
  "tWhile",                         ///< 'while'
  "tDo",                            ///< 'do'
  "tReturn",                        ///< 'return'

  "tComment (%s)",                  ///< comment ('// .... \n')
  "tEOF",                           ///< end of file
  "tIOError",                       ///< I/O error
  "tUndefined (%s)",                ///< undefined
};


//------------------------------------------------------------------------------
// reserved keywords
//
pair<const char*, EToken> Keywords[] =
{
  { "module",    tModule    },
  { "procedure", tProcedure },
  { "function",  tFunction  },
  { "var",       tVarDecl   },
  { "integer",   tInteger   },
  { "boolean",   tBoolean   },
  { "begin",     tBegin     },
  { "end",       tEnd       },
  { "if",        tIf        },
  { "then",      tThen      },
  { "else",      tElse      },
  { "while",     tWhile     },
  { "do",        tDo        },
  { "return",    tReturn    },
  { "true",      tBoolConst },
  { "false",     tBoolConst },
};



//------------------------------------------------------------------------------
// CToken
//
CToken::CToken()
{
  _type = tUndefined;
  _value = "";
  _line = _char = 0;
}

CToken::CToken(int line, int charpos, EToken type, const string value)
{
  _type = type;
  _value = value;
  _line = line;
  _char = charpos;
}

CToken::CToken(const CToken &token)
{
  _type = token.GetType();
  _value = token.GetValue();
  _line = token.GetLineNumber();
  _char = token.GetCharPosition();
}

CToken::CToken(const CToken *token)
{
  _type = token->GetType();
  _value = token->GetValue();
  _line = token->GetLineNumber();
  _char = token->GetCharPosition();
}

const string CToken::Name(EToken type)
{
  return string(ETokenName[type]);
}

const string CToken::GetName(void) const
{
  return string(ETokenName[GetType()]);
}

ostream& CToken::print(ostream &out) const
{
  int str_len = _value.length();
  str_len = TOKEN_STRLEN + (str_len < 64 ? str_len : 64);
  char *str = (char*)malloc(str_len);
  snprintf(str, str_len, ETokenStr[GetType()], _value.c_str());
  out << dec << _line << ":" << _char << ": " << str;
  free(str);
  return out;
}

ostream& operator<<(ostream &out, const CToken &t)
{
  return t.print(out);
}

ostream& operator<<(ostream &out, const CToken *t)
{
  return t->print(out);
}


//------------------------------------------------------------------------------
// CScanner
//
map<string, EToken> CScanner::keywords;

CScanner::CScanner(istream *in)
{
  InitKeywords();
  _in = in;
  _delete_in = false;
  _line = _char = 1;
  _token = NULL;
  _good = in->good();
  NextToken();
}

CScanner::CScanner(string in)
{
  InitKeywords();
  _in = new istringstream(in);
  _delete_in = true;
  _line = _char = 1;
  _token = NULL;
  _good = true;
  NextToken();
}

CScanner::~CScanner()
{
  if (_token != NULL) delete _token;
  if (_delete_in) delete _in;
}

void CScanner::InitKeywords(void)
{
  if (keywords.size() == 0) {
    int size = sizeof(Keywords) / sizeof(Keywords[0]);
    for (int i=0; i<size; i++) {
      keywords[Keywords[i].first] = Keywords[i].second;
    }
  }
}

CToken CScanner::Get()
{
  CToken result(_token);

  EToken type = _token->GetType();
  _good = !(type == tIOError);

  NextToken();
  return result;
}

CToken CScanner::Peek() const
{
  return CToken(_token);
}

void CScanner::NextToken()
{
  if (_token != NULL) delete _token;

  _token = Scan();
}

void CScanner::RecordStreamPosition()
{
  _saved_line = _line;
  _saved_char = _char;
}

void CScanner::GetRecordedStreamPosition(int *lineno, int *charpos)
{
  *lineno = _saved_line;
  *charpos = _saved_char;
}

CToken* CScanner::NewToken(EToken type, const string token)
{
  return new CToken(_saved_line, _saved_char, type, token);
}

CToken* CScanner::Scan()
{
  EToken token;
  string tokval;
  char c;

  do {
    while (_in->good() && IsWhite(_in->peek())) GetChar();

    RecordStreamPosition();

    if (_in->eof()) return NewToken(tEOF);
    if (!_in->good()) return NewToken(tIOError);

    c = GetChar();
    tokval = c;
    token = tUndefined;

    switch (c) {
      case ':':
        if (_in->peek() == '=') {
          tokval += GetChar();
          token = tAssign;
        } else {
          token = tColon;
        }
        break;

      case '+':
      case '-':
        token = tPlusMinus;
        break;

      case '*':
        token = tMulDiv;
        break;

      case '/':
        if (_in->peek() == '/') {
          do {
            tokval += GetChar();
          } while (_in->good() && (_in->peek() != '\n'));
          token = tComment;
        } else {
          token = tMulDiv;
        }
        break;

      case '|':
        if (_in->peek() == '|') {
          tokval += GetChar();
          token = tOr;
        }
        break;

      case '&':
        if (_in->peek() == '&') {
          tokval += GetChar();
          token = tAnd;
        }
        break;

      case '!':
        token = tNot;
        break;

      case '<':
      case '>':
        if (_in->peek() == '=') tokval += GetChar();
        token = tRelOp;
        break;

      case '=':
      case '#':
        token = tRelOp;
        break;

      case ',':
        token = tComma;
        break;

      case ';':
        token = tSemicolon;
        break;

      case '.':
        token = tDot;
        break;

      case '(':
        token = tLBrak;
        break;

      case ')':
        token = tRBrak;
        break;

      default:
        if (IsAlpha(c)) {
          while (IsIDChar(_in->peek())) tokval += GetChar();

          map<string, EToken>::const_iterator it = keywords.find(tokval);
          if (it != keywords.end()) token = (*it).second;
          else token = tIdent;
        } else
          if (IsNum(c)) {
            while (IsNum(_in->peek())) tokval += GetChar();
            token = tNumber;
          } else {
            tokval = "invalid character '";
            tokval += c;
            tokval += "'";
          }
        break;

    }
  } while (token == tComment);

  return NewToken(token, tokval);
}

char CScanner::GetChar()
{
  char c = _in->get();
  if (c == '\n') { _line++; _char = 1; } else _char++;
  return c;
}

string CScanner::GetChar(int n)
{
  string str;
  for (int i=0; i<n; i++) str += GetChar();
  return str;
}

bool CScanner::IsWhite(char c) const
{
  return ((c == ' ') || (c == '\t') || (c == '\n'));
}

bool CScanner::IsAlpha(char c) const
{
  return ((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (c == '_'));
}

bool CScanner::IsNum(char c) const
{
  return (('0' <= c) && (c <= '9'));
}

bool CScanner::IsIDChar(char c) const
{
  return (IsAlpha(c) || IsNum(c));
}
