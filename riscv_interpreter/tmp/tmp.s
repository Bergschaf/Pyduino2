.global _start
.section .text
_start:addi x1, x1, 1234
amoswap.w x1, x1, 0(x0)
lw x2, 0(x0)
ebreak
