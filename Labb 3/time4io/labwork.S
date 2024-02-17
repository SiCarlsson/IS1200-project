  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
  
  # Modified 2024 by Simon Carlsson and Emma Lindblom

  # Emma Lindblom 2024-01-29
.global		delay
.global 	time2string
.global 	hexasc
.data
.text

.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #


hexasc: 
	andi 	$a0, $a0, 0xF 	# Emma Lindblom 2024-01-23
				# med en mask (0xF) plockas de 4 minst signifikanta bitarna och stoppas i $a0

    	ble	$a0, 0x9, hexasc_numbers       	# Simon Carlsson 2024-01-23
    						# om a0 är mindre än 0x9 hoppar koden till hexasc_numbers 	
    						# då det är sifrra och inte en bokstav
	nop

    	addi	$v0, $a0, 55                	# Emma Lindblom 2024-01-23
    						# om det är en bokstav görs detta. 
    						# addera a0 och 55 (0x30 + 0x7), och lägg i v0 för att 
    						# anpassa efter ASCII
	
	jr 	$ra					# Emma Lindblom 2024-01-23
						# hoppar till returadressen i main
	nop
	
hexasc_numbers:
    	addi     $v0,$a0, 48            # Simon Carlsson 2024-01-23
    					# adderar a0 och 48(0x30) och lagrar i v0 för att anpassa efter ASCII
    	
    	jr      $ra			# Simon Carlsson 2024-01-23
    					# hoppar till returadressen i main
    	nop
					
delay:
	li 	$t6, 0 			# sätter i till 0
	li	$a0, 3300000		# Simon Carlsson 2024-01-29

delaywhile:	
	# while (ms > 0)
	# Emma Lindblom 2024-01-29
	slti 	$t4, $a0 , 1		# om ms är mindre än 1 sätts t4 till 1, annars 0
	bne 	$t4, $0, delayend	# om t4 inte är lika med 0 (ms är mindre än 1), hoppa till delayend 
	nop
	# ms = ms - 1
	addi 	$a0, $a0 , -1		# adderar ms med -1 och stoppar i ms, motsvarar ms = ms - 1
	
delayloop:	
	# for( i = 0; i < 4711; i = i + 1 )	konstanten 4711 ska enkelt kunna ändras
	# Simon Carlsson 2024-01-29
	slti 	$t5, $t6, 10000	 	# om register för i är mindre än 4711 sätts temp till 1, annars 0		
	beq 	$t5, $0, delaywhile 	# om temp är lika med 0 hoppa till delaywhile, annars fortsätt
	
	# gör ingenting i for loopen och gör i = i + 1
	nop 				# gör ingenting
	addi 	$t6, $t6 , 1			# adderar i + 1 och lägger i register för i (i++)
	
	j 	delayloop			# hoppar tillbaka till toppen av loopen
	nop
	
delayend:
	# Emma Lindblom 2024-01-29
	jr 	$ra 				# avsluta funktionen
	nop
	
time2string:

	# Simon Carlsson och Emma Lindblom 2024-01-27
	
	PUSH 	$ra 			# stoppa undan returadressen då den kommer att ändras varje gång
					# funktionen hexasc körs. Annars kommer den få ett enda 
					# felaktigt värde
	
	# hundradels sekunder
	PUSH 	$a0			# stoppar undan a0 
	andi 	$a0, $a1, 0xF		# maska de 4 sista bitarna och stoppa i a0
	jal 	hexasc			# hoppa in i hexasc funktionen och kör den
	nop
	POP 	$a0				# hämta tillbaka a0
	sb 	$v0, 4($a0)			# lagrar adressen a0 med offset 4 i v0
	
	# tiondels sekunder
	PUSH 	$a0 			# stoppar undan a0
	andi 	$a0, $a1, 0xF0		# maskar den näst sista byten och stoppar i a0
	srl 	$a0, $a0, 4			# shiftar bitvis a0 4 steg till höger, de maskade bitarna 
					# hamnar alltså längst till höger
	jal 	hexasc			# hoppa in i hexasc funktionen och kör den
	nop
	POP 	$a0 			# hämta tillbaka a0
	sb 	$v0, 3($a0)			# lagrar adressen a0 med offset 3 i v0
	
	# kolon
	li 	$v0, 0x3A			# lagrar 0x3A (kolon i ASCII) i v0
	sb 	$v0, 2($a0)			# lagrar adressen a0 med offset 2 i v0
	
	# minuter (ental)
	PUSH 	$a0 			# stoppar undan a0
	andi	$a0, $a1, 0xF00		# maskar den 3:e byten från höger och stoppar i a0
	srl	$a0, $a0, 8			# shiftar bitvis a0 8 steg åt höger, alltså längst till höger
	jal	hexasc			# hoppa in i hexasc funktionen och kör den
	nop
	POP	$a0 			# hämta tillbaka a0
	sb	$v0, 1($a0)			# lagrar adressen a0 med offset 1 i v0
	
	# minuter (tiotal)
	PUSH 	$a0 			# stoppa undan a0
	andi 	$a0, $a1, 0xF000	# maskar den 4:e byten från höger och stoppar i a0
	srl	$a0, $a0, 12		# shiftar bitvis a0 12 steg åt höger, alltså längst till höger
	jal 	hexasc			# hoppa in i hexasc funktionen och kör den
	nop
	POP 	$a0 			# hämta tillbaka a0
	sb 	$v0, 0($a0)			# lagrar adressen a0 med offset 0 i v0
	
	# null
	li 	$v0, 0x00			# lagrar 0x00 (NUL i ASCII) i v0
	sb 	$v0, 5($a0)			# lagrar adressen a0 med offset 5 i v0
	
	POP 	$ra 			# hämtar tillbaka returadressen
	jr 	$ra				# hoppar till returadressen i main
	nop