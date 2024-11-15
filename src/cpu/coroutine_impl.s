.global _Z12YieldContextPvS_

.text
#if defined(__x86_64__)
_Z12YieldContextPvS_:
    mov %rsp, 0x0(%rdi)
    mov %r15, 0x8(%rdi)
    mov %r14, 0x10(%rdi)
    mov %r13, 0x18(%rdi)
    mov %r12, 0x20(%rdi)
    mov %rbx, 0x28(%rsp)
    mov %rbp, 0x30(%rdi)

    mov 0x0(%rsi), %rsp
    mov 0x8(%rsi), %r15
    mov 0x10(%rsi), %r14
    mov 0x18(%rsi), %r13
    mov 0x20(%rsi), %r12
    mov 0x28(%rsi), %rbx
    mov 0x30(%rsi), %rbp
    ret
#endif
