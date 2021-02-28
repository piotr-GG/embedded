start: 
		sbi ddrd, 5 ;wybranie pinu5 do sterowania pwm. 
		ldi r16, 0b00100011 ;ustawienie wartości dla wyboru -fast pwm mode
		out tccr0a, r16 ;Zapisanie wartosci do rejestr
		ldi r16, 0b00000001 ;ustawienie prescalera 
		out tccr0b, r16;zapisanie wartości prescalera
		ldi r17, 15 ; ustawienie poziomu wypełnienia impulsu 
		out ocr0b, r17; zapisanie poziomu wypełnienia impulsu do rejestru
		rjmp inkrementuj

dioda2:
		cbi ddrd, 5
		sbi ddrd, 6
		ldi r16, 0b10000011 ;ustawienie wartości dla wyboru -fast pwm mode
		out tccr0a, r16 ;Zapisanie wartosci do rejestr
		ldi r16, 0b00000001 ;ustawienie prescalera 
		out tccr0b, r16;zapisanie wartości prescalera
		ldi r17, 15 ; ustawienie poziomu wypełnienia impulsu 
		out ocr0a, r17; zapisanie poziomu wypełnienia impulsu do rejestru
		rjmp inkrementuj2

dioda3:
		cbi DDRD, 6
		sbi DDRB, 1

		LDI R16, (1<<COM1A1)|(1<<COM1B1)|(1<<COM1B0)|(1<<WGM10)
		sts TCCR1A , r16
		
		LDI R16, 0b11111111
		STS ICR1L, r16
		LDI R16, 0b11111111
		STS ICR1H, r16

		LDI R16, (1<<WGM12)|(1<<CS10) ;ustawienie prescalera 
		STS TCCR1B, R16;zapisanie wartości prescalera

		ldi r17, 15 ; ustawienie poziomu wypełnienia impulsu 
		STS OCR1AL, r17; zapisanie poziomu wypełnienia impulsu do rejestru

		LDI R16, 0b00000000
		STS OCR1AH, r16

		LDI R16, 96
		STS OCR1BL, r16
		rjmp inkrementuj3		

inkrementuj:
		call delay_05
		inc r17
		out ocr0b, r17; zapisanie poziomu wypełnienia impulsu do rejestru
		CPI r17, 200 ;jesli carry bit jest ustawiony, to jest mniejsze
		BRCS inkrementuj ;przeskakuje jesli carry bit jest ustawiony (branch if carry set)

		rjmp dioda2

inkrementuj2:
		call delay_05
		inc r17
		out ocr0a, r17; zapisanie poziomu wypełnienia impulsu do rejestru
		CPI r17, 200 ;jesli carry bit jest ustawiony, to jest mniejsze
		BRCS inkrementuj2 ;przeskakuje jesli carry bit jest ustawiony (branch if carry set)

		rjmp dioda3

inkrementuj3:
		call delay_05
		inc r17
		STS ocr1aL, r17; zapisanie poziomu wypełnienia impulsu do rejestru
		CPI r17, 200 ;jesli carry bit jest ustawiony, to jest mniejsze
		BRCS inkrementuj3 ;przeskakuje jesli carry bit jest ustawiony (branch if carry set)

		cbi DDRb, 1
		rjmp start


// funkcja opozniajaca
 delay_05:
 LDI    r16, 2					  //zaladowanie wartosci 8 do rejestru r16
 // zewnetrzna petla funkcji opozniajacej
 outer_loop:
 LDI    r24, low(3037)            //przypisanie mlodszego bajtu z wartosci 3037 do rejestru r24 
 LDI    r25, high(3037)           //przypisanie starszego bajtu z wartosci 3037 do rejestru r25
 // wewnetrzna petla funkcji opozniajacej
 delay_loop:
 ADIW   r24, 1                    //inkrementacja wartosci w rejestrze r24 o 1
 BRNE   delay_loop                //skok do delay_loop jesli Zero Flag jest nieustawiona (Z=0)
 DEC    r16                       //dekrementacja wartosci w rejestrze r16     
 BRNE   outer_loop                //skok do outer_loop jesli Z = 0
 RET							  //powrot jesli r16 = 0 (wtedy Z =1 i BRNE jest niewykonywane)


