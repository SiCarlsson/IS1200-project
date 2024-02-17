.macro	PUSH (%reg)
	addi	$sp,$sp,-4
	sw	%reg,0($sp)
.end_macro

.macro	POP (%reg)
	lw	%reg,0($sp)
	addi	$sp,$sp,4
.end_macro

main:
	# void delay( int ms )
	li $a0 , 5000
	jal delay
	jal stop
	
delay:
	
	li $t6 , 0 			# sätter i till 0

delaywhile:	
	# while (ms > 0)
	slti $t4 , $a0 , 1		# om ms är mindre än 1 sätts temp till 1, annars 0
	bne $t4 , $0, delayend		# om t4 inte är lika med 0 (ms är mindre än 1)
					# , hoppa till delayend 
	# ms = ms - 1
	addi $a0 , $a0 , -1		# adderar ms med -1 och stoppar i ms, motsvarar ms = ms - 1
delayloop:	
	# for( i = 0; i < 4711; i = i + 1 )	konstanten 4711 ska enkelt kunna ändras
	slti $t5 , $t6 , 4711	 	# om register för i är mindre än 4711 sätts temp till 1, annars 0		
	beq $t5 , $0, delaywhile 	# om temp är lika med 0 hoppa till delaywhile, annars fortsätt
	
	# gör ingenting i for loopen och gör i = i + 1
	nop 				# gör ingenting
	addi $t6 , $t6 , 1		# adderar i + 1 och lägger i register för i (i++)
	
	j delayloop			# hoppar tillbaka till toppen av loopen
delayend:
	
	jr $ra 				# avsluta funktionen
	
stop:
	nop