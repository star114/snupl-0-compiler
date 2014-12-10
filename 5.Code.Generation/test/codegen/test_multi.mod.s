##################################################
# il_test_shortcircuit_multi4
#

    #-----------------------------------------
    # text section
    #
    .text
    .align 4

    # entry point and pre-defined functions
    .global main
    .extern Input
    .extern Output

    # scope il_test_shortcircuit_multi4
main:
    pushl   %ebp                   
    movl    %esp, %ebp             
    pushl   %ebx                   
    pushl   %esi                   
    pushl   %edi                   
    subl    $16, %esp               # allocate space for locals

    call    Input                   #   0:     call   t0 <- Input
    movl    %eax, -16(%ebp)        
    movl    -16(%ebp), %eax         #   1:     assign p <- t0
    movl    %eax, p                
    call    Input                   #   2:     call   t1 <- Input
    movl    %eax, -20(%ebp)        
    movl    -20(%ebp), %eax         #   3:     assign q <- t1
    movl    %eax, q                
    call    Input                   #   4:     call   t2 <- Input
    movl    %eax, -24(%ebp)        
    movl    -24(%ebp), %eax         #   5:     assign r <- t2
    movl    %eax, r                
    call    Input                   #   6:     call   t3 <- Input
    movl    %eax, -28(%ebp)        
    movl    -28(%ebp), %eax         #   7:     assign s <- t3
    movl    %eax, s                
    movl    p, %eax                 #   8:     if     p = 0 goto 9
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_9
    jmp     l_il_test_shortcircuit_multi4_7 #   9:     goto   7
l_il_test_shortcircuit_multi4_9:
    movl    q, %eax                 #  11:     if     q = 0 goto 8
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_8
    jmp     l_il_test_shortcircuit_multi4_7 #  12:     goto   7
l_il_test_shortcircuit_multi4_8:
    movl    r, %eax                 #  14:     if     r = 0 goto 5_if_true
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_5_if_true
l_il_test_shortcircuit_multi4_7:
    movl    s, %eax                 #  16:     if     s = 0 goto 5_if_true
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_5_if_true
    jmp     l_il_test_shortcircuit_multi4_6_if_false #  17:     goto   6_if_false
l_il_test_shortcircuit_multi4_5_if_true:
    movl    $1, %eax                #  19:     param  0 <- 1
    pushl   %eax                   
    call    Output                  #  20:     call   Output
    addl    $4, %esp               
    jmp     l_il_test_shortcircuit_multi4_4 #  21:     goto   4
l_il_test_shortcircuit_multi4_6_if_false:
l_il_test_shortcircuit_multi4_4:
    movl    p, %eax                 #  24:     if     p = 0 goto 16
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_16
    jmp     l_il_test_shortcircuit_multi4_14 #  25:     goto   14
l_il_test_shortcircuit_multi4_16:
    movl    q, %eax                 #  27:     if     q = 0 goto 15
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_15
    jmp     l_il_test_shortcircuit_multi4_14 #  28:     goto   14
l_il_test_shortcircuit_multi4_15:
    movl    r, %eax                 #  30:     if     r = 0 goto 13_if_false
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_13_if_false
l_il_test_shortcircuit_multi4_14:
    movl    s, %eax                 #  32:     if     s = 0 goto 13_if_false
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_13_if_false
    jmp     l_il_test_shortcircuit_multi4_11 #  33:     goto   11
l_il_test_shortcircuit_multi4_13_if_false:
    movl    $1, %eax                #  35:     param  0 <- 1
    pushl   %eax                   
    call    Output                  #  36:     call   Output
    addl    $4, %esp               
l_il_test_shortcircuit_multi4_11:
    movl    p, %eax                 #  38:     if     p = 0 goto 23
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_23
    jmp     l_il_test_shortcircuit_multi4_21 #  39:     goto   21
l_il_test_shortcircuit_multi4_23:
    movl    q, %eax                 #  41:     if     q = 0 goto 22
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_22
    jmp     l_il_test_shortcircuit_multi4_21 #  42:     goto   21
l_il_test_shortcircuit_multi4_22:
    movl    r, %eax                 #  44:     if     r = 0 goto 20_if_false
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_20_if_false
l_il_test_shortcircuit_multi4_21:
    movl    s, %eax                 #  46:     if     s = 0 goto 20_if_false
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_20_if_false
    movl    $1, %eax                #  47:     param  0 <- 1
    pushl   %eax                   
    call    Output                  #  48:     call   Output
    addl    $4, %esp               
    jmp     l_il_test_shortcircuit_multi4_18 #  49:     goto   18
l_il_test_shortcircuit_multi4_20_if_false:
l_il_test_shortcircuit_multi4_18:
    movl    p, %eax                 #  52:     if     p = 0 goto 30
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_30
    jmp     l_il_test_shortcircuit_multi4_28 #  53:     goto   28
l_il_test_shortcircuit_multi4_30:
    movl    q, %eax                 #  55:     if     q = 0 goto 29
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_29
    jmp     l_il_test_shortcircuit_multi4_28 #  56:     goto   28
l_il_test_shortcircuit_multi4_29:
    movl    r, %eax                 #  58:     if     r = 0 goto 26_if_true
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_26_if_true
l_il_test_shortcircuit_multi4_28:
    movl    s, %eax                 #  60:     if     s = 0 goto 26_if_true
    cmpl    $0, %eax               
    je      l_il_test_shortcircuit_multi4_26_if_true
    jmp     l_il_test_shortcircuit_multi4_27_if_false #  61:     goto   27_if_false
l_il_test_shortcircuit_multi4_26_if_true:
    jmp     l_il_test_shortcircuit_multi4_25 #  63:     goto   25
l_il_test_shortcircuit_multi4_27_if_false:
    movl    $1, %eax                #  65:     param  0 <- 1
    pushl   %eax                   
    call    Output                  #  66:     call   Output
    addl    $4, %esp               
l_il_test_shortcircuit_multi4_25:

l_il_test_shortcircuit_multi4_exit:
    addl    $16, %esp               # remove locals
    popl    %edi                   
    popl    %esi                   
    popl    %ebx                   
    popl    %ebp                   
    ret                            

    # end of text section
    #-----------------------------------------

    #-----------------------------------------
    # global data section
    #
    .data
    .align 4

    # scope : il_test_shortcircuit_multi4
p:    .skip    4    #<int>
q:    .skip    4    #<int>
r:    .skip    4    #<int>
s:    .skip    4    #<int>

    # end of global data section
    #-----------------------------------------

    .end
##################################################
