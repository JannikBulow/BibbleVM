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

    push rdx

    mov r14, rcx
    mov r15, [rcx + Frame.registers]
    mov r13, [rcx + Frame.resumeCheckpoint]

    sub rsp, 40
    call [rcx + Frame.code]
    add rsp, 40

    pop rsi

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