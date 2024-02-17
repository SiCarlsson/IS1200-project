  # timetemplate.asm
  # Written 2015 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

  # Modified 2024 by Simon Carlsson and Emma Lindblom 
  
.macro	PUSH (%reg)
	addi	$sp,$sp,-4
	sw	%reg,0($sp)
.end_macro

.macro	POP (%reg)
	lw	%reg,0($sp)
	addi	$sp,$sp,4
.end_macro

	.data
	.align 2
mytime:	.word 0x5957
timstr:	.ascii "text more text lots of text\0"
	.text
main:
	# print timstr
	la	$a0,timstr
	li	$v0,4
	syscall
	nop
	# wait a little
	li	$a0,2
	jal	delay
	nop
	# call tick
	la	$a0,mytime
	jal	tick
	nop
	# call your function time2string
	la	$a0,timstr
	la	$t0,mytime
	lw	$a1,0($t0)
	jal	time2string
	nop
	# print a newline
	li	$a0,10
	li	$v0,11
	syscall
	nop
	# go back and do it all again
	j	main
	nop
# tick: update time pointed to by $a0
tick:	lw	$t0,0($a0)	# get time
	addiu	$t0,$t0,1	# increase
	andi	$t1,$t0,0xf	# check lowest digit
	sltiu	$t2,$t1,0xa	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x6	# adjust lowest digit
	andi	$t1,$t0,0xf0	# check next digit
	sltiu	$t2,$t1,0x60	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa0	# adjust digit
	andi	$t1,$t0,0xf00	# check minute digit
	sltiu	$t2,$t1,0xa00	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x600	# adjust digit
	andi	$t1,$t0,0xf000	# check last digit
	sltiu	$t2,$t1,0x6000	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa000	# adjust last digit
tiend:	sw	$t0,0($a0)	# save updated result
	jr	$ra		# return
	nop

  # you can write your code for subroutine "hexasc" below this line
  #
hexasc: 
	andi $a0, $a0, 0xF 	# Emma Lindblom 2024-01-23
				# med en mask (0xF) plockas de 4 minst signifikanta bitarna och stoppas i $a0

    	ble     $a0, 0x9, hexasc_numbers       	# Simon Carlsson 2024-01-23
    						# om a0 är mindre än 0x9 hoppar koden till hexasc_numbers 	
    						# då det är sifrra och inte en bokstav

    	addi      $v0, $a0, 55                	# Emma Lindblom 2024-01-23
    						# om det är en bokstav görs detta. 
    						# addera a0 och 55 (0x30 + 0x7), och lägg i v0 för att 
    						# anpassa efter ASCII
	
	jr $ra					# Emma Lindblom 2024-01-23
						# hoppar till returadressen i main

hexasc_numbers:
    	addi     $v0,$a0, 48            # Simon Carlsson 2024-01-23
    					# adderar a0 och 48(0x30) och lagrar i v0 för att anpassa efter ASCII
    	
    	jr      $ra			# Simon Carlsson 2024-01-23
    					# hoppar till returadressen i main
    					
delay:
	# void delay( int ms )
	# PUSH($ra)			# void?
	# PUSH($ )			# int ms, vilket register?
	# PUSH($ )			# variabeln i
	li $t6 , 0 			# sätter i till 0

delaywhile:	
	# while (ms > 0)
	slti $t4 , $a0 , 1		# om ms är mindre än 1 sätts temp till 1, annars 0
	bne $t4 , $0, delayend		# om t4 inte är lika med 0 (ms är mindre än 1)
					# , hoppa till delayend 
	# ms = ms - 1
	addi $a0 , $a0 , -1			# adderar ms med -1 och stoppar i ms, motsvarar ms = ms - 1
delayloop:	
	# for( i = 0; i < 4711; i = i + 1 )	konstanten 4711 ska enkelt kunna ändras
	slti $t5 , $ , 4711	 	# om register för i är mindre än 4711 sätts temp till 1, annars 0		
	beq $t5 , $0, delaywhile 	# om temp är lika med 0 hoppa till delaywhile, annars fortsätt
	
	# gör ingenting i for loopen och gör i = i + 1
	nop 				# gör ingenting
	addi $t6 , $t6 , 1			# adderar i + 1 och lägger i register för i (i++)
	
	j delayloop			# hoppar tillbaka till toppen av loopen
delayend:
	# POP($ra)			# om dessa behövs då inget faktiskt görs i funktionen?
	# POP($ )
	# POP($ )
	
	jr $ra

time2string:
	PUSH($ra)			# stoppa undan returadressen då den kommer att ändras varje gång
					# funktionen hexasc körs. Annars kommer den få ett enda 
					# felaktigt värde
	
	PUSH($a0)
	andi $a0, $a1, 0xF
	jal hexasc
	#nop
	POP($a0)
	sb $v0, 4($a0)
	
	PUSH($a0)
	andi $a0, $a1, 0xF0
	srl $a0, $a0, 4
	jal hexasc
	#nop
	POP($a0)
	sb $v0, 3($a0)
	
	li $v0, 0x3A
	sb $v0, 2($a0)
	
	PUSH($a0)
	andi $a0, $a1, 0xF00
	srl $a0, $a0, 8
	jal hexasc
	#nop
	POP($a0)
	sb $v0, 1($a0)
	
	PUSH($a0)
	andi $a0, $a1, 0xF000
	srl $a0, $a0, 12
	jal hexasc
	#nop
	POP($a0)
	sb $v0, 0($a0)
	
	li $v0, 0x00
	sb $v0, 5($a0)
	
	POP($ra)
	jr $ra
	
	
