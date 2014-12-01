#-------------------------------------------------------------------------------
#// @brief SnuPL I/O library
#// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
#// @section changelog Change Log
#// 2012/10/12 Bernhard Egger created
#//
#// @section license_section License
#// Copyright (c) 2012, Bernhard Egger
#// All rights reserved.
#//
#// Redistribution and use in source and binary forms,  with or without modifi-
#// cation, are permitted provided that the following conditions are met:
#//
#// - Redistributions of source code must retain the above copyright notice,
#//   this list of conditions and the following disclaimer.
#// - Redistributions in binary form must reproduce the above copyright notice,
#//   this list of conditions and the following disclaimer in the documentation
#//   and/or other materials provided with the distribution.
#//
#// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE
#// IMPLIED WARRANTIES OF MERCHANTABILITY  AND FITNESS FOR A PARTICULAR PURPOSE
#// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER  OR CONTRIBUTORS BE
#// LIABLE FOR ANY DIRECT,  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSE-
#// QUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE
#// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT
#// LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY
#// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
#// DAMAGE.
#-------------------------------------------------------------------------------

  .text
  .align 4

.global Input
.global Output

#-------------------------------------------------------------------------------
# function Input()
# (C: int Input(void))
#
# reads a (decimal) value string from stdin and returns it a signed integer
#
# Reads strings in the format ["-"] digit {digit}.
# Recognizes newline, space and tab as number separators.
# Illegal and superfluous characters are skipped.
# Does not check for overflow.
#
# IA32 Linux calling convention (result = %eax)
Input:
  push    %ebp
  movl    %esp, %ebp
  push    %ebx
  push    %esi
  push    %edi
  subl    $44, %esp             # align at 32-byte boundary

  leal    -12(%ebp), %ecx       # pointer to input string. We need at least an
                                # 11-char buffer to parse '-2147483648'
  leal    -1(%ebp), %esi        # last character of input buffer

.Lread:
  movl    $1, %edx              # %edx = number of characters to read
                                # %ecx = pointer to buffer
  movl    $0, %ebx              # %ebx = stdin
  movl    $3, %eax              # %eax = 3 (read syscall)
  int     $0x80                 # syscall

  test    %eax, %eax            # end of input?
  je      .Lscan

  movb    (%ecx), %al           # load character into %al
  cmpb    $0xa, %al             # early-exit on newlines before increasing %ecx
  je      .Lskip

  inc     %ecx
  cmp     %ecx, %esi            # buffer full?
  je      .Lskip

  cmpb    $0x2d, %al            # '-'?
  je      .Lread

  subb    $0x30, %al            # convert to number
  cmpb    $10, %al              # valid digit?
  jb      .Lread

  dec     %ecx                  # character not valid -> discard

.Lskip:
  movl    %ecx, %esi            # save %ecx

.Lskiploop:
  movb    (%ecx), %al           # get last read character into %al

  cmpb    $0xa, %al             # newline ?
  je      .Lskipdone
  cmpb    $0x9, %al             # tab?
  je      .Lskipdone
  cmpb    $0x20, %al            # space ?
  je      .Lskipdone

  movl    $1, %edx              # %edx = number of characters to read
  leal    -16(%ebp), %ecx       # %ecx = pointer to (scratch) buffer
  movl    $0, %ebx              # %ebx = stdin
  movl    $3, %eax              # %eax = 3 (read syscall)
  int     $0x80                 # syscall

  test    %eax, %eax            # end of input?
  je      .Lskipdone
  jmp     .Lskiploop

.Lskipdone:
  movl    %esi, %ecx            # restore %ecx

.Lscan:
  movb    $0, (%ecx)            # terminate input buffer


  movl    $0, %eax              # accumulated number
  xorl    %ebx, %ebx            # negative flag
  leal    -12(%ebp), %ecx       # input string to parse

  mov     $10, %edi             # base multiplicator

  cmpb    $0x2d, (%ecx)         # first character a '-'?
  jne     .Lscanloop

  mov     $1, %ebx              # set negative flag
  inc     %ecx

.Lscanloop:
  movzbl  (%ecx), %esi          # read character
  inc     %ecx

  subl    $0x30, %esi           # conver to number
  cmpl    $9, %esi              # valid digit?
  ja      .Lscandone

  mull    %edi                  # new value = 10 * old value
  addl    %esi, %eax            #             + digit

  jmp     .Lscanloop

.Lscandone:
  test    %ebx, %ebx            # negative?
  je      .Lexit

  neg     %eax                  # negate

.Lexit:
  addl    $44, %esp
  popl    %edi
  popl    %esi
  popl    %ebx
  popl    %ebp
  ret


#-------------------------------------------------------------------------------
# procedure Output(value)
# (C: void Output(int value))
#
# prints a signed integer value in decimal notation to stdout
#
# IA32 Linux calling convention (1st parameter @ 4(%esp) on entry)
Output:
  push    %ebp
  movl    %esp, %ebp
  push    %ebx
  push    %esi
  subl    $16, %esp             # align at 32-byte boundary

  movl    8(%ebp), %eax         # value in %eax
  movl    $10, %ebx             # base divident in %ebx
  leal    -10(%ebp), %ecx       # pointer to result string in %ecx
                                # initial pos: 2 bytes below the saved
                                # value of %esi on the stack

  movb    $0x0, 1(%ecx)         # insert end of string
  movb    $0xa, (%ecx)          # insert newline

  movl    %eax, %esi
  shr     $31, %esi             # sign flag in %esi

  je      .Lloop                # if positive goto Lloop
  negl    %eax                  # otherwise negate value first

.Lloop:
  xorl    %edx, %edx            # since %eax is positive
  divl    %ebx                  # divide value by base
  addl    $0x30, %edx           # add '0' to remainder

  dec     %ecx
  movb    %dl, (%ecx)           # add digit to result string

  test    %eax, %eax            # more digits?
  jne     .Lloop

  test    %esi, %esi            # sign set?
  je      .Lprint

  dec     %ecx
  movb    $0x2d, (%ecx)         # output '-' sign

.Lprint:
  leal    -8(%ebp), %edx
  subl    %ecx, %edx            # %edx = number of characters (= %ebp-8 - %ecx)
                                # %ecx = pointer to string (already set)
  movl    $1, %ebx              # %ebx = stdout, %ecx = string buffer
  movl    $4, %eax              # %eax = 4 (write syscall)
  int     $0x80                 # syscall

  addl    $16, %esp
  popl    %esi
  popl    %ebx
  popl    %ebp
  ret
