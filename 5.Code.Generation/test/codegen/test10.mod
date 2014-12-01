//
// test10
//
// greatest common divisor
// using the Euclidian algorithm
//

module test10;

var a,b: integer;

// gcd(a,b)
//
// computes the greatest common divisor for a and b
// subtraction-based version
function gcd_iter(a, b): integer;
begin
  while (a # b) do
    if (a > b) then a := a - b
    else b := b - a
    end
  end;
  return a
end gcd_iter;

// gcd(a,b)
//
// computes the greatest common divisor for a and b
// division-based version
function gcd_mod(a, b): integer;
var t: integer;
begin
  while (b # 0) do
    t := b;
    b := a - a/t*t;
    a := t
  end;
  return a
end gcd_mod;

// gcd(a,b)
//
// computes the greatest common divisor for a and b
// recursive version
function gcd_rec(a, b): integer;
begin
  if (b = 0) then return a
  else return gcd_rec(b, a - a/b*b)
  end
end gcd_rec;

begin
  a := Input();
  b := Input();
  Output(gcd_iter(a,b));
  Output(gcd_mod(a,b));
  Output(gcd_rec(a,b))
end test10.
