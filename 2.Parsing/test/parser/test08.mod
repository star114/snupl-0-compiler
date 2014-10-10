//
// fibonacci
//
module fibonacci;
var n: integer;
// fib(n): integer
// compute the fibonacci number of n. n >= 0
function fib(n): integer;
begin
  if (n <= 1) then
      return n
        else
            return fib(n-1) + fib(n-2)
   end 
   end fib;
function Input(): integer;
begin
end Input;
function Output(k) : integer;
begin
end Output;
   
begin
    n := Input();
    // loop until the user enters a number < 0
    while (n > 0) do
        Output(fib(n));
        n := Input()
end
end fibonacci.
