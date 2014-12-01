//
// test08
//
// fibonacci and factorial
//

module test08;

var a : integer;
    b : boolean;

// fib(n): integer
//
// compute the fibonacci number of n.
// n >= 0
function fib(n): integer;
begin
  if (n <= 1) then
    return n
  else
    return fib(n-1) + fib(n-2)
  end
end fib;

// fact(n): integer
//
// compute the factorial of n.
// n >= 0
function fact(n): integer;
begin
  if (n <= 0) then
    return 0
  else
    if (n <= 1) then
      return n
    else
      return n*fact(n-1)
    end
  end
end fact;

// print(x)
//
// print value x
procedure print(x);
begin
  Output(x)
end print;

begin
  a := Input();
  b := a >= 0;

  // loop until the user enters a number < 0
  while (b = true) do
    print(fib(a));
    print(fact(a));

    a := Input();
    b := a >= 0
  end
end test08.
