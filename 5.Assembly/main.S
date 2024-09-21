' Arch: RISC-V 64

max_element:
    xor t0, t0, t0

    xor t1, t1, t1
.loop:
    beq t1, a1, .exit

    lw t2, t1(a0)
    bgeu t2, t0, .setmax
    j .nextiter
.setmax
    mv t0, t2
.nextiter
    addi t1, t1, 1
    j .loop

.exit:
    mv a0, t0
    ret