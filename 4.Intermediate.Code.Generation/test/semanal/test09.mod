//////
//// statements still confusing me are denoted by ??# followed by description
//// OK is for statements that are actually okay for the final compiler,
////    or problems that are not supposed to be catched in this phase)
//////
// test module
module m1;
    var x, y, z : integer;
        b1 : boolean;
//        x : boolean;        // ??1: x is declared again in the same scope -> int
    function f1(x, y): integer;
        var fx1, fy1, z : integer; // OK: z is declared again, but its scope is different
//            y : boolean;           // ??2: y is one of the parameters,but declared again -> param?
        begin
//            w := 3;                        // ??3: w is unbound-> find symbol
//            y := y + x / true; // OK: type mismatch -- NEXT PHASE
            z := f1(1,2);
            if(z # 2)
            then
                return 3;
                x := 2    // OK: unreachable statement -- NEXT PHASE
        end
        // OK: no return statement -- NEXT PHASE
    end f1;
    procedure p1(x);
        begin
            x := f1(3,4);    // OK: f1 is declared above
            return //1
            // OK: return statement in procedure -- NEXT PHASE
    end p1;
    procedure p2(p1, f1); // ??4: f1 and p1 are used as parameters -> local variable!
    begin
    end p2;

    // body of m1
    begin
//        x := p1(3); // ??5: procedure call used as expression -> type check?
        f1(3,4); // ??6: function call used as statement -> type check?
//        b1 := f1(1,2); // OK: type mismatch -- NEXT PHASE
//        y := f1(4,5,6,7); // OK: incorrect number of arguments -- NEXT PHASE
//        z := f1(true, false); // OK: parameter type mismatch -- NEXT PHASE
//        b1(4,5,6); // ??7: variable used like procedure -> type check?
//        p1 := 3;    // ??8: procedure used like variable -> type check?
//        p3(1,2)     // ??9: unbound procedure name
//        Input(y);    // ??10: Input is not in symbol table (but presents in fibonacci module)
        Output(y)    // ??11: Output is not in symbol table (but presents in fiboancci module)
    end m1.
//////
