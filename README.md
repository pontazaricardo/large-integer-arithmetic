# large-integer-arithmetic
This repository shows how to perform arithmetic operations for large integers (256-bits) in multiple architectures.

In order to generate the disassembly, run the command
```
$ objdump -d ./executable >> executable_disassembly.asm
```

uint64x2_t vpaddlq_u32(uint32x4_t a)