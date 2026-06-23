default rel

struc Frame
    .vm resq 1
    .registers resq 1
    .frame resq 1
    .task resq 1
    .resumeCheckpoint resq 1
    .code resq 1
endstruc

struc LeaveRegisters
    .reason resq 1
    .checkpoint resq 1
    .exit1 resq 1
    .exit2 resq 1
endstruc

section .text

global bibblevm_enter

bibblevm_enter:
    push rbx
    push rbp
    push rdi
    push rsi
    push r12
    push r13
    push r14
    push r15

    sub rsp, 168
    movaps [rsp + 0*16], xmm6
    movaps [rsp + 1*16], xmm7
    movaps [rsp + 2*16], xmm8
    movaps [rsp + 3*16], xmm9
    movaps [rsp + 4*16], xmm10
    movaps [rsp + 5*16], xmm11
    movaps [rsp + 6*16], xmm12
    movaps [rsp + 7*16], xmm13
    movaps [rsp + 8*16], xmm14
    movaps [rsp + 9*16], xmm15

    push rdx

    mov r14, rcx
    mov r15, [rcx + Frame.registers]
    mov r13, [rcx + Frame.resumeCheckpoint]

    sub rsp, 40
    call [rcx + Frame.code]
    add rsp, 40

    pop rsi

    movaps xmm6,  [rsp + 0*16]
    movaps xmm7,  [rsp + 1*16]
    movaps xmm8,  [rsp + 2*16]
    movaps xmm9,  [rsp + 3*16]
    movaps xmm10, [rsp + 4*16]
    movaps xmm11, [rsp + 5*16]
    movaps xmm12, [rsp + 6*16]
    movaps xmm13, [rsp + 7*16]
    movaps xmm14, [rsp + 8*16]
    movaps xmm15, [rsp + 9*16]

    add rsp, 168

    mov [rsi + LeaveRegisters.reason], rax
    mov [rsi + LeaveRegisters.checkpoint], rcx
    mov [rsi + LeaveRegisters.exit1], rdx
    mov [rsi + LeaveRegisters.exit2], rbx

    pop r15
    pop r14
    pop r13
    pop r12
    pop rsi
    pop rdi
    pop rbp
    pop rbx
    ret