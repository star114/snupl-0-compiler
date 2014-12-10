##################################################
# mine
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

    # scope mine
main:
    pushl   %ebp                   
    movl    %esp, %ebp             
    pushl   %ebx                   
    pushl   %esi                   
    pushl   %edi                   
    subl    $0, %esp                # allocate space for locals

    movl    $1, %eax                #   0:     assign p <- 1
    movl    %eax, p                
    movl    $2, %eax                #   1:     assign q <- 2
    movl    %eax, q                
    movl    $3, %eax                #   2:     assign r <- 3
    movl    %eax, r                
    movl    p, %eax                 #   3:     assign s <- p
    movl    %eax, s                
    movl    p, %eax                 #   4:     param  0 <- p
    pushl   %eax                   
    call    Output                  #   5:     call   Output
    addl    $4, %esp               
    movl    q, %eax                 #   6:     param  0 <- q
    pushl   %eax                   
    call    Output                  #   7:     call   Output
    addl    $4, %esp               
    movl    r, %eax                 #   8:     param  0 <- r
    pushl   %eax                   
    call    Output                  #   9:     call   Output
    addl    $4, %esp               
    movl    s, %eax                 #  10:     param  0 <- s
    pushl   %eax                   
    call    Output                  #  11:     call   Output
    addl    $4, %esp               

l_mine_exit:
    addl    $0, %esp                # remove locals
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

    # scope : mine
i:    .skip    4    #<bool>
p:    .skip    4    #<int>
q:    .skip    4    #<int>
r:    .skip    4    #<int>
s:    .skip    4    #<int>
w:    .skip    4    #<bool>
x:    .skip    4    #<bool>
z:    .skip    4    #<bool>

    # end of global data section
    #-----------------------------------------

    .end
##################################################
