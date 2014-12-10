//------------------------------------------------------------------------------
/// @brief SnuPL backend
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/11/28 Bernhard Egger created
/// 2013/06/09 Bernhard Egger adapted to SnuPL/0
///
/// @section license_section License
/// Copyright (c) 2012-2014 Bernhard Egger
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

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>

#include "backend.h"
using namespace std;


//------------------------------------------------------------------------------
// CBackend
//
CBackend::CBackend(ostream &out)
  : _out(out)
{
}

CBackend::~CBackend(void)
{
}

bool CBackend::Emit(CModule *m)
{
  assert(m != NULL);
  _m = m;

  if (!_out.good()) return false;

  bool res = true;

  try {
    EmitHeader();
    EmitCode();
    EmitData();
    EmitFooter();

    res = _out.good();
  } catch (...) {
    res = false;
  }

  return res;
}

void CBackend::EmitHeader(void)
{
}

void CBackend::EmitCode(void)
{
}

void CBackend::EmitData(void)
{
}

void CBackend::EmitFooter(void)
{
}


//------------------------------------------------------------------------------
// CBackendx86
//
CBackendx86::CBackendx86(ostream &out)
: CBackend(out), _curr_scope(NULL)
{
    _ind = string(4, ' ');
}

CBackendx86::~CBackendx86(void)
{
}

void CBackendx86::EmitHeader(void)
{
    _out << "##################################################" << endl
        << "# " << _m->GetName() << endl
        << "#" << endl
        << endl;
}

void CBackendx86::EmitCode(void)
{
    _out << _ind << "#-----------------------------------------" << endl
        << _ind << "# text section" << endl
        << _ind << "#" << endl
        << _ind << ".text" << endl
        << _ind << ".align 4" << endl
        << endl
        << _ind << "# entry point and pre-defined functions" << endl
        << _ind << ".global main" << endl
        << _ind << ".extern Input" << endl
        << _ind << ".extern Output" << endl
        << endl;


    // TODO
    // forall s in subscopes do
    //   EmitScope(s)
    // EmitScope(program)

    // Implement
    const vector<CScope*> scopelist = _m->GetSubscopes();
    for (int n = 0; n < scopelist.size() ; n++)
        EmitScope(scopelist[n]);
    EmitScope(_m);


    _out << _ind << "# end of text section" << endl
        << _ind << "#-----------------------------------------" << endl
        << endl;
}

void CBackendx86::EmitData(void)
{
    _out << _ind << "#-----------------------------------------" << endl
        << _ind << "# global data section" << endl
        << _ind << "#" << endl
        << _ind << ".data" << endl
        << _ind << ".align 4" << endl
        << endl;

    EmitGlobalData(_m);

    _out << _ind << "# end of global data section" << endl
        << _ind << "#-----------------------------------------" << endl
        << endl;
}

void CBackendx86::EmitFooter(void)
{
    _out << _ind << ".end" << endl
        << "##################################################" << endl;
}

void CBackendx86::SetScope(CScope *scope)
{
    _curr_scope = scope;
}

CScope* CBackendx86::GetScope(void) const
{
    return _curr_scope;
}

void CBackendx86::EmitScope(CScope *scope)
{
    assert(scope != NULL);

    string label;

    if (scope->GetParent() == NULL) label = "main";
    else label = scope->GetName();

    // label
    _out << _ind << "# scope " << scope->GetName() << endl
        << label << ":" << endl;

    // TODO
    // ComputeStackOffsets(scope)
    //
    // emit function prologue
    //
    // forall i in instructions do
    //   EmitInstruction(i)
    //
    // emit function epilogue

    // Implement
    // ComputeStackOffsets
    SetScope(scope);
    CSymtab* st = scope->GetSymbolTable();
    size_t size = ComputeStackOffsets(st, 8, -12);

    // prologue
    EmitInstruction("pushl", "%ebp");
    EmitInstruction("movl", "%esp, %ebp");
    EmitInstruction("pushl", "%ebx");
    EmitInstruction("pushl", "%esi");
    EmitInstruction("pushl", "%edi");
    EmitInstruction("subl", Imm(size) + ", %esp", "allocate space for locals");

    _out << endl;

    // EmitInstruction for each CTacInstr
    CCodeBlock* code_block = scope->GetCodeBlock();
    if ( NULL != code_block )
        EmitCodeBlock(code_block, st);

    // [To-do] label?
    // label
    _out << endl << Label("exit") << ":" << endl;

    // epilogue
    EmitInstruction("addl", Imm(size) + ", %esp", "remove locals");
    EmitInstruction("popl", "%edi");
    EmitInstruction("popl", "%esi");
    EmitInstruction("popl", "%ebx");
    EmitInstruction("popl", "%ebp");
    EmitInstruction("ret");

    _out << endl;
}

void CBackendx86::EmitGlobalData(CScope *scope)
{
    assert(scope != NULL);
    
    // TODO
    // forall globals g in scope's symbol table do
    //   emit g respecting its alignment restrictions

    bool fHeader = true;

    CSymtab* st = scope->GetSymbolTable();
    vector<CSymbol*> slist = st->GetSymbols();
    for (size_t i=0; i<slist.size(); i++)
    {
        CSymbol* s = slist[i];
        const CType* t = s->GetDataType();

        if(s->GetSymbolType() == stGlobal)
        { 
            if (fHeader)
            {
                fHeader = false;
                _out << _ind << "# scope : " << scope->GetName() << endl;
            }
            //[fix-me] int / bool are the same as 4 byte?? 
            _out << s->GetName() << ":" << _ind <<  ".skip" << _ind << "4" << _ind <<    
                (t->Match(CTypeManager::Get()->GetInt())?
                ("#<int>") 
                : ("#<bool>")) << endl;
        }
    }
    
    // emit globals in subscopes (necessary if we support static local variables)
    vector<CScope*>::const_iterator sit = scope->GetSubscopes().begin();
    while (sit != scope->GetSubscopes().end()) EmitGlobalData(*sit++);

    _out << endl;
}

void CBackendx86::EmitCodeBlock(CCodeBlock *cb, CSymtab *symtab)
{
    assert(cb != NULL);
    assert(symtab != NULL);

    const list<CTacInstr*> &instr = cb->GetInstr();
    list<CTacInstr*>::const_iterator it = instr.begin();

    while (it != instr.end()) EmitInstruction(*it++, symtab);
}

void CBackendx86::EmitInstruction(CTacInstr *i, CSymtab *symtab)
{
    assert(i != NULL);
    assert(symtab != NULL);

    ostringstream cmt;
    cmt << i;

    EOperation op = i->GetOperation();
    switch (op) {
        // binary operators
        // dst = src1 op src2
        // TODO
        case opAdd:
        case opSub:
        case opMul:
        case opDiv:
        case opAnd:
        case opOr:
            {
            CTac* destination = i->GetDest();
            CTacAddr* src1 = i->GetSrc(1);
            CTacAddr* src2 = i->GetSrc(2);
            EmitInstruction("movl", Operand(src1) + ", %eax", cmt.str());
            if ( opAdd == op )
                EmitInstruction("addl",Operand(src2) + ", %eax");
            else if ( opSub == op)
                EmitInstruction("subl",Operand(src2) + ", %eax");
            else if ( opMul == op)
            {
                //[fix-me] if idivl using %ebx, imull also use %ebx?
                //EmitInstruction("movl", Operand(src2) + ", %ebx");
                //EmitInstruction("imull", "%ebx");
                EmitInstruction("imull",Operand(src2));
            }
            else if ( opDiv == op)
            {
                EmitInstruction("cdq");
                //[fix-me] in reference, Operand(src2) -> %ebx : idivl %ebx ?? why??
                EmitInstruction("movl", Operand(src2) + ", %ebx");
                EmitInstruction("idivl", "%ebx");
                //EmitInstruction("idivl",Operand(src2));
            }
            else if (opAnd == op)
                EmitInstruction("andl", Operand(src2) + ", %eax");
            else if (opOr == op)
                EmitInstruction("orl", Operand(src2) + ", %eax");

            EmitInstruction("movl","%eax, "+ Operand(destination));
            }
            break;
        // unary operators
        // dst = op src1
        // TODO
        case opPos:
        case opNeg:
        case opNot:
            {
            CTac* destination = i->GetDest();
            CTacAddr* src1 = i->GetSrc(1);
            EmitInstruction("movl", Operand(src1) + ", %eax", cmt.str());
            if (opPos == op)
                ; // do nothing.
            else if (opNeg == op)
                EmitInstruction("negl", "%eax");
            else if (opNot == op)
                EmitInstruction("notl", "%eax");

            EmitInstruction("movl", "%eax, "+ Operand(destination));
            }
            break;

        // memory operations
        // dst = src1
        // TODO
        case opAssign:
            {
            CTac* destination = i->GetDest();
            CTacAddr* src1 = i->GetSrc(1);
            EmitInstruction("movl", Operand(src1) + ", %eax", cmt.str());
            EmitInstruction("movl", "%eax, " + Operand(destination));
            }
            break;

        // unconditional branching
        // goto dst
        // TODO
        case opGoto:
            {
            CTac* destination = i->GetDest();
            EmitInstruction("jmp", Operand(destination), cmt.str());
            }
            break;

        // conditional branching
        // if src1 relOp src2 then goto dst
        // TODO
        case opEqual:
        case opNotEqual:
        case opLessThan:
        case opLessEqual:
        case opBiggerThan:
        case opBiggerEqual:
            {
            CTac* destination = i->GetDest();
            CTacAddr* src1 = i->GetSrc(1);
            CTacAddr* src2 = i->GetSrc(2);
            EmitInstruction("movl", Operand(src1) + ", %eax", cmt.str());
            EmitInstruction("cmpl", Operand(src2) + ", %eax");
            EmitInstruction("j" + Condition(op), Operand(destination));
            }
            break;
        // function call-related operations
        // TODO
        case opCall:
            {
            CTac* destination = i->GetDest();
            CTacAddr* src1 = i->GetSrc(1);
            CTacName* proc_name = dynamic_cast<CTacName*>(src1);
            assert(NULL != proc_name);

            EmitInstruction("call", proc_name->GetSymbol()->GetName(), cmt.str());
            const CSymProc* proc = dynamic_cast<const CSymProc*>(proc_name->GetSymbol());
            assert(NULL != proc);
            int nParamCounts = proc->GetNParams();
            if (0 < nParamCounts)
                EmitInstruction("addl", Imm(nParamCounts*4) + ", %esp");
            if (NULL != destination)
                EmitInstruction("movl", "%eax, " + Operand(destination));
            }
            break;
        case opReturn:
            {
            CTacAddr* src1 = i->GetSrc(1);
            if(NULL != src1) 
               EmitInstruction("movl", Operand(src1) + ", %eax", cmt.str()); 
            EmitInstruction("jmp", Label("exit"), (NULL == src1 ) ? cmt.str() : "");
            }
            break;
        case opParam:
            {
            CTac* destination = i->GetDest();
            CTacAddr* src1 = i->GetSrc(1);
            EmitInstruction("movl", Operand(src1) + ", %eax", cmt.str());
            EmitInstruction("pushl", "%eax");
            }
            break;
        // special
        case opLabel:
            _out << Label(dynamic_cast<CTacLabel*>(i)) << ":" << endl;
            break;

        case opNop:
            EmitInstruction("nop", "", cmt.str());
            break;


        default:
            EmitInstruction("# ???", "not implemented", cmt.str());
    }
}

void CBackendx86::EmitInstruction(string mnemonic, string args, string comment)
{
  _out << left
       << _ind
       << setw(7) << mnemonic << " "
       << setw(23) << args;
  if (comment != "") _out << " # " << comment;
  _out << endl;
}

string CBackendx86::Operand(CTac *op) const
{
    string operand;

    // TODO
    // return a string represending op
    CTacName *n;
    CTacConst *c;
    CTacLabel *l;
    
    n = dynamic_cast<CTacName*>(op);
    c = dynamic_cast<CTacConst*>(op);
    l = dynamic_cast<CTacLabel*>(op);
    if (NULL != n)
    {
        const CSymbol* symbol = n->GetSymbol();
        ESymbolType st = symbol->GetSymbolType();
        if (stParam == st || stLocal == st)
        {
            operand = to_string(symbol->GetOffset())
                + "(" + symbol->GetBaseRegister() + ")";
        }
        //else if (stGlobal == st)
        //{
            //[fix-me] global type !
        //}
        else //stProcedure == st
        {
            operand = symbol->GetName();
            
        }
    }
    else if (NULL != c)
    {
        operand = Imm(c->GetValue());
    }
    else if (NULL != l)
    {
        operand = Label(l->GetLabel());
    }

    return operand;
}

string CBackendx86::Imm(int value) const
{
  ostringstream o;
  o << "$" << dec << value;
  return o.str();
}

string CBackendx86::Label(CTacLabel* label) const
{
  CScope *cs = GetScope();
  assert(cs != NULL);

  ostringstream o;
  o << "l_" << cs->GetName() << "_" << label->GetLabel();
  return o.str();
}

string CBackendx86::Label(string label) const
{
  CScope *cs = GetScope();
  assert(cs != NULL);

  return "l_" + cs->GetName() + "_" + label;
}

string CBackendx86::Condition(EOperation cond) const
{
  switch (cond) {
    case opEqual:       return "e";
    case opNotEqual:    return "ne";
    case opLessThan:    return "l";
    case opLessEqual:   return "le";
    case opBiggerThan:  return "g";
    case opBiggerEqual: return "ge";
    default:            assert(false); break;
  }
}


size_t CBackendx86::ComputeStackOffsets(CSymtab *symtab,
                                        int param_ofs,int local_ofs)
{
    assert(symtab != NULL);
    vector<CSymbol*> slist = symtab->GetSymbols();

    // TODO
    size_t size = 0;
    // foreach local symbol l in slist do
    //   compute aligned offset on stack and store in symbol l
    //   set base register to %ebp
    //
    // foreach parameter p in slist do
    //   compute offset on stack and store in symbol p
    //   set base register to %ebp
    //
    // align size

    for (size_t i = 0 ; i < slist.size(); i++)
    {
        CSymbol* symbol = slist[i];
        ESymbolType st = symbol->GetSymbolType();
        if ( stLocal == st )
        {
            const CType* dt = symbol->GetDataType();
            if(dt->Match(CTypeManager::Get()->GetBool()))
            {
                size += 1;
            }
            else if(dt->Match(CTypeManager::Get()->GetInt()))
            {
                AlignSize(size, 4);
                size += 4;
            }
            symbol->SetBaseRegister("%ebp");
            symbol->SetOffset(local_ofs-size);
        }
        else if ( stParam == st)
        {
            CSymParam* param = dynamic_cast<CSymParam*>(symbol);
            symbol->SetBaseRegister("%ebp");
            symbol->SetOffset(param_ofs+4*param->GetIndex());
        }
    }
    
    // align size
    AlignSize(size, 4);

    return size;
}

void CBackendx86::AlignSize(size_t& size, const size_t& align)
{
    if ((size % align) != 0)
        size += (align - (size % align)); // size = (size / align) + 1 
}
