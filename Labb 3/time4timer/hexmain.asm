  # hexmain.asm
  # Written 2015-09-04 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

  # Modified 2024 by Simon Carlsson and Emma Lindblom
	.text
main:
	li	$a0,0x17		# change this to test different values

	jal	hexasc		# call hexasc
	nop			# delay slot filler (just in case)	

	move	$a0,$v0		# copy return value to argument register

	li	$v0,11		# syscall with v0 = 11 will print out
	syscall			# one byte from a0 to the Run I/O window
	
stop:	j	stop		# stop after one run
	nop			# delay slot filler (just in case)

  # You can write your own code for hexasc here
  
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
