  # analyze.asm
  # This file written 2015 by F Lundevall
  # Copyright abandoned - this file is in the public domain.
  
  # Modified 2024 by Simon Carlsson and Emma Lindblom

	.text
main:
	li	$s0,0x30	
loop:
	move	$a0,$s0		# copy from s0 to a0
	
	li	$v0,11		# syscall with v0 = 11 will print out
	syscall			# one byte from a0 to the Run I/O window

	addi	$s0,$s0,3	# what happens if the constant is changed? # Emma Lindblom 2024-01-23 
				# tidigare ökade s0 med 1 men för att skriva var tredje tecken krävs att öka med 3 istället
							
	
	li	$t0,0x5d	# Simon Carlsson 2024-01-23
				# loopen fortsätter så länge s0 och t0 inte är lika, för att matcha 3 stegs hoppen sätts t0 till 5d
				# för att avsluta efter Z
				
	bne	$s0,$t0,loop	
	nop			# delay slot filler (just in case)

stop:	j	stop		# loop forever here
	nop			# delay slot filler (just in case)

