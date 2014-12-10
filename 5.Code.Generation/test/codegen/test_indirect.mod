module il_test_shortcircuit_indirect;

var p,q: integer;

function is_zero(n): boolean;
begin
	return n = 0
end is_zero;

begin
	p := Input();
	q := Input();

	if (is_zero(p) && is_zero(q)) then
		Output(0)
	else
		if (is_zero(p) || is_zero(q)) then
			Output(1)
		else
			Output(2)
		end
	end
end il_test_shortcircuit_indirect.
