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

// constant range test
procedure Range;
var a,b,c: integer;
begin
    a := - ( - 2147483647 );
    b := - ( - ( - 2147483648));
    c := + ( - ( + 2147483648))
end Range;

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
//  a := -true;               // fail (semantic assign typecheck)
//  a := +false;              // fail (semantic assign typecheck)

  i := -0;                  // pass
  i := +0;                  // pass
//  i := --0;                 // fail (parser)
//  i := +-0;                 // fail (parser)
  i := -2147483648;         // pass (min int)
  j :=  2147483647//;         // pass (max int)
//  i := -2147483649;         // fail (<min int)
//  j :=  2147483648;         // fail (>max int)

//  a := 0                    // fail (semantic assign typecheck)
end Constants;


// variable definitions
procedure UseBeforeDef(k);
var i,j: integer;
begin
  i := j;                   // pass
  i := k;                   // pass
  i := x;                   // pass
//  i := z;                   // fail (parser)

  i := 0
end UseBeforeDef;


// multiple definitions
//procedure MultipleDef(i,i); // fail (parameter - parameter) (parser)
procedure MultipleDef(i,j);
//var i: integer;             // fail (parameter - local var) (parser) 
//    i: integer;             // fail (local var - local var) (parser)
var k: integer;
    l: integer;
begin
  i := 1
end MultipleDef;


// parameters: too many/few/wrong type of parameters
// each line except 'Parameters(1,2);' contains an error
procedure Parameters(p1, p2);
begin
//  Parameters();             // fail (semantic - parameter counts mismatching)
//  Parameters(1);            // fail (semantic - parameter counts mismatching)
//  Parameters(true, false);  // fail (semantic - parameter type error)
//  Parameters(1,2,3);        // fail (semantic - parameter counts mismatching)
  //Parameters(1,2,3,4)       // fail (parser - semicolon error)
//  Parameters(1,2,3,4);      // fail (semantic - parameter counts mismatching)

  Parameters(p2,p1);        // pass
  Parameters(1,2)           // pass
end Parameters;


// type checks
procedure Expressions(p1, p2, p3, p4);
var a,b,c: boolean;
    i,j,k: integer;
begin
//  a := 1 + true;            // fail
//  a := true + false;        // fail
//  a := b + c;               // fail
//  a := a > b;               // fail
  a := !!!b;                // pass
  a := a && (!b);           // pass

//  i := j + b;               // fail
//  i := j && k || p1;        // fail
//  i := !j;                  // fail
  //i := j + -k;              // fail (parser - need factor not simpleexpr)
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
//  a := i;                   // fail (semantic assign)
//  i := a;                   // fail (semantic assign)
//  i := ProcedureCall()      // fail (semantic assign)
end Assignments;


// return statements type checks
procedure ProcReturn();
begin
    return
//  return 5                  // fail (semantic)
end ProcReturn;

function NoReturn(): integer;
begin
//  return                    // fail (semantic)
//  NoReturn := 1;          // fail (designator must be parameter or variables)
    return 5                //
//end IntReturn;            // fail (names are not matching each other)
end NoReturn;

function IntReturn(): integer;
begin
//  return 1 > 2              // fail
    return 1 + 2
end IntReturn;

function BoolReturn(): boolean;
begin
//  return 1 + 2              // fail (semantic - return type mismatch)
  return 1 > 2            
end BoolReturn;


// condition type checking
procedure If(p1, p2);
begin
//  if (p1 + p2) then         // fail
  if (p1 > p2) then
    return
  else
    return
  end;

//  if (ProcedureCall()) then // fail
  if (BoolReturn()) then
    return
  else
    return
  end
end If;

procedure While();
var a,b: integer;
begin
//  while (a) do              // fail
  while (a < b) do
    b := b-1
  end
end While;


begin
  u := v;                   // pass
  x := y+1                  // pass
end semantics.
