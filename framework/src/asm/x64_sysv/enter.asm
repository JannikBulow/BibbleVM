default rel

struc Frame
    .vm resq 1
    .registers resq 1
    .frame resq 1
    .task resq 1
    .code resq 1
endstruc

struc LeaveRegisters
    .exit0 resq 1
    .exit1 resq 1
    .exit2 resq 1
endstruc

section .text

global bibblevm_enter

; rdi : pointer to const Frame
; rsi : pointer to LeaveRegisters
bibblevm_enter:
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    push rsi

    mov r14, rdi
    mov r15, [rdi + Frame.registers]

    sub rsp, 8
    call [rdi + Frame.code]
    add rsp, 8

    pop rsi

    mov [rsi + LeaveRegisters.exit0], rax
    mov [rsi + LeaveRegisters.exit1], rcx
    mov [rsi + LeaveRegisters.exit2], rdx

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    ret