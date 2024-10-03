	.file	"main.cpp"
	.intel_syntax noprefix
	.text
#APP
	.globl _ZSt21ios_base_library_initv
#NO_APP
	.section	.text._ZN4nogl9_mm_loaduEPKi,"axG",@progbits,_ZN4nogl9_mm_loaduEPKi,comdat
	.weak	_ZN4nogl9_mm_loaduEPKi
	.type	_ZN4nogl9_mm_loaduEPKi, @function
_ZN4nogl9_mm_loaduEPKi:
.LFB6849:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	QWORD PTR -24[rbp], rdi
	mov	rax, QWORD PTR -24[rbp]
	mov	QWORD PTR -8[rbp], rax
	mov	rax, QWORD PTR -8[rbp]
	vmovdqu	xmm0, XMMWORD PTR [rax]
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6849:
	.size	_ZN4nogl9_mm_loaduEPKi, .-_ZN4nogl9_mm_loaduEPKi
	.section	.text._ZN4nogl8_mm_loadEPKi,"axG",@progbits,_ZN4nogl8_mm_loadEPKi,comdat
	.weak	_ZN4nogl8_mm_loadEPKi
	.type	_ZN4nogl8_mm_loadEPKi, @function
_ZN4nogl8_mm_loadEPKi:
.LFB6859:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	QWORD PTR -24[rbp], rdi
	mov	rax, QWORD PTR -24[rbp]
	mov	QWORD PTR -8[rbp], rax
	mov	rax, QWORD PTR -8[rbp]
	vmovdqa	xmm0, XMMWORD PTR [rax]
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6859:
	.size	_ZN4nogl8_mm_loadEPKi, .-_ZN4nogl8_mm_loadEPKi
	.section	.text._ZN4nogl10_mm_storeuEDv2_xPi,"axG",@progbits,_ZN4nogl10_mm_storeuEDv2_xPi,comdat
	.weak	_ZN4nogl10_mm_storeuEDv2_xPi
	.type	_ZN4nogl10_mm_storeuEDv2_xPi, @function
_ZN4nogl10_mm_storeuEDv2_xPi:
.LFB6869:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	vmovdqa	XMMWORD PTR -48[rbp], xmm0
	mov	QWORD PTR -56[rbp], rdi
	mov	rax, QWORD PTR -56[rbp]
	mov	QWORD PTR -24[rbp], rax
	vmovdqa	xmm0, XMMWORD PTR -48[rbp]
	vmovdqa	XMMWORD PTR -16[rbp], xmm0
	vmovdqa	xmm0, XMMWORD PTR -16[rbp]
	mov	rax, QWORD PTR -24[rbp]
	vmovdqu	XMMWORD PTR [rax], xmm0
	nop
	nop
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6869:
	.size	_ZN4nogl10_mm_storeuEDv2_xPi, .-_ZN4nogl10_mm_storeuEDv2_xPi
	.section	.text._ZN4nogl9_mm_storeEDv2_xPi,"axG",@progbits,_ZN4nogl9_mm_storeEDv2_xPi,comdat
	.weak	_ZN4nogl9_mm_storeEDv2_xPi
	.type	_ZN4nogl9_mm_storeEDv2_xPi, @function
_ZN4nogl9_mm_storeEDv2_xPi:
.LFB6879:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	vmovdqa	XMMWORD PTR -48[rbp], xmm0
	mov	QWORD PTR -56[rbp], rdi
	mov	rax, QWORD PTR -56[rbp]
	mov	QWORD PTR -24[rbp], rax
	vmovdqa	xmm0, XMMWORD PTR -48[rbp]
	vmovdqa	XMMWORD PTR -16[rbp], xmm0
	mov	rax, QWORD PTR -24[rbp]
	vmovdqa	xmm0, XMMWORD PTR -16[rbp]
	vmovdqa	XMMWORD PTR [rax], xmm0
	nop
	nop
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6879:
	.size	_ZN4nogl9_mm_storeEDv2_xPi, .-_ZN4nogl9_mm_storeEDv2_xPi
	.section	.text._ZN4nogl7_mm_addIiDv2_xEET0_S2_S2_,"axG",@progbits,_ZN4nogl7_mm_addIiDv2_xEET0_S2_S2_,comdat
	.weak	_ZN4nogl7_mm_addIiDv2_xEET0_S2_S2_
	.type	_ZN4nogl7_mm_addIiDv2_xEET0_S2_S2_, @function
_ZN4nogl7_mm_addIiDv2_xEET0_S2_S2_:
.LFB6889:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	vmovdqa	XMMWORD PTR -48[rbp], xmm0
	vmovdqa	XMMWORD PTR -64[rbp], xmm1
	vmovdqa	xmm0, XMMWORD PTR -48[rbp]
	vmovdqa	XMMWORD PTR -32[rbp], xmm0
	vmovdqa	xmm0, XMMWORD PTR -64[rbp]
	vmovdqa	XMMWORD PTR -16[rbp], xmm0
	vmovdqa	xmm1, XMMWORD PTR -32[rbp]
	vmovdqa	xmm0, XMMWORD PTR -16[rbp]
	vpaddd	xmm0, xmm1, xmm0
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6889:
	.size	_ZN4nogl7_mm_addIiDv2_xEET0_S2_S2_, .-_ZN4nogl7_mm_addIiDv2_xEET0_S2_S2_
	.section	.text._ZN4nogl7_mm_subIiDv2_xEET0_S2_S2_,"axG",@progbits,_ZN4nogl7_mm_subIiDv2_xEET0_S2_S2_,comdat
	.weak	_ZN4nogl7_mm_subIiDv2_xEET0_S2_S2_
	.type	_ZN4nogl7_mm_subIiDv2_xEET0_S2_S2_, @function
_ZN4nogl7_mm_subIiDv2_xEET0_S2_S2_:
.LFB6899:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	vmovdqa	XMMWORD PTR -48[rbp], xmm0
	vmovdqa	XMMWORD PTR -64[rbp], xmm1
	vmovdqa	xmm0, XMMWORD PTR -48[rbp]
	vmovdqa	XMMWORD PTR -32[rbp], xmm0
	vmovdqa	xmm0, XMMWORD PTR -64[rbp]
	vmovdqa	XMMWORD PTR -16[rbp], xmm0
	vmovdqa	xmm0, XMMWORD PTR -32[rbp]
	vmovdqa	xmm1, XMMWORD PTR -16[rbp]
	vpsubd	xmm0, xmm0, xmm1
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6899:
	.size	_ZN4nogl7_mm_subIiDv2_xEET0_S2_S2_, .-_ZN4nogl7_mm_subIiDv2_xEET0_S2_S2_
	.section	.text._ZN4nogl8_mm_set1Ei,"axG",@progbits,_ZN4nogl8_mm_set1Ei,comdat
	.weak	_ZN4nogl8_mm_set1Ei
	.type	_ZN4nogl8_mm_set1Ei, @function
_ZN4nogl8_mm_set1Ei:
.LFB6915:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	DWORD PTR -36[rbp], edi
	mov	eax, DWORD PTR -36[rbp]
	mov	DWORD PTR -20[rbp], eax
	mov	eax, DWORD PTR -20[rbp]
	mov	DWORD PTR -16[rbp], eax
	mov	eax, DWORD PTR -20[rbp]
	mov	DWORD PTR -12[rbp], eax
	mov	eax, DWORD PTR -20[rbp]
	mov	DWORD PTR -8[rbp], eax
	mov	eax, DWORD PTR -20[rbp]
	mov	DWORD PTR -4[rbp], eax
	mov	eax, DWORD PTR -16[rbp]
	vmovd	xmm0, DWORD PTR -12[rbp]
	vpinsrd	xmm1, xmm0, eax, 1
	mov	eax, DWORD PTR -8[rbp]
	vmovd	xmm0, DWORD PTR -4[rbp]
	vpinsrd	xmm0, xmm0, eax, 1
	vpunpcklqdq	xmm0, xmm0, xmm1
	nop
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6915:
	.size	_ZN4nogl8_mm_set1Ei, .-_ZN4nogl8_mm_set1Ei
	.section	.text._ZN4nogl11_mm_setzeroIiDv2_xEET0_v,"axG",@progbits,_ZN4nogl11_mm_setzeroIiDv2_xEET0_v,comdat
	.weak	_ZN4nogl11_mm_setzeroIiDv2_xEET0_v
	.type	_ZN4nogl11_mm_setzeroIiDv2_xEET0_v, @function
_ZN4nogl11_mm_setzeroIiDv2_xEET0_v:
.LFB6923:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	vpxor	xmm0, xmm0, xmm0
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6923:
	.size	_ZN4nogl11_mm_setzeroIiDv2_xEET0_v, .-_ZN4nogl11_mm_setzeroIiDv2_xEET0_v
	.section	.rodata
.LC0:
	.string	" 69 "
	.text
	.globl	main
	.type	main, @function
main:
.LFB6945:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 64
	mov	rax, QWORD PTR fs:40
	mov	QWORD PTR -8[rbp], rax
	xor	eax, eax
	mov	DWORD PTR -32[rbp], 69
	mov	DWORD PTR -28[rbp], 420
	mov	DWORD PTR -24[rbp], 69
	mov	DWORD PTR -20[rbp], 420
	lea	rcx, -32[rbp]
	lea	rax, -64[rbp]
	mov	edx, 1
	mov	rsi, rcx
	mov	rdi, rax
	call	_ZN4nogl5AnyMMIiDv2_xEC1EPKib
	lea	rax, -48[rbp]
	mov	esi, 420
	mov	rdi, rax
	call	_ZN4nogl5AnyMMIiDv2_xEC1Ei
	lea	rdx, -48[rbp]
	lea	rax, -64[rbp]
	mov	rsi, rdx
	mov	rdi, rax
	call	_ZN4nogl5AnyMMIiDv2_xEpLERKS2_
	lea	rax, -64[rbp]
	mov	rdi, rax
	call	_ZNK4nogl5AnyMMIiDv2_xEngEv
	vmovdqa	XMMWORD PTR -64[rbp], xmm0
	lea	rcx, -32[rbp]
	lea	rax, -64[rbp]
	mov	edx, 0
	mov	rsi, rcx
	mov	rdi, rax
	call	_ZNK4nogl5AnyMMIiDv2_xE5StoreEPib
	mov	eax, DWORD PTR -32[rbp]
	mov	esi, eax
	lea	rax, _ZSt4cout[rip]
	mov	rdi, rax
	call	_ZNSolsEi@PLT
	mov	rdx, rax
	lea	rax, .LC0[rip]
	mov	rsi, rax
	mov	rdi, rdx
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
	mov	rdx, rax
	mov	eax, DWORD PTR -28[rbp]
	mov	esi, eax
	mov	rdi, rdx
	call	_ZNSolsEi@PLT
	mov	rdx, rax
	lea	rax, .LC0[rip]
	mov	rsi, rax
	mov	rdi, rdx
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
	mov	rdx, rax
	mov	eax, DWORD PTR -24[rbp]
	mov	esi, eax
	mov	rdi, rdx
	call	_ZNSolsEi@PLT
	mov	rdx, rax
	lea	rax, .LC0[rip]
	mov	rsi, rax
	mov	rdi, rdx
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
	mov	rdx, rax
	mov	eax, DWORD PTR -20[rbp]
	mov	esi, eax
	mov	rdi, rdx
	call	_ZNSolsEi@PLT
	mov	esi, 10
	mov	rdi, rax
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_c@PLT
	mov	eax, 0
	mov	rdx, QWORD PTR -8[rbp]
	sub	rdx, QWORD PTR fs:40
	je	.L26
	call	__stack_chk_fail@PLT
.L26:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6945:
	.size	main, .-main
	.section	.text._ZN4nogl5AnyMMIiDv2_xEC2EPKib,"axG",@progbits,_ZN4nogl5AnyMMIiDv2_xEC5EPKib,comdat
	.align 2
	.weak	_ZN4nogl5AnyMMIiDv2_xEC2EPKib
	.type	_ZN4nogl5AnyMMIiDv2_xEC2EPKib, @function
_ZN4nogl5AnyMMIiDv2_xEC2EPKib:
.LFB7201:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 32
	mov	QWORD PTR -8[rbp], rdi
	mov	QWORD PTR -16[rbp], rsi
	mov	eax, edx
	mov	BYTE PTR -20[rbp], al
	cmp	BYTE PTR -20[rbp], 0
	je	.L28
	mov	rax, QWORD PTR -16[rbp]
	mov	rdi, rax
	call	_ZN4nogl8_mm_loadEPKi
	mov	rax, QWORD PTR -8[rbp]
	vmovdqa	XMMWORD PTR [rax], xmm0
	jmp	.L30
.L28:
	mov	rax, QWORD PTR -16[rbp]
	mov	rdi, rax
	call	_ZN4nogl9_mm_loaduEPKi
	mov	rax, QWORD PTR -8[rbp]
	vmovdqa	XMMWORD PTR [rax], xmm0
.L30:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7201:
	.size	_ZN4nogl5AnyMMIiDv2_xEC2EPKib, .-_ZN4nogl5AnyMMIiDv2_xEC2EPKib
	.weak	_ZN4nogl5AnyMMIiDv2_xEC1EPKib
	.set	_ZN4nogl5AnyMMIiDv2_xEC1EPKib,_ZN4nogl5AnyMMIiDv2_xEC2EPKib
	.section	.text._ZN4nogl5AnyMMIiDv2_xEC2Ei,"axG",@progbits,_ZN4nogl5AnyMMIiDv2_xEC5Ei,comdat
	.align 2
	.weak	_ZN4nogl5AnyMMIiDv2_xEC2Ei
	.type	_ZN4nogl5AnyMMIiDv2_xEC2Ei, @function
_ZN4nogl5AnyMMIiDv2_xEC2Ei:
.LFB7204:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 16
	mov	QWORD PTR -8[rbp], rdi
	mov	DWORD PTR -12[rbp], esi
	mov	eax, DWORD PTR -12[rbp]
	mov	edi, eax
	call	_ZN4nogl8_mm_set1Ei
	mov	rax, QWORD PTR -8[rbp]
	vmovdqa	XMMWORD PTR [rax], xmm0
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7204:
	.size	_ZN4nogl5AnyMMIiDv2_xEC2Ei, .-_ZN4nogl5AnyMMIiDv2_xEC2Ei
	.weak	_ZN4nogl5AnyMMIiDv2_xEC1Ei
	.set	_ZN4nogl5AnyMMIiDv2_xEC1Ei,_ZN4nogl5AnyMMIiDv2_xEC2Ei
	.section	.text._ZN4nogl5AnyMMIiDv2_xEpLERKS2_,"axG",@progbits,_ZN4nogl5AnyMMIiDv2_xEpLERKS2_,comdat
	.align 2
	.weak	_ZN4nogl5AnyMMIiDv2_xEpLERKS2_
	.type	_ZN4nogl5AnyMMIiDv2_xEpLERKS2_, @function
_ZN4nogl5AnyMMIiDv2_xEpLERKS2_:
.LFB7206:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 16
	mov	QWORD PTR -8[rbp], rdi
	mov	QWORD PTR -16[rbp], rsi
	mov	rax, QWORD PTR -16[rbp]
	vmovdqa	xmm1, XMMWORD PTR [rax]
	mov	rax, QWORD PTR -8[rbp]
	vmovdqa	xmm0, XMMWORD PTR [rax]
	call	_ZN4nogl7_mm_addIiDv2_xEET0_S2_S2_
	mov	rax, QWORD PTR -8[rbp]
	vmovdqa	XMMWORD PTR [rax], xmm0
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7206:
	.size	_ZN4nogl5AnyMMIiDv2_xEpLERKS2_, .-_ZN4nogl5AnyMMIiDv2_xEpLERKS2_
	.section	.text._ZNK4nogl5AnyMMIiDv2_xEngEv,"axG",@progbits,_ZNK4nogl5AnyMMIiDv2_xEngEv,comdat
	.align 2
	.weak	_ZNK4nogl5AnyMMIiDv2_xEngEv
	.type	_ZNK4nogl5AnyMMIiDv2_xEngEv, @function
_ZNK4nogl5AnyMMIiDv2_xEngEv:
.LFB7207:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 64
	mov	QWORD PTR -56[rbp], rdi
	mov	rax, QWORD PTR fs:40
	mov	QWORD PTR -8[rbp], rax
	xor	eax, eax
	call	_ZN4nogl11_mm_setzeroIiDv2_xEET0_v
	vmovdqa	XMMWORD PTR -32[rbp], xmm0
	mov	rax, QWORD PTR -56[rbp]
	vmovdqa	xmm1, XMMWORD PTR [rax]
	vmovdqa	xmm0, XMMWORD PTR -32[rbp]
	call	_ZN4nogl7_mm_subIiDv2_xEET0_S2_S2_
	lea	rax, -48[rbp]
	mov	rdi, rax
	call	_ZN4nogl5AnyMMIiDv2_xEC1ES1_
	vmovdqa	xmm0, XMMWORD PTR -48[rbp]
	mov	rax, QWORD PTR -8[rbp]
	sub	rax, QWORD PTR fs:40
	je	.L35
	call	__stack_chk_fail@PLT
.L35:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7207:
	.size	_ZNK4nogl5AnyMMIiDv2_xEngEv, .-_ZNK4nogl5AnyMMIiDv2_xEngEv
	.section	.text._ZNK4nogl5AnyMMIiDv2_xE5StoreEPib,"axG",@progbits,_ZNK4nogl5AnyMMIiDv2_xE5StoreEPib,comdat
	.align 2
	.weak	_ZNK4nogl5AnyMMIiDv2_xE5StoreEPib
	.type	_ZNK4nogl5AnyMMIiDv2_xE5StoreEPib, @function
_ZNK4nogl5AnyMMIiDv2_xE5StoreEPib:
.LFB7208:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 32
	mov	QWORD PTR -8[rbp], rdi
	mov	QWORD PTR -16[rbp], rsi
	mov	eax, edx
	mov	BYTE PTR -20[rbp], al
	cmp	BYTE PTR -20[rbp], 0
	je	.L37
	mov	rax, QWORD PTR -8[rbp]
	vmovdqa	xmm0, XMMWORD PTR [rax]
	mov	rax, QWORD PTR -16[rbp]
	mov	rdi, rax
	call	_ZN4nogl9_mm_storeEDv2_xPi
	jmp	.L39
.L37:
	mov	rax, QWORD PTR -8[rbp]
	vmovdqa	xmm0, XMMWORD PTR [rax]
	mov	rax, QWORD PTR -16[rbp]
	mov	rdi, rax
	call	_ZN4nogl10_mm_storeuEDv2_xPi
.L39:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7208:
	.size	_ZNK4nogl5AnyMMIiDv2_xE5StoreEPib, .-_ZNK4nogl5AnyMMIiDv2_xE5StoreEPib
	.section	.text._ZN4nogl5AnyMMIiDv2_xEC2ES1_,"axG",@progbits,_ZN4nogl5AnyMMIiDv2_xEC5ES1_,comdat
	.align 2
	.weak	_ZN4nogl5AnyMMIiDv2_xEC2ES1_
	.type	_ZN4nogl5AnyMMIiDv2_xEC2ES1_, @function
_ZN4nogl5AnyMMIiDv2_xEC2ES1_:
.LFB7311:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	QWORD PTR -8[rbp], rdi
	vmovdqa	XMMWORD PTR -32[rbp], xmm0
	mov	rax, QWORD PTR -8[rbp]
	vmovdqa	xmm0, XMMWORD PTR -32[rbp]
	vmovdqa	XMMWORD PTR [rax], xmm0
	nop
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7311:
	.size	_ZN4nogl5AnyMMIiDv2_xEC2ES1_, .-_ZN4nogl5AnyMMIiDv2_xEC2ES1_
	.weak	_ZN4nogl5AnyMMIiDv2_xEC1ES1_
	.set	_ZN4nogl5AnyMMIiDv2_xEC1ES1_,_ZN4nogl5AnyMMIiDv2_xEC2ES1_
	.section	.rodata
	.type	_ZNSt8__detail30__integer_to_chars_is_unsignedIjEE, @object
	.size	_ZNSt8__detail30__integer_to_chars_is_unsignedIjEE, 1
_ZNSt8__detail30__integer_to_chars_is_unsignedIjEE:
	.byte	1
	.type	_ZNSt8__detail30__integer_to_chars_is_unsignedImEE, @object
	.size	_ZNSt8__detail30__integer_to_chars_is_unsignedImEE, 1
_ZNSt8__detail30__integer_to_chars_is_unsignedImEE:
	.byte	1
	.type	_ZNSt8__detail30__integer_to_chars_is_unsignedIyEE, @object
	.size	_ZNSt8__detail30__integer_to_chars_is_unsignedIyEE, 1
_ZNSt8__detail30__integer_to_chars_is_unsignedIyEE:
	.byte	1
	.ident	"GCC: (Ubuntu 13.2.0-23ubuntu4) 13.2.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
