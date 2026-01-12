*
BDOS	equ	0005h
BGETC	equ	01h
BPUTC	equ	02h
BPUTS	equ	09h
*
BIOS	equ	0f200h
WBOOT	equ	BIOS +  0
HOME	equ	BIOS + 21
SELDSK	equ	BIOS + 24
SETTRK	equ	BIOS + 27
SETSEC	equ	BIOS + 30
SETDMA	equ	BIOS + 33
READ	equ	BIOS + 36
WRITE	equ	BIOS + 39
*
*
	org	0100h
*set disk B
	mvi	c,1
	mvi	e,0
	call	SELDSK
*set track 0
	lxi	b,0
	call	SETTRK
*get first 4 blocks
	lxi	b,0
	lxi	h,ENTRY$LIST
	call	getblk		;CPM sector is 128B, Blk is 256, call 8 times
	call	getblk
	call	getblk
	call	getblk
	call	getblk
	call	getblk
	call	getblk
	call	getblk
*print every non-blank entry (max 64)
	mvi	c,64
	lxi	h,ENTRY$LIST
loop$entry:
	call	putc
	call	putc
	call	putc
	call	putc
	call	putc
	call	putc
	call	putc
	call	putc
	call	tab
	call	putword
	call	tab
	call	putword
	call	tab
	call	putbyte
	call	tab
	call	putbyte
	call	putbyte
	call	putbyte
	call	crlf
	dcr	c
	jnz	loop$entry

putc:	push	b
	push	h
	mov	e,m
	mvi	c,BPUTC
	call	BDOS
	pop	h
	pop	b
	inx	h
	ret

putbyte:
	push	b
	push	h
	mov	a,m
	push	psw
	ani	0f0h
	rlc !rlc !rlc !rlc
	adi	'0'
	mov	e,a
	mvi	c,BPUTC
	call	BDOS
	pop	psw
	ani	0fh
	adi	'0'
	mov	e,a
	mvi	c,BPUTC
	call	BDOS
	pop	h
	pop	b
	inx	h
	ret

putword:
	inx	h
	call	putbyte
	dcx	h
	dcx	h
	call	putbyte
	inx	h
	ret

tab:	
	push	b
	push	h
	mvi	c,BPUTC
	mvi	e,09h
	call	bdos
	pop	h
	pop	b
	ret

crlf:	
	push	b
	push	h
	mvi	c,BPUTC
	mvi	e,0ah
	call	bdos
	mvi	c,BPUTC
	mvi	e,0dh
	call	bdos
	pop	h
	pop	b
	ret


getblk:
	push	b	;protect sector
	push	h	;protect dma

	lxi	b,0	;set sector
	call	SETSEC

	pop	b	;restore and push dma
	push	b	
	call	SETDMA	;set dma
	call	READ	;read into buffer

	pop	h	;restore dma
	lxi	b,080h	;128 sector size
	dad	b	;next sector's dma

	pop	b	;sector
	inx	b	;next sector
	ret
*		
*
*
*
*nsdos file entry
*ENTRY:
*ENTRY$NAME:	ds	8
*ENTRY$DADDR:	ds	2
*ENTRY$SIZE:	ds	2
*ENTRY$TYPE:	ds	1
*ENTRY$OTHER:	ds	3
*
*ENTRY$size	equ	10h
*
*ENTRY$MADDR	equ	ENTRY$OTHER
*ENTRY$BVALID	equ	ENTRY$OTHER
*
ENTRY$LIST:	ds	100h
*
* vim: ft=i8080 ts=8 sts=8 sw=8 noet
