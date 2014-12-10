##################################################
# il_test_shortcircuit_indirect
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

    # scope is_zero
is_zero:
    pushl   %ebp                   
    movl    %esp, %ebp             
    pushl   %ebx                   
    pushl   %esi                   
    pushl   %edi                   
    subl    $4, %esp                # allocate space for locals
    movl    8(%ebp), %eax           #   0:     if     n = 0 goto 1
    cmpl    $0, %eax               
    je      l_is_zero_1            
    jmp     l_is_zero_2             #   1:     goto   2
l_is_zero_1:
    movl    $1, %eax                #   3:     assign t0 <- 1
    movl    %eax, -13(%ebp)        
    jmp     l_is_zero_3             #   4:     goto   3
l_is_zero_2:
    movl    $0, %eax                #   6:     assign t0 <- 0
    movl    %eax, -13(%ebp)        
l_is_zero_3:
    movl    -13(%ebp), %eax         #   8:     return t0
    jmp     l_is_zero_exit         

l_is_zero_exit:
    addl    $4, %esp                # remove locals
    popl    %edi                   
    popl    %esi                   
    popl    %ebx                   
    popl    %ebp                   
    ret                            

    # scope il_test_shortcircuit_indirect
main:
    pushl   %ebp                   
    movl    %esp, %ebp             
    pushl   %ebx                   
    pushl   %esi                   
    pushl   %edi                   
    subl    $12, %esp               # allocate space for locals
    call    Input                   #   0:     call   t0 <- Input
    movl    %eax, -16(%ebp)        
    movl    -16(%ebp), %eax         #   1:     assign p <- t0
    movl    %eax, p                
    call    Input                   #   2:     call   t1 <- Input
    movl    %eax, -20(%ebp)        
    movl    -20(%ebp), %eax         #   3:     assign q <- t1
    movl    %eax, q                
    movl    p, %eax                 #   4:     param  0 <- p
    pushl   %eax                   
    call    is_zero                 #   5:     call   t2 <- is_zero
    addl    $4, %esp               
    movl    %eax, -21(%ebp)        
    movl    -21(%ebp), %eax         #   6:     if     t2 = 1 goto 5
    cmpl    $1, %eax               
    je      l_il_test_shortcircuit_indirect_5
    jmp     l_il_test_shortcircuit_indirect_4_if_false #   7:     goto   4_if_false
l_il_test_shortcircuit_indirect_5:
    movl    q, %eax                 #   9:     param  0 <- q
    pushl   %eax                   
    call    is_zero                 #  10:     call   t3 <- is_zero
    addl    $4, %esp               
    movl    %eax, -22(%ebp)        
    movl    -22(%ebp), %eax         #  11:     if     t3 = 1 goto 3_if_true
    cmpl    $1, %eax               
    je      l_il_test_shortcircuit_indirect_3_if_true
    jmp     l_il_test_shortcircuit_indirect_4_if_false #  12:     goto   4_if_false
l_il_test_shortcircuit_indirect_3_if_true:
    movl    $0, %eax                #  14:     param  0 <- 0
    pushl   %eax                   
    call    Output                  #  15:     call   Output
    addl    $4, %esp               
    jmp     l_il_test_shortcircuit_indirect_2 #  16:     goto   2
l_il_test_shortcircuit_indirect_4_if_false:
    movl    p, %eax                 #  18:     param  0 <- p
    pushl   %eax                   
    call    is_zero                 #  19:     call   t4 <- is_zero
    addl    $4, %esp               
    movl    %eax, -23(%ebp)        
    movl    -23(%ebp), %eax         #  20:     if     t4 = 1 goto 8_if_true
    cmpl    $1, %eax               
    je      l_il_test_shortcircuit_indirect_8_if_true
    movl    q, %eax                 #  21:     param  0 <- q
    pushl   %eax                   
    call    is_zero                 #  22:     call   t5 <- is_zero
    addl    $4, %esp               
    movl    %eax, -24(%ebp)        
    movl    -24(%ebp), %eax         #  23:     if     t5 = 1 goto 8_if_true
    cmpl    $1, %eax               
    je      l_il_test_shortcircuit_indirect_8_if_true
    jmp     l_il_test_shortcircuit_indirect_9_if_false #  24:     goto   9_if_false
l_il_test_shortcircuit_indirect_8_if_true:
    movl    $1, %eax                #  26:     param  0 <- 1
    pushl   %eax                   
    call    Output                  #  27:     call   Output
    addl    $4, %esp               
    jmp     l_il_test_shortcircuit_indirect_7 #  28:     goto   7
l_il_test_shortcircuit_indirect_9_if_false:
    movl    $2, %eax                #  30:     param  0 <- 2
    pushl   %eax                   
    call    Output                  #  31:     call   Output
    addl    $4, %esp               
l_il_test_shortcircuit_indirect_7:
l_il_test_shortcircuit_indirect_2:

l_il_test_shortcircuit_indirect_exit:
    addl    $12, %esp               # remove locals
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

    # scope : il_test_shortcircuit_indirect
p:    .skip    4    #<int>
q:    .skip    4    #<int>
    # scope : is_zero


    # end of global data section
    #-----------------------------------------

    .end
##################################################
