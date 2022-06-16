

.section ".start.text","ax"

start:
	mov x0, #0x8000
	mov x1, #0x41		//A
	strb w1, [x0]
	mov x1, #0x42		//B
	strb w1, [x0]
	mov x1, #0x43		//C
	strb w1, [x0]
	mov x1, #0xa		//\n
	strb w1, [x0]

	ldr x0, =0x101000
	mov sp, x0

	bl  main
	
	ldr x1, =0x10000
	mov x0, #1
	strb w0, [x1]
