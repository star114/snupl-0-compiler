//------------------------------------------------------------------------------
/// @brief SnuPL/0 parser
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2013/03/07 Bernhard Egger adapted to SnuPL/0
/// 2014/09/28 Bernhard Egger assignment 2: parser for SnuPL/-1
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

#include <limits.h>
#include <cassert>
#include <vector>
#include <iostream>
#include <exception>

#include "parser.h"
using namespace std;


//------------------------------------------------------------------------------
// CParser
//
CParser::CParser(CScanner *scanner)
{
  _scanner = scanner;
  _module = NULL;
}

CAstNode* CParser::Parse(void)
{
  _abort = false;

  if (_module != NULL) { delete _module; _module = NULL; }

  try {
    if (_scanner != NULL) _module = module();

    if (_module != NULL) {
      CToken t;
      string msg;
      if (!_module->TypeCheck(&t, &msg)) SetError(t, msg);
    }
  } catch (...) {
    _module = NULL;
  }

  return _module;
}

const CToken* CParser::GetErrorToken(void) const
{
  if (_abort) return &_error_token;
  else return NULL;
}

string CParser::GetErrorMessage(void) const
{
  if (_abort) return _message;
  else return "";
}

void CParser::SetError(CToken t, const string message)
{
  _error_token = t;
  _message = message;
  _abort = true;
  throw message;
}

bool CParser::Consume(EToken type, CToken *token)
{
  if (_abort) return false;

  CToken t = _scanner->Get();

  if (t.GetType() != type) {
    SetError(t, "expected '" + CToken::Name(type) + "', got '" +
             t.GetName() + "'");
  }

  if (token != NULL) *token = t;

  return t.GetType() == type;
}

void CParser::InitSymbolTable(CSymtab *s)
{
  CTypeManager *tm = CTypeManager::Get();

  CSymProc *input = new CSymProc("Input", tm->GetInt());

  CSymProc *output = new CSymProc("Output", tm->GetNull());
  output->AddParam(new CSymParam(0, "x", tm->GetInt()));

  s->AddSymbol(input);
  s->AddSymbol(output);
}

CAstModule* CParser::module(void)
{
  //
  // module = "module" ident ";" varDeclaration { subroutineDecl } 
  //  "begin" statSequence "end" ident ".".
  //
  Consume(tModule);
  CToken tModuleName;
  Consume(tIdent, &tModuleName);
  Consume(tSemicolon);

  CAstModule *m = new CAstModule(tModuleName, tModuleName.GetValue());
  vardeclaration(m);

  while(_scanner->Peek().GetType() == tProcedure || _scanner->Peek().GetType() == tFunction) {
    CAstProcedure *p = subroutinedecl(m);
  }

  Consume(tBegin);
  CAstStatement *statseq = NULL;
  statseq = statSequence(m);
  Consume(tEnd);
  CToken tModuleNameEnd;
  Consume(tIdent, &tModuleNameEnd);
 
  //Check Name matched.
  if (0 != tModuleName.GetValue().compare(tModuleNameEnd.GetValue())) SetError(tModuleNameEnd, "Module Name is not matched"); 
  Consume(tDot);

  m->SetStatementSequence(statseq);

  return m;
}

CAstStatement* CParser::statSequence(CAstScope *s)
{
  //
  // statSequence = [ statement { ";" statement } ].
  // statement    = assignment.
  //
  // FIRST(statSequence) = { tNumber }
  // FOLLOW(statSequence) = { tDot }
  //
  CAstStatement *head = NULL;


  EToken tt = _scanner->Peek().GetType();
  if (!(tt == tDot)) {
    CAstStatement *tail = NULL;

    do {
      CToken t;
      EToken tt = _scanner->Peek().GetType();
      CAstStatement *st = NULL;

      switch (tt) {
        // statement ::= assignment
        case tNumber:
          st = assignment(s);
          break;

        default:
          SetError(_scanner->Peek(), "statement expected.");
          break;
      }

      assert(st != NULL);
      if (head == NULL) head = st;
      else tail->SetNext(st);
      tail = st;

      tt = _scanner->Peek().GetType();
      if (tt == tDot) break;

      Consume(tSemicolon);
    } while (!_abort);
  }

  return head;
}

CAstStatAssign* CParser::assignment(CAstScope *s)
{
  //
  // assignment = number ":=" expression.
  //
  CToken t;

  CAstConstant *lhs = number();

  Consume(tAssign, &t);

  CAstExpression *rhs = expression(s);

  return new CAstStatAssign(t, lhs, rhs);
}

CAstStatCall* CParser::procedurecall(CAstScope* s)
{
   //
   // procedurecall = ident "(" [ expression {"," expression} ] ")".
   //

   CToken t;
   CAstFunctionCall* call = NULL;

   //[FIX-ME]

   return new CAstStatCall(t, call);
}

CAstStatReturn* CParser::returnstatement(CAstScope* s)
{
   //
   // returnstatement = "return" [ expression ].
   //
   CToken t;
   CAstExpression* expr = NULL;
    
   //[FIX-ME]

   return new CAstStatReturn(t, s, expr);
}

CAstStatIf* CParser::ifstatement(CAstScope* s)
{
   //
   // ifstatement = "if" "(" expression ")" "then" statSequence
   //              [ "else" statSequence ] "end".
   //
   CToken t;
   CAstExpression* cond = NULL;
   CAstStatement* ifBody = NULL;
   CAstStatement* elseBody = NULL;

   //[FIX-ME]

   return new CAstStatIf(t, cond, ifBody, elseBody);
}

CAstStatWhile* CParser::whilestatement(CAstScope* s)
{
   //
   // whilestatement = "while" "(" expression ")" "do" statSequence "end".
   //
   CToken t;
   CAstExpression* cond = NULL;
   CAstStatement* body = NULL;

   //[FIX-ME]

   return new CAstStatWhile(t, cond, body);
}

CAstStatement* CParser::subroutinebody(CAstScope* s)
{
   //
   // subroutineBody = varDeclaration "begin" statSequence "end".
   //

   CAstStatement* statbody = NULL;

   //[FIX-ME]


   return statbody;

}

CAstExpression* CParser::expression(CAstScope* s)
{
  //
  // expression = simpleexpr [ relOp simpleexpression ].
  //
  CToken t;
  EOperation relop;
  CAstExpression *left = NULL, *right = NULL;
 
  // [FIX-ME]
  left = simpleexpr(s);

  if (_scanner->Peek().GetType() == tRelOp) {
    Consume(tRelOp, &t);
    right = simpleexpr(s);

    if (t.GetValue() == "=")       relop = opEqual;
    else if (t.GetValue() == "#")  relop = opNotEqual;
    else SetError(t, "invalid relation.");

    return new CAstBinaryOp(t, relop, left, right);
  } else {
    return left;
  }
}

CAstExpression* CParser::simpleexpr(CAstScope *s)
{
  //
  // simpleexpr ::= term { termOp term }.
  //
  CAstExpression *n = NULL;

  // [FIX-ME]
  n = term(s);

  while (_scanner->Peek().GetType() == tPlusMinus) {
    CToken t;
    CAstExpression *l = n, *r;

    Consume(tPlusMinus, &t);

    r = term(s);

    n = new CAstBinaryOp(t, t.GetValue() == "+" ? opAdd : opSub, l, r);
  }


  return n;
}

CAstExpression* CParser::term(CAstScope *s)
{
  //
  // term = factor { ("*"|"/") factor }.
  //
  CAstExpression *n = NULL;

  // [FIX-ME]
  n = factor(s);

  while (_scanner->Peek().GetType() == tMulDiv) {
    CToken t;
    CAstExpression *l = n, *r;

    Consume(tMulDiv, &t);

    r = factor(s);

    n = new CAstBinaryOp(t, t.GetValue() == "*" ? opMul : opDiv, l, r);
  }

  return n;
}

CAstExpression* CParser::factor(CAstScope *s)
{
  //
  // factor = ident | number | boolean |
  //   "(" expression ")" | subroutineCall | "!" factor.
  // FIRST(factor) = { tIdent, tNumber, tLBrak, tBoolConst, tNot }
  //
  CAstExpression *n = NULL;

  switch (_scanner->Peek().GetType()) {
    // factor ::= number
    case tNumber:
      n = number();
      break;

    // factor ::= "(" expression ")"
    case tLBrak:
      Consume(tLBrak);
      n = expression(s);
      Consume(tRBrak);
      break;
    
    // factor ::= ident | subroutineCall
    case tIdent:
      //[FIX-ME]
      n = ident(s);
      break;

    // factor ::= boolean
    case tBoolConst:
      n = boolean();
      break;

    // factor ::= "!" factor
    case tNot:
      Consume(tNot);
      n = factor(s);
      break;

    default:
      SetError(_scanner->Peek(), "factor expected.");
      break;
  }

  return n;
}

CAstFunctionCall* CParser::functioncall(CAstScope *s)
{
   //
   // subroutineCall = ident "(" [ expression {"," expression} ] ")".
   //
   CToken t;
   CSymProc* pSymproc = new CSymProc(t.GetValue(), NULL); //return type must be set after check s's symtable
   //[FIX-ME]
   return new CAstFunctionCall(t, pSymproc);
}

CAstConstant* CParser::number(void)
{
  //
  // number = digit { digit }.
  //
  // "digit { digit }" is scanned as one token (tNumber)
  //

  CToken t;

  Consume(tNumber, &t);

  errno = 0;
  long long v = strtoll(t.GetValue().c_str(), NULL, 10);
  if (errno != 0) SetError(t, "invalid number.");

  return new CAstConstant(t, CTypeManager::Get()->GetInt(), v);
}

CAstConstant* CParser::boolean(void)
{
  //
  // boolean = "true" | "false"
  //
  // scanned as one token (tBoolConst)
  //

  CToken t;

  Consume(tBoolConst, &t);

  long long v = -1;
  if (0 == t.GetValue().compare("true"))
     v = 1;
  else if (0 == t.GetValue().compare("false"))
     v = 0;
  else
     SetError(t, "invalid boolean type.");

  return new CAstConstant(t, CTypeManager::Get()->GetBool(), v);
}

// [Check]
// CAstType* -> cannot destroy class 
// CAstType's Node is not consist of any other class instance.
CAstType* CParser::type(void)
{
   //
   // type = "integer" | "boolean"
   //
   // scanned as one token (tType)
   //

   CToken t;

   Consume(tType, &t);

   CTypeManager* pTypeManager = CTypeManager::Get();
   if (0 == t.GetValue().compare("integer"))
      return new CAstType(t, pTypeManager->GetInt());
   else if (0 == t.GetValue().compare("boolean"))
      return new CAstType(t, pTypeManager->GetBool());
   else
   {
      SetError(t, "invalid type.");
      return new CAstType(t, pTypeManager->GetNull());
   }
}

CAstDesignator* CParser::ident(CAstScope *s)
{
   //
   // ident = letter { letter | number }
   //
   // scanned as one token (tIdent)
   //

   CToken t;

   Consume(tIdent, &t);

   CSymtab *pSymTab = s->GetSymbolTable();
   const CSymbol* pSymbol = _findsymbol(t.GetValue(), pSymTab);
   if (NULL == pSymbol) SetError(t, "invalid ident.");
   
   return new CAstDesignator(t, pSymbol);
}

CAstProcedure* CParser::subroutinedecl(CAstScope* s)
{
   //
   // subroutinedecl = (procedureDecl | functionDecl) subroutineBody ident ";"
   // procedureDecl = "procedure" ident [ formalParam ] ";".
   // functionDecl = "function" ident [ formalParam ] ":" type ";";
   // formalParam = "(" [ident { "," ident } ] ")".
   //

#define INVALID 0
#define PROCEDURE 1
#define FUNCTION 2

   CAstProcedure* sp = NULL;
   
   CToken tMain, tid;
   int nCheck = INVALID;

   switch (_scanner->Peek().GetType())
   {
        case tProcedure:
            Consume(tProcedure, &tMain);
            nCheck = PROCEDURE;
            break;
        case tFunction:
            Consume(tFunction, &tMain);
            nCheck = FUNCTION;
            break;
        default:
            SetError(_scanner->Peek(), "subroutineDecl expected.");
            break;
   }

   if (INVALID != nCheck)
   {
        Consume(tIdent, &tid);
        vector<CToken> vt;

        if (tLBrak == _scanner->Peek().GetType())
        {
            Consume(tLBrak);
            if (tIdent == _scanner->Peek().GetType())
            {
                vector<CToken> vt;
                _makeidentlist(vt);
            }
            Consume(tRBrak);
        }

        CSymProc* pSymProc = NULL;
        if (PROCEDURE == nCheck)
        {
            Consume(tSemicolon);
            pSymProc = new CSymProc(tid.GetValue(), CTypeManager::Get()->GetNull());
        }
        else if (FUNCTION == nCheck)
        {
            Consume(tColon);
            CAstType* pType = type();
            pSymProc = new CSymProc(tid.GetValue(), pType->GetType());
            Consume(tSemicolon);
        }
        else 
            SetError(tMain, "UNEXPECTED ERROR!!");
         
        sp = new CAstProcedure(tMain, tid.GetValue(), s, pSymProc);

        CSymtab* pSymtab = sp->GetSymbolTable();
        vector<CToken>::const_iterator it = vt.begin();
        int nIndex = 0;
        while (it != vt.end())
        {
            CToken tvar = (*it++);
            string strName = tvar.GetValue();
            const CSymbol* pSymbol = _findsymbol(strName, pSymtab);
            if (NULL == pSymbol) SetError(tvar, "invalid parameter.");

            // [Check]
            CSymbol* pLocalSymbol = sp->CreateVar(strName, pSymbol->GetDataType());
            bool fSuccess = pSymtab->AddSymbol(pLocalSymbol);
            if (fSuccess && pSymProc)
                pSymProc->AddParam(new CSymParam(nIndex++, strName, pSymbol->GetDataType()));
            else
            {
                SetError(tvar, "already exist parameter.");
                delete pLocalSymbol;
            }
        }

        CAstStatement* pStat = subroutinebody(sp);
        sp->SetStatementSequence(pStat);

        CToken tid_tail;
        Consume(tIdent, &tid_tail);
        if (0 != tid.GetValue().compare(tid_tail.GetValue()))
            SetError(tid_tail, "SubroutineDecl Name Match Failed.");
        Consume(tSemicolon);
   }
   return sp;
}

void CParser::vardeclaration(CAstScope* s)
{
   //
   // varDeclaration = [ "var" { ident { "," ident } ":" type ";" } ].
   //

   if (tVar == _scanner->Peek().GetType())
   {
        Consume(tVar);
        
        if (tIdent == _scanner->Peek().GetType())
        {
            vector<CToken> vt;
            _makeidentlist(vt);

            Consume(tColon);

            CAstType* pType = type();

            Consume(tSemicolon);

            CSymtab* pSymtab = s->GetSymbolTable();
            vector<CToken>::const_iterator it = vt.begin();
            while (it != vt.end())
            {
                CToken tvar = (*it++);
                CSymbol* pSymbol = s->CreateVar(tvar.GetValue(), pType->GetType());
                bool fSuccess = pSymtab->AddSymbol(pSymbol);
                if (false == fSuccess)
                {
                    SetError(tvar, "Already Exist variable name");
                    delete pSymbol;
                }
            }
        }
   }
   return;
}

void CParser::_makeidentlist(vector<CToken>& vt)
{
    CToken t;
    Consume(tIdent, &t);
    vt.push_back(t);

    while (tComma == _scanner->Peek().GetType())
    {
        Consume(tComma);
        CToken tid;
        Consume(tIdent, &tid);
        vt.push_back(tid);
    }
}

const CSymbol* CParser::_findsymbol(const string& strName, CSymtab* pSymtab)
{
    const CSymbol* pSymbol = pSymtab->FindSymbol(strName, sLocal);
    if (NULL == pSymbol) return pSymtab->FindSymbol(strName);
    return pSymbol;
}
