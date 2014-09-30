//
// test06
//
// parser test:
// - integer & boolean, binary and unary expressions
//
module test06;

var a,b,c: integer;
    b0,b1: boolean;

procedure foo(a);
var b: integer;
begin
  b := c;
  c := (a+b)
end foo;

procedure unary();
begin
  a := -b;
  b0 := !b1
end unary;

function binop(p0, p1): boolean;
begin
  return !((-p0+p1 > 0) || (p0 > 0) && (p1 < 0))
end binop;

function intop(p0, p1): integer;
begin
  return -p0 + (+p1)
end intop;

function compare(a, b): boolean;
begin
  if (binop(a, b) && (intop(b, a) < a) && (a # b)) then
    return true
  else
    return false
  end
end compare;

begin
end test06.
