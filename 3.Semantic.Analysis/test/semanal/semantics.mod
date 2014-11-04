//
// semantics
//
// semantical anaysis
// - constants assignments (range, type, unary operators)
// - variable definitions (use before def, multiple definitions)
// - function parameters (number, type)
// - type checking
//   - expressions: compatible operations/operands
//   - assignments: type of LHS = type of RHS
//   - return statements: correct type
//   - boolean types for conditions

module semantics;


var u,v: boolean;
    x,y: integer;


// nothing wrong here, just a helper procedure
procedure ProcedureCall();
begin
end ProcedureCall;
// constant assignments and unary +/-/! operations
procedure Constants;
var a,b: boolean;
    i,j: integer;
begin
  a := true;                // pass
  a := false;               // pass
  a := !true;               // pass
  a := !!!true;             // pass
  a := !false;              // pass
  a := -true;               // fail
  a := +false;              // fail

  i := -0;                  // pass
  i := +0;                  // pass
  i := --0;                 // fail
  i := +-0;                 // fail
  i := -2147483648;         // pass (min int)
  j :=  2147483647;         // pass (max int)
  i := -2147483649;         // fail (<min int)
  j :=  2147483648;         // fail (>max int)

  a := 0
end Constants;


// variable definitions
procedure UseBeforeDef(k);
var i,j: integer;
begin
  i := j;                   // pass
  i := k;                   // pass
  i := x;                   // pass
  i := z;                   // fail

  i := 0
end UseBeforeDef;


// multiple definitions
procedure MultipleDef(i,i); // fail (parameter - parameter)
var i: integer;             // fail (parameter - local var)
    i: integer;             // fail (local var - local var)
begin
  i := 1
end MultipleDef;


// parameters: too many/few/wrong type of parameters
// each line except 'Parameters(1,2);' contains an error
procedure Parameters(p1, p2);
begin
  Parameters();             // fail
  Parameters(1);            // fail
  Parameters(true, false);  // fail
  Parameters(1,2,3);        // fail
  Parameters(1,2,3,4)       // fail

  Parameters(p2,p1);        // pass
  Parameters(1,2)           // pass
end Parameters;


// type checks
procedure Expressions(p1, p2, p3, p4);
var a,b,c: boolean;
    i,j,k: integer;
begin
  a := 1 + true;            // fail
  a := true + false;        // fail
  a := b + c;               // fail
  a := a > b;               // fail
  a := !!!b;                // pass
  a := a && (!b);           // pass

  i := j + b;               // fail
  i := j && k || p1;        // fail
  i := !j;                  // fail
  i := j + -k;              // fail
  i := j + (-k);            // pass

  a := a && !b && (i < j)   // pass
         && (j < k)
         || (i = k)
end Expressions;


// assignment type checks
procedure Assignments(p1, p2, p3, p4);
var a,b,c: boolean;
    i,j,k: integer;
begin
  a := i;                   // fail
  i := a;                   // fail
  i := ProcedureCall()      // fail
end Assignments;


// return statements type checks
procedure ProcReturn();
begin
  return 5                  // fail
end ProcReturn;

function NoReturn(): integer;
begin
  return                    // fail
end IntReturn;

function IntReturn(): integer;
begin
  return 1 > 2              // fail
end IntReturn;

function BoolReturn(): boolean;
begin
  return 1 + 2              // fail
end BoolReturn;


// condition type checking
procedure If(p1, p2);
begin
  if (p1 + p2) then         // fail
    return
  else
    return
  end;

  if (ProcedureCall()) then // fail
    return
  else
    return
  end
end If;

procedure While();
var a,b: integer;
begin
  while (a) do              // fail
    b := b-1
  end
end While;


begin
  u := v;                   // pass
  x := y+1                  // pass
end semantics.
