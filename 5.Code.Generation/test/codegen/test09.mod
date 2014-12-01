//
// test09
//
// sum of natural numbers
//

module test09;

var a : integer;

// sum(n): integer
//
// sum of values 0...n
// recursive version
function sum_rec(n): integer;
begin
  if (n > 0) then
    return n + sum_rec(n-1)
  else
    return 0
  end
end sum_rec;

// sum(n): integer
//
// sum of values 0...n
// iterative version
function sum_iter(n): integer;
var sum, i: integer;
begin
  sum := 0;
  i := 0;
  while (i <= n) do
    sum := sum + i;
    i := i+1
  end;
  return sum
end sum_iter;

// sum(n): integer
//
// sum of values 0...n
// algorithmic version
function sum_alg(n): integer;
begin
  return n * (n+1) / 2
end sum_alg;

begin
  a := Input();

  // loop until the user enters a number < 0
  while (a >= 0) do
    Output(sum_rec(a));
    Output(sum_iter(a));
    Output(sum_alg(a));

    a := Input()
  end
end test09.
