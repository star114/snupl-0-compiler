//
// test11
//
// prime factors
//

module test11;

procedure primefactor(n);
var f: integer;
begin
  if (n < 1) then return
  else
    if (n = 1) then Output(1)
    else
      while (n > 1) do
        f := 2;
        while (f <= n) do
          if (n/f*f = n) then
            Output(f);
            n := n/f
          else
            f := f + 1
          end
        end
      end
    end
  end
end primefactor;

begin
  primefactor(Input())
end test11.
