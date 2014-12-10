##################################################
# il_test_shortcircuit_return
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

    # scope func
func:
    pushl   %ebp                   
    movl    %esp, %ebp             
    pushl   %ebx                   
    pushl   %esi                   
    pushl   %edi                   
    subl    $8, %esp                # allocate space for locals

    movl    8(%ebp), %eax           #   0:     if     p > 0 goto 7
    cmpl    $0, %eax               
    jg      l_func_7               
    jmp     l_func_2                #   1:     goto   2
l_func_7:
    call    Input                   #   3:     call   t0 <- Input
    movl    %eax, -16(%ebp)        
    movl    -16(%ebp), %eax         #   4:     if     t0 = 0 goto 6
    cmpl    $0, %eax               
    je      l_func_6               
    jmp     l_func_2                #   5:     goto   2
l_func_6:
    movl    12(%ebp), %eax          #   7:     if     q > 0 goto 5
    cmpl    $0, %eax               
    jg      l_func_5               
    jmp     l_func_2                #   8:     goto   2
l_func_5:
    movl    12(%ebp), %eax          #  10:     param  1 <- q
    pushl   %eax                   
    movl    $1, %eax                #  11:     param  0 <- 1
    pushl   %eax                   
    call    func                    #  12:     call   t1 <- func
    addl    $8, %esp               
    movl    %eax, -17(%ebp)        
    movl    -17(%ebp), %eax         #  13:     if     t1 = 1 goto 4
    cmpl    $1, %eax               
    je      l_func_4               
    jmp     l_func_2                #  14:     goto   2
l_func_4:
    movl    $1, %eax                #  16:     param  1 <- 1
    pushl   %eax                   
    movl    8(%ebp), %eax           #  17:     param  0 <- p
    pushl   %eax                   
    call    func                    #  18:     call   t2 <- func
    addl    $8, %esp               
    movl    %eax, -18(%ebp)        
    movl    -18(%ebp), %eax         #  19:     if     t2 = 1 goto 1
    cmpl    $1, %eax               
    je      l_func_1               
    jmp     l_func_2                #  20:     goto   2
l_func_1:
    movl    $1, %eax                #  22:     assign t3 <- 1
    movl    %eax, -19(%ebp)        
    jmp     l_func_3                #  23:     goto   3
l_func_2:
    movl    $0, %eax                #  25:     assign t3 <- 0
    movl    %eax, -19(%ebp)        
l_func_3:
    movl    -19(%ebp), %eax         #  27:     return t3
    jmp     l_func_exit            

l_func_exit:
    addl    $8, %esp                # remove locals
    popl    %edi                   
    popl    %esi                   
    popl    %ebx                   
    popl    %ebp                   
    ret                            

    # scope il_test_shortcircuit_return
main:
    pushl   %ebp                   
    movl    %esp, %ebp             
    pushl   %ebx                   
    pushl   %esi                   
    pushl   %edi                   
    subl    $16, %esp               # allocate space for locals

    movl    $1, %eax                #   0:     param  1 <- 1
    pushl   %eax                   
    movl    $1, %eax                #   1:     param  0 <- 1
    pushl   %eax                   
    call    func                    #   2:     call   t0 <- func
    addl    $8, %esp               
    movl    %eax, -13(%ebp)        
    movl    $0, %eax                #   3:     param  1 <- 0
    pushl   %eax                   
    movl    $0, %eax                #   4:     param  0 <- 0
    pushl   %eax                   
    call    func                    #   5:     call   t1 <- func
    addl    $8, %esp               
    movl    %eax, -14(%ebp)        
    call    Input                   #   6:     call   t2 <- Input
    movl    %eax, -20(%ebp)        
    movl    -20(%ebp), %eax         #   7:     param  1 <- t2
    pushl   %eax                   
    call    Input                   #   8:     call   t3 <- Input
    movl    %eax, -24(%ebp)        
    movl    -24(%ebp), %eax         #   9:     param  0 <- t3
    pushl   %eax                   
    call    func                    #  10:     call   t4 <- func
    addl    $8, %esp               
    movl    %eax, -25(%ebp)        

l_il_test_shortcircuit_return_exit:
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



    # end of global data section
    #-----------------------------------------

    .end
##################################################
