.code

; =======================================================================================
; ROTATE CARRY LEFT
; =======================================================================================

x64e_asm_rcl8 proc public
    xchg rcx, rdx
    rcl dl, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_rcl8 endp

x64e_asm_rcl16 proc public
    xchg rcx, rdx
    rcl dx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_rcl16 endp

x64e_asm_rcl32 proc public
    xchg rcx, rdx
    rcl edx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_rcl32 endp

x64e_asm_rcl64 proc public
    xchg rcx, rdx
    rcl rdx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_rcl64 endp

; =======================================================================================



; =======================================================================================
; ROTATE CARRY RIGHT
; =======================================================================================

x64e_asm_rcr8 proc public
    xchg rcx, rdx
    rcr dl, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_rcr8 endp

x64e_asm_rcr16 proc public
    xchg rcx, rdx
    rcr dx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_rcr16 endp

x64e_asm_rcr32 proc public
    xchg rcx, rdx
    rcr edx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_rcr32 endp

x64e_asm_rcr64 proc public
    xchg rcx, rdx
    rcr rdx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_rcr64 endp

; =======================================================================================



; =======================================================================================
; ROTATE LEFT
; =======================================================================================

x64e_asm_rol8 proc public
    xchg rcx, rdx
    rol dl, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_rol8 endp

x64e_asm_rol16 proc public
    xchg rcx, rdx
    rol dx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_rol16 endp

x64e_asm_rol32 proc public
    xchg rcx, rdx
    rol edx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_rol32 endp

x64e_asm_rol64 proc public
    xchg rcx, rdx
    rol rdx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_rol64 endp

; =======================================================================================



; =======================================================================================
; ROTATE RIGHT
; =======================================================================================

x64e_asm_ror8 proc public
    xchg rcx, rdx
    ror dl, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_ror8 endp

x64e_asm_ror16 proc public
    xchg rcx, rdx
    ror dx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_ror16 endp

x64e_asm_ror32 proc public
    xchg rcx, rdx
    ror edx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_ror32 endp

x64e_asm_ror64 proc public
    xchg rcx, rdx
    ror rdx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_ror64 endp

; =======================================================================================



; =======================================================================================
; SHIFT ARITHMETIC RIGHT
; =======================================================================================

x64e_asm_sar8 proc public
    xchg rcx, rdx
    sar dl, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_sar8 endp

x64e_asm_sar16 proc public
    xchg rcx, rdx
    sar dx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_sar16 endp

x64e_asm_sar32 proc public
    xchg rcx, rdx
    sar edx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_sar32 endp

x64e_asm_sar64 proc public
    xchg rcx, rdx
    sar rdx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_sar64 endp

; =======================================================================================



; =======================================================================================
; SHIFT LEFT
; =======================================================================================

x64e_asm_shl8 proc public
    xchg rcx, rdx
    shl dl, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_shl8 endp

x64e_asm_shl16 proc public
    xchg rcx, rdx
    shl dx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_shl16 endp

x64e_asm_shl32 proc public
    xchg rcx, rdx
    shl edx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_shl32 endp

x64e_asm_shl64 proc public
    xchg rcx, rdx
    shl rdx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_shl64 endp

; =======================================================================================



; =======================================================================================
; SHIFT RIGHT
; =======================================================================================

x64e_asm_shr8 proc public
    xchg rcx, rdx
    shr dl, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_shr8 endp

x64e_asm_shr16 proc public
    xchg rcx, rdx
    shr dx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_shr16 endp

x64e_asm_shr32 proc public
    xchg rcx, rdx
    shr edx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_shr32 endp

x64e_asm_shr64 proc public
    xchg rcx, rdx
    shr rdx, cl
    pushfq
    pop qword ptr [r8]
    mov rax, rdx
    ret
x64e_asm_shr64 endp

; =======================================================================================



; =======================================================================================
; XOR
; =======================================================================================

x64e_asm_xor8 proc public
    xor cl, dl
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_xor8 endp

x64e_asm_xor16 proc public
    xor cx, dx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_xor16 endp

x64e_asm_xor32 proc public
    xor ecx, edx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_xor32 endp

x64e_asm_xor64 proc public
    xor rcx, rdx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_xor64 endp

; =======================================================================================



; =======================================================================================
; AND
; =======================================================================================

x64e_asm_and8 proc public
    and cl, dl
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_and8 endp

x64e_asm_and16 proc public
    and cx, dx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_and16 endp

x64e_asm_and32 proc public
    and ecx, edx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_and32 endp

x64e_asm_and64 proc public
    and rcx, rdx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_and64 endp

; =======================================================================================



; =======================================================================================
; OR
; =======================================================================================

x64e_asm_or8 proc public
    or cl, dl
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_or8 endp

x64e_asm_or16 proc public
    or cx, dx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_or16 endp

x64e_asm_or32 proc public
    or ecx, edx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_or32 endp

x64e_asm_or64 proc public
    or rcx, rdx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_or64 endp

; =======================================================================================



; =======================================================================================
; NOT
; =======================================================================================

x64e_asm_not8 proc public
    not cl
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_not8 endp

x64e_asm_not16 proc public
    not cx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_not16 endp

x64e_asm_not32 proc public
    not ecx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_not32 endp

x64e_asm_not64 proc public
    not rcx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_not64 endp

; =======================================================================================



; =======================================================================================
; NEG
; =======================================================================================

x64e_asm_neg8 proc public
    neg cl
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_neg8 endp

x64e_asm_neg16 proc public
    neg cx
    pushfq
    pop qword ptr [r8]
    ret
x64e_asm_neg16 endp

x64e_asm_neg32 proc public
    neg ecx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_neg32 endp

x64e_asm_neg64 proc public
    neg rcx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_neg64 endp

; =======================================================================================



; =======================================================================================
; SUB
; =======================================================================================

x64e_asm_sub8 proc public
    sub cl, dl
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_sub8 endp

x64e_asm_sub16 proc public
    sub cx, dx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_sub16 endp

x64e_asm_sub32 proc public
    sub ecx, edx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_sub32 endp

x64e_asm_sub64 proc public
    sub rcx, rdx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_sub64 endp

; =======================================================================================



; =======================================================================================
; ADD
; =======================================================================================

x64e_asm_add8 proc public
    add cl, dl
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_add8 endp

x64e_asm_add16 proc public
    add cx, dx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_add16 endp

x64e_asm_add32 proc public
    add ecx, edx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_add32 endp

x64e_asm_add64 proc public
    add rcx, rdx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_add64 endp

; =======================================================================================



; =======================================================================================
; INC
; =======================================================================================

x64e_asm_inc8 proc public
    inc cl
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_inc8 endp

x64e_asm_inc16 proc public
    inc cx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_inc16 endp

x64e_asm_inc32 proc public
    inc ecx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_inc32 endp

x64e_asm_inc64 proc public
    inc rcx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_inc64 endp

; =======================================================================================



; =======================================================================================
; DEC
; =======================================================================================

x64e_asm_dec8 proc public
    dec cl
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_dec8 endp

x64e_asm_dec16 proc public
    dec cx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_dec16 endp

x64e_asm_dec32 proc public
    dec ecx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_dec32 endp

x64e_asm_dec64 proc public
    dec rcx
    pushfq
    pop qword ptr [r8]
    mov rax, rcx
    ret
x64e_asm_dec64 endp

; =======================================================================================



; =======================================================================================
; MUL
; =======================================================================================

x64e_asm_mul8 proc public
    mov al, byte ptr [rdx]
    mul r8b
    pushfq
    pop qword ptr [r9]
    mov word ptr [rcx], ax
    ret
x64e_asm_mul8 endp

x64e_asm_mul16 proc public
    push rdi
    mov rdi, rdx
    mov ax, word ptr [rdi]
    mul r8w
    pushfq
    pop qword ptr [r9]
    mov word ptr [rcx], dx
    mov word ptr [rdi], ax
    pop rdi
    ret
x64e_asm_mul16 endp

x64e_asm_mul32 proc public
    push rdi
    mov rdi, rdx
    mov eax, dword ptr [rdi]
    mul r8d
    pushfq
    pop qword ptr [r9]
    mov dword ptr [rcx], edx
    mov dword ptr [rdi], eax
    pop rdi
    ret
x64e_asm_mul32 endp

x64e_asm_mul64 proc public
    push rdi
    mov rdi, rdx
    mov rax, qword ptr [rdi]
    mul r8
    pushfq
    pop qword ptr [r9]
    mov qword ptr [rcx], rdx
    mov qword ptr [rdi], rax
    pop rdi
    ret
x64e_asm_mul64 endp

; =======================================================================================



; =======================================================================================
; IMUL
; =======================================================================================

x64e_asm_imul8 proc public
    mov al, byte ptr [rdx]
    imul r8b
    pushfq
    pop qword ptr [r9]
    mov word ptr [rcx], ax
    ret
x64e_asm_imul8 endp

x64e_asm_imul16 proc public
    push rdi
    mov rdi, rdx
    mov ax, word ptr [rdi]
    imul r8w
    pushfq
    pop qword ptr [r9]
    mov word ptr [rcx], dx
    mov word ptr [rdi], ax
    pop rdi
    ret
x64e_asm_imul16 endp

x64e_asm_imul32 proc public
    push rdi
    mov rdi, rdx
    mov eax, dword ptr [rdi]
    imul r8d
    pushfq
    pop qword ptr [r9]
    mov dword ptr [rcx], edx
    mov dword ptr [rdi], eax
    pop rdi
    ret
x64e_asm_imul32 endp

x64e_asm_imul64 proc public
    push rdi
    mov rdi, rdx
    mov rax, qword ptr [rdi]
    imul r8
    pushfq
    pop qword ptr [r9]
    mov qword ptr [rcx], rdx
    mov qword ptr [rdi], rax
    pop rdi
    ret
x64e_asm_imul64 endp

; =======================================================================================



; =======================================================================================
; IMULn
; =======================================================================================

x64e_asm_imuln16 proc public
    push rdi
    mov di, word ptr [rcx]
    imul di, dx
    pushfq
    pop qword ptr [r8]
    mov word ptr [rcx], di
    pop rdi
    ret
x64e_asm_imuln16 endp

x64e_asm_imuln32 proc public
    push rdi
    mov edi, dword ptr [rcx]
    imul edi, edx
    pushfq
    pop qword ptr [r8]
    mov dword ptr [rcx], edi
    pop rdi
    ret
x64e_asm_imuln32 endp

x64e_asm_imuln64 proc public
    push rdi
    mov rdi, qword ptr [rcx]
    imul rdi, rdx
    pushfq
    pop qword ptr [r8]
    mov qword ptr [rcx], rdi
    pop rdi
    ret
x64e_asm_imuln64 endp

; =======================================================================================



; =======================================================================================
; BT
; =======================================================================================

x64e_asm_bt16 proc public
    bt cx, dx
    pushfq
    pop qword ptr [r8]
    ret
x64e_asm_bt16 endp

x64e_asm_bt32 proc public
    bt ecx, edx
    pushfq
    pop qword ptr [r8]
    ret
x64e_asm_bt32 endp

x64e_asm_bt64 proc public
    bt rcx, rdx
    pushfq
    pop qword ptr [r8]
    ret
x64e_asm_bt64 endp

; =======================================================================================





















end