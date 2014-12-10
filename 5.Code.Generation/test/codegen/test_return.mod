module il_test_shortcircuit_return;

function func(p, q): boolean;
begin
	return (p > 0) && (Input() = 0) && (q > 0) && func(1, q) && func(p, 1)
end func;

begin
	func(1, 1);
	func(0, 0);
	func(Input(), Input())
end il_test_shortcircuit_return.
