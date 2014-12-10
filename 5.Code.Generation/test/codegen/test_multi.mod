module il_test_shortcircuit_multi4;

var p,q,r,s: integer;

begin
	p := Input();
	q := Input();
	r := Input();
	s := Input();

	if ((p = 0) && (q = 0) && (r = 0) || (s = 0)) then
		Output(1)
	end;

	if (!((p = 0) && (q = 0) && (r = 0) || (s = 0))) then
		// do nothing
	else
		Output(1)
	end;

	if (!((p = 0) && (q = 0) && (r = 0) || (s = 0))) then
		Output(1)
	end;

	if ((p = 0) && (q = 0) && (r = 0) || (s = 0)) then
		// do nothing
	else
		Output(1)
	end

end il_test_shortcircuit_multi4.
