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
  CToken tModuleNameTail;
  Consume(tIdent, &tModuleNameTail);
 
  //Check Name matched.
  if (tModuleName.GetValue() != tModuleNameTail.GetValue()) 
     SetError(tModuleNameTail, "Module Name is not matched"); 
  Consume(tDot);

  m->SetStatementSequence(statseq);

  return m;
}

CAstStatement* CParser::statSequence(CAstScope *s)
{
  //
  // statSequence = [ statement { ";" statement } ].
  //
  // FIRST(statSequence) = { e, tIdent, tIf, tWhile, tReturn }
  // FOLLOW(statSequence) = { tEnd, tElse }
  //
  CAstStatement *head = NULL;


  EToken tt = _scanner->Peek().GetType();
  if (tt != tEnd && tt != tElse) {
    CAstStatement *tail = NULL;

    do {
      CToken t;
      CAstStatement *st = statement(s);

      assert(st != NULL);
      if (head == NULL) head = st;
      else tail->SetNext(st);
      tail = st;

      tt = _scanner->Peek().GetType();
      if (tt == tEnd || tt == tElse) break;

      Consume(tSemicolon);
    } while (!_abort);
  }

  return head;
}

CAstStatement* CParser::statement(CAstScope *s)
{
    // statement = assignment | subroutineCall | ifStatement 
    //          | whileStatement | returnStatement.
    // FIRST(statement) = { tIdent, tIf, tWhile, tReturn}
    // 
    CAstStatement *st = NULL;
    
    EToken et = _scanner->Peek().GetType();
    switch (et) {
    // statement ::= assignment || subroutineCall
    case tIdent:
    {
        CToken t;
        EToken tt;
        Consume(tIdent, &t);
        tt = _scanner->Peek().GetType();
        if (tLBrak == tt)
            st = procedurecall(t, s);
        else if (tAssign == tt)
            st = assignment(t, s);
        else SetError(_scanner->Peek(), "statement expected.");
        break;
    }
    // statement ::= ifstatement
    case tIf:
        st = ifstatement(s);
        break;
    // statement ::= whilestatement
    case tWhile:
        st = whilestatement(s);
        break;
    // statement ::= returnstatement
    case tReturn:
        st = returnstatement(s);
        break;

    default:
        SetError(_scanner->Peek(), "statement expected.");
        break;
    }

    return st;
}
CAstStatAssign* CParser::assignment(CToken t, CAstScope *s)
{
  //
  // assignment = ident ":=" expression.
  //
  CToken tAs;

  CAstDesignator *lhs = ident(t, s);

  Consume(tAssign, &tAs);

  CAstExpression *rhs = expression(s);

  return new CAstStatAssign(tAs, lhs, rhs);
}

CAstStatCall* CParser::procedurecall(CToken t, CAstScope* s)
{
   //
   // procedurecall = ident "(" [ expression {"," expression} ] ")".
   //

   CAstFunctionCall* call = functioncall(t, s);

   return new CAstStatCall(t, call);
}

CAstStatReturn* CParser::returnstatement(CAstScope* s)
{
   //
   // returnstatement = "return" [ expression ].
   //
   // FIRST(returnstatement) = {tReturn}
   // FOLLOW(returnstatement) = FOLLOW(statement) = {tSemiColon, tEnd, tElse}
   //
   CToken t;
   CAstExpression* expr = NULL;
   
   Consume(tReturn, &t);
   if (_isexpressionfirst(_scanner->Peek().GetType()))
      expr = expression(s);

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

   Consume(tIf, &t);

   Consume(tLBrak);
   cond = expression(s);
   Consume(tRBrak);
   Consume(tThen);
   ifBody = statSequence(s);
   
   if (_scanner->Peek().GetType() == tElse)
   {
        Consume(tElse);
        elseBody = statSequence(s);
   }
   Consume(tEnd);

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

   Consume(tWhile, &t);

   Consume(tLBrak);
   cond = expression(s);
   Consume(tRBrak);
   
   Consume(tDo);
   body = statSequence(s);
   Consume(tEnd);
   
   return new CAstStatWhile(t, cond, body);
}

CAstStatement* CParser::subroutinebody(CAstScope* s)
{
   //
   // subroutineBody = varDeclaration "begin" statSequence "end".
   //

   CAstStatement* statbody = NULL;

   vardeclaration(s);
   Consume(tBegin);
   statbody = statSequence(s);
   Consume(tEnd);

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
 
  left = simpleexpr(s);

  if (_scanner->Peek().GetType() == tRelOp) {
    Consume(tRelOp, &t);
    right = simpleexpr(s);

    if (t.GetValue() == "=")       relop = opEqual;
    else if (t.GetValue() == "#")  relop = opNotEqual;
    else if (t.GetValue() == "<")  relop = opLessThan;
    else if (t.GetValue() == "<=") relop = opLessEqual;
    else if (t.GetValue() == ">")  relop = opBiggerThan;
    else if (t.GetValue() == ">=") relop = opBiggerEqual;
    else SetError(t, "invalid relation.");

    return new CAstBinaryOp(t, relop, left, right);
  } else {
    return left;
  }
}

CAstExpression* CParser::simpleexpr(CAstScope *s)
{
  //
  // simpleexpr ::= ["+"|"-"] term { ("+"|"-"|"||") term }.
  //
  CAstExpression *n = NULL;

  bool fNeg = false;
  CToken tpm;
  if (tPlusMinus == _scanner->Peek().GetType())
  {
     Consume(tPlusMinus, &tpm);
     if (tpm.GetValue() == "-")
        fNeg = true;
  }
  n = term(s);

  //[Check] how to handle "+"
  if(fNeg)
  {
     CAstExpression *u = n;
     n = new CAstUnaryOp(tpm, opNeg, u);
  }

  EToken et = _scanner->Peek().GetType();
  while (et == tPlusMinus || et == tOr) {
    CToken t;
    CAstExpression *l = n, *r;

    EOperation op;
    switch(et)
    {
    case tPlusMinus:
        Consume(tPlusMinus, &t);
        op = (t.GetValue() == "+" ? opAdd : opSub);
        break;
    case tOr:
        Consume(tOr, &t);
        op = opOr;
        break;
    default:
        SetError(_scanner->Peek(), "Unreachable");
        break;
    }

    r = term(s);

    n = new CAstBinaryOp(t, op, l, r);

    et = _scanner->Peek().GetType();
  }


  return n;
}

CAstExpression* CParser::term(CAstScope *s)
{
  //
  // term = factor { ("*"|"/"|"&&") factor }.
  //
  CAstExpression *n = NULL;

  n = factor(s);

  EToken et = _scanner->Peek().GetType();
  while (et == tMulDiv || et == tAnd) {
    CToken t;
    CAstExpression *l = n, *r;

    EOperation op;
    switch(et)
    {
    case tMulDiv:
        Consume(tMulDiv, &t);
        op = (t.GetValue() == "*" ? opMul : opDiv);
        break;
    case tAnd:
        Consume(tAnd, &t);
        op = opAnd;
        break;
    default:
        SetError(_scanner->Peek(), "Unreachable");
        break;
    }

    r = factor(s);

    n = new CAstBinaryOp(t, op, l, r);
    
    et = _scanner->Peek().GetType();
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
      {
        Consume(tLBrak);
        n = expression(s);
        Consume(tRBrak);
      }
      break;
    
    // factor ::= ident | subroutineCall
    case tIdent:
      {
        CToken t;
        Consume(tIdent, &t);
        if (tLBrak == _scanner->Peek().GetType())
            n = functioncall(t, s);
        else
            n = ident(t, s);
      }
      break;

    // factor ::= boolean
    case tBoolConst:
      n = boolean();
      break;

    // factor ::= "!" factor
    case tNot:
    {
      CToken t;
      Consume(tNot, &t);
      n = factor(s);
      CAstExpression* u = n;
      n = new CAstUnaryOp(t, opNot, u);
      break;
    }
    default:
      SetError(_scanner->Peek(), "factor expected.");
      break;
  }

  return n;
}

CAstFunctionCall* CParser::functioncall(CToken t, CAstScope *s)
{
   //
   // subroutineCall = ident "(" [ expression {"," expression} ] ")".
   //

   CAstFunctionCall* fc = NULL;
   
   const CSymbol* pSymbol = _findsymbol(t.GetValue(), s->GetSymbolTable());
   if (NULL == pSymbol)
        SetError(t, "Cannot find function name.");
   // [Check] to type check?? 
   else if (stProcedure != pSymbol->GetSymbolType())
        SetError(t, "name is not subroutine symbol type");
   else
   {

        Consume(tLBrak);
        vector<CAstExpression*> vae;
        _makeexplist(vae, s);
        Consume(tRBrak);

        CSymProc* pSymProc = new CSymProc(pSymbol->GetName(), pSymbol->GetDataType());
        fc = new CAstFunctionCall(t, pSymProc);

        int nIndex = 0;
        vector<CAstExpression*>::const_iterator it = vae.begin();
        while (it != vae.end())
        {
            CAstExpression* pexp = (*it++);
            fc->AddArg(pexp);
        }
   }

   // [CHECK] to type check??
   // parameter counts check.
   //if (pSymbol->GetNParams() != vae.size())
   //   SetError(t, "function call's parameter counts are not matched with registered.");

   //int nIndex = 0;
   //vector<CAstExpression*>::const_iterator it = vae.begin();
   //while (it != vae.end())
   //{
   //     CAstExpression* pexp = (*it++);
   //     const CSymParam* pSymParam = pSymbol->GetParam(nIndex++);
   //     if (pexp->GetType() != pSymParam->GetDataType())
   //        SetError(pexp.GetToken(), "type check error!");
   //}
   //

   return fc;
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
  if (t.GetValue() == "true")
     v = 1;
  else if (t.GetValue() == "false")
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
   if (t.GetValue() == "integer")
      return new CAstType(t, pTypeManager->GetInt());
   else if (t.GetValue() == "boolean")
      return new CAstType(t, pTypeManager->GetBool());
   else
   {
      SetError(t, "invalid type.");
      return new CAstType(t, pTypeManager->GetNull());
   }
}


CAstDesignator* CParser::ident(CToken t, CAstScope *s)
{
   //
   // ident = letter { letter | number }
   //
   // scanned as one token (tIdent)
   //

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
            _makeidentlist(vt);
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
       
        //Add symbol to global symtab.;
        if (pSymProc)
        {
           // [check] semantic
            s->GetSymbolTable()->AddSymbol(pSymProc);
            //bool fSuccess = s->GetSymbolTable()->AddSymbol(pSymProc);
            //if (false == fSuccess)
            //   SetError(tid, "already exist subroutine name");
        }
 
        sp = new CAstProcedure(tMain, tid.GetValue(), s, pSymProc);

        CSymtab* pSymtab = sp->GetSymbolTable();
        vector<CToken>::const_iterator it = vt.begin();
        int nIndex = 0;
        while (it != vt.end())
        {
            CToken tvar = (*it++);
            string strName = tvar.GetValue();

            // [Check] what is parameter type ?? 
            CSymParam* pSymParam = new CSymParam(nIndex++, strName, 
                  CTypeManager::Get()->GetNull());
            pSymtab->AddSymbol(pSymParam);
            pSymProc->AddParam(pSymParam);

            // [check] semantic
            //bool fSuccess = pSymtab->AddSymbol(pSymParam);
            //if (fSuccess)
            //    pSymProc->AddParam(pSymParam);
            //else
            //    SetError(tvar, "already exist parameter.");
        }
        
        CAstStatement* pStat = subroutinebody(sp);
        sp->SetStatementSequence(pStat);

        CToken tid_tail;
        Consume(tIdent, &tid_tail);
        if (tid.GetValue() != tid_tail.GetValue())
            SetError(tid_tail, "subroutine name is not matched.");
        Consume(tSemicolon);

   }
   return sp;
}

void CParser::vardeclaration(CAstScope* s)
{
   //
   // varDeclaration = [ "var" { ident { "," ident } ":" type ";" } ].
   // 
   // FOLLOW(varDeclaration) = {tBegin, tProcedure, tFunction}
   //

   if (tVar == _scanner->Peek().GetType())
   {
        Consume(tVar);
        
        EToken tt = _scanner->Peek().GetType();

        if ( tBegin != tt && tProcedure != tt && tFunction != tt)
        {
            while ( tIdent == _scanner->Peek().GetType() )
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
                    pSymtab->AddSymbol(pSymbol);

                    //[check] semantic
                    //bool fSuccess = pSymtab->AddSymbol(pSymbol);
                    //if (false == fSuccess)
                    //{
                    //    SetError(tvar, "Already Exist variable name");
                    //    delete pSymbol;
                    //}
                }
            }
        }
   }
   return;
}

bool CParser::_isexpressionfirst(EToken et)
{
    // FIRST(expression) = {tIdent, tNumber, tBoolConst, tLBrak, tNot, tPlusMinus}
    return ( tIdent == et 
          || tNumber == et 
          || tBoolConst == et 
          || tLBrak == et 
          || tNot == et 
          || tPlusMinus == et
          );
}


void CParser::_makeexplist(vector<CAstExpression*>& vae, CAstScope* s)
{
    EToken tt = _scanner->Peek().GetType();

    if(_isexpressionfirst(tt))
    {
        do
        {
            CAstExpression* pexp = expression(s);

            if (NULL != pexp)
                vae.push_back(pexp);
            else
            {
                SetError(_scanner->Peek(), "unexpected Expression");
                break;   
            }
            
            tt = _scanner->Peek().GetType();
            if (tt != tComma) break;
            Consume(tComma);
        }
        while (!_abort);
    }

    return;
}

void CParser::_makeidentlist(vector<CToken>& vt)
{
    EToken tt = _scanner->Peek().GetType();
    if (tt == tIdent)
    {
        do
        {
            CToken t;
            Consume(tIdent, &t);
            vt.push_back(t);

            tt = _scanner->Peek().GetType();
            if (tt != tComma) break;
            Consume(tComma);
        } while (!_abort);
    }
    return;
}

const CSymbol* CParser::_findsymbol(const string& strName, CSymtab* pSymtab)
{
    const CSymbol* pSymbol = pSymtab->FindSymbol(strName, sLocal);
    if (NULL == pSymbol) return pSymtab->FindSymbol(strName);
    return pSymbol;
}
