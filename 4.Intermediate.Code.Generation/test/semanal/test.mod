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

begin
//  u := v;                   // pass
//  x := y+1;                  // pass
//  u := ( u && v ) && ( v && u ); 
  u := true || (1 < 3)
end semantics.
