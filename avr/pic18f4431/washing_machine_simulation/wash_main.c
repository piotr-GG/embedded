#include <xc.h>
#include <pic18f4431.h>

// CONFIG2H
#pragma config WDTEN = OFF // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDPS = 32768 // Watchdog Timer Postscale Select bits (1:32768)
#pragma config WINEN = OFF // Watchdog Timer Window Enable bit (WDT window disabled)


const unsigned char NONE = 0;
const unsigned char PREWASH = 1;
const unsigned char MAINWASH = 2;
const unsigned char RINSE = 3;

const unsigned char CLOCKWISE = 1;
const unsigned char ANTICLOCKWISE = 2;

unsigned char MODE;
unsigned char lye_temperature = 60;
unsigned char lye_level = 0;

unsigned char timer1_count = 0;
int ADC_val = 0;

void ADC_init(){

    TRISAbits.RA0 = 1;
    ANSEL0bits.ANS0 = 1;
       
    ADRESH = 0;      
    ADRESL = 0;
    ADCON1 =0;  
    //A/D Acquisition Time delay: TAD time=4                     ACQT2-0  = 0010 
    //A/D Conversion Clock: FOSC/2 = 500kHz(1clock period= 2us)  ADCS2-0 = 000
    //A/D Result Format: 1 = right justified                     ADFM
    ADCON2 =  0B10010000;
    ADCON0bits.ADON =1;  // A/D Converter module is enabled
}
int ADC_read(){
    ADCON0bits.GODONE = 1; //run the conversion
    while (ADCON0bits.GODONE == 1); // wait for the end of conversion
    return ADRES;  
}

void open_first_valve(){
        ADC_val = ADC_read();
        if (ADC_val < 250) {
            LATAbits.LA1 = 0;
        }
        else {
            LATAbits.LA1 = 1;
        }
}


void setClockwise(){
    CCP1CONbits.CCP1M3 = 1;
    CCP1CONbits.CCP1M2 = 1;
    
    CCP2CONbits.CCP2M3 = 0;
    CCP2CONbits.CCP2M2 = 0; 
    
    T2CONbits.TMR2ON = 1;
}

void setAntiClockwise(){
    CCP1CONbits.CCP1M3 = 0;
    CCP1CONbits.CCP1M2 = 0;
    
    CCP2CONbits.CCP2M3 = 1;
    CCP2CONbits.CCP2M2 = 1; 
    
    T2CONbits.TMR2ON = 1;    
}

void resetPWM(){
    CCP1CONbits.CCP1M3 = 0;
    CCP1CONbits.CCP1M2 = 0;
    
    CCP2CONbits.CCP2M3 = 0;
    CCP2CONbits.CCP2M2 = 0; 
    
    T2CONbits.TMR2ON = 0;     
}

void wait(char seconds){
    /*
     F = 4Mhz, so the basic frequency is 1Mhz
     P = 1 us (period)
     T0PS = 0b111, so...
     P = 1us * 256 = 256us
     * 
     Formula for clock counts: 
     desired delay in us / period with prescaler
     * 
     Example:
     4 * 10^6 / 256 = 15 625 counts
     */
    
    T0CONbits.TMR0ON=0; //stop the timer
    INTCONbits.TMR0IF=0; //clear timer flag
    T0CONbits.T016BIT=0; //16 bit mode    
    T0CONbits.T0CS =0; //interal clock sourse
    T0CONbits.PSA=0; //prescaler is selected
    T0CONbits.T0PS = 0b111; //prescaler division 1:256
    TMR0=65530; //set timer value   61630

    T0CONbits.TMR0ON=1;
    
    char temp = seconds;
    while(temp != 0){
        if(INTCONbits.T0IF == 1){  //overflow
            TMR0=65530; //set timer value
            INTCONbits.T0IF = 0; //clear timer flag
            temp = temp - 1;
        }
    };
    
    T0CONbits.TMR0ON=0;
    INTCONbits.TMR0IF=0;
    
    return;
}

/*
 Basic function responsible for rotating the drum in specified direction
 */
void rotate_drum(unsigned char direction){
    if(direction == ANTICLOCKWISE) {
        setAntiClockwise();
    }
    if(direction == CLOCKWISE) {
        setClockwise();
    }
}
/*
 Function that is responsible for alternating the drum's spin direction X times
 * X is specified by count argument
 */
void spin_drum(unsigned char count){
    for(unsigned char i = count; i > 0; i--){
        rotate_drum(CLOCKWISE);
        wait(1);
        rotate_drum(ANTICLOCKWISE);
        wait(1);
    }
    //Reset spin direction
    resetPWM();
    return;
}

void prewash(){
    //Wait for the first valve to open
    
    while(LATAbits.LA1 == 0){
        open_first_valve();
    }

    //Wait for the drum to be filled with detergent
    wait(2);
    //Close the 1st valve
    LATAbits.LA1 = 0;
    //Spin the drum 3  consecutive times
    spin_drum(3);
    //End the cycle
    MODE = NONE;
    return;
}

void rinse(){
    //Open the 3rd valve
    LATAbits.LA2 = 1;
    //Wait for the drum to be filled with detergent
    wait(3);
    //Close the 3rd valve
    LATAbits.LA2 = 0;
    //Spin the drum 5 consecutive times
    spin_drum(5);
    //End the cycle
    MODE = NONE;
    return;
    
}

void fill_lye(){
    //Set lye level to zero
    lye_level = 0;
    //Open the 2nd valve with lye
    LATAbits.LA1 = 1;
    //Fill tank with lye
    while(lye_level != 100){
        //Add 20% of lye level every second
        wait(1);
        lye_level = lye_level + 20;
    }
    //Close the 2nd valve with lye
    LATAbits.LA1 = 0;
    return;
}

void heat_lye(){
    while(lye_temperature <= 90){
        wait(1);
        lye_temperature = lye_temperature + 5;
    }
    return;
}

void pump_lye(){
    //Decrease lye level by 20 every 2 seconds
    while(lye_level > 0){
        wait(2);
        lye_level = lye_level - 20;
    }
    return;
}
void mainwash(){
    //First execute prewash
    prewash();
    //Fill tank with lye
    fill_lye();
    //Heat lye
    heat_lye();
    //Pump lye
    pump_lye();
    //Spin the drum
    spin_drum(3);
    //Execute rinse as last
    rinse();
}
void lye_update(){
    if(timer1_count == 15){
        if(lye_temperature > 30){
            lye_temperature = lye_temperature - 5;
            timer1_count = 0;
        }
    }
}

void init_T1(){
    
    PIR1bits.TMR1IF = 0 ;// Reset the TMR1IF flag bit
    TMR1H = 0x00; //Set initial value for the Timer1
    TMR1L = 0x00;
    T1CONbits.TMR1CS = 0; // Timer1 counts pulses from internal oscillator
    T1CONbits.T1CKPS0 = 1; // Assigned prescaler rate is 1:8
    T1CONbits.T1CKPS1 = 1;
    PIE1bits.TMR1IE = 1 ;// Enable interrupt on overflow
    T1CONbits.TMR1ON = 1; // Turn the Timer1 on
     
}
void __interrupt(high_priority) High_isr(void){
    if(TMR1IF){
        timer1_count = timer1_count + 1;
        lye_update();
        TMR1IF = 0;  //clear flag
        INTCONbits.GIE = 1; 
        INTCONbits.PEIE = 1;  //enable peripheral interrupts
        
        return;
    }

        if(INTCONbits.INT0IF){
            //Set mode as prewash
            MODE = PREWASH;
            //Turn on the corresponding LED
            LATAbits.LA3 = 1;
            //Execute prewash cycle
            prewash();
            //Clear flag
            INTCONbits.INT0IF = 0;
            //Turn off the LED
            LATAbits.LA3 = 0;
            MODE = NONE;
            
            INTCONbits.GIE = 1;  
            return;
        }
        if(INTCON3bits.INT1IF){
            //Set mode as mainwash
            MODE = MAINWASH;
            //Turn on the corresponding LED
            LATAbits.LA4 = 1;
            //Execute mainwash
            mainwash();
            //Clear flag
            INTCON3bits.INT1IF = 0;
            //Turn off the LED
            LATAbits.LA4 = 0;
            MODE = NONE;
            
            INTCONbits.GIE = 1;  
            return;
        }
        if(INTCON3bits.INT2F){
            //Set mode as rinse
            MODE = RINSE;
            //Turn on the corresponding LED
            LATAbits.LA5 = 1;
            //Execute rinse cycle
            rinse();
            //Clear flag
            INTCON3bits.INT2F = 0;
            //Turn off the LED
            LATAbits.LA5 = 0;
            
            INTCONbits.GIE = 1;  
            MODE = NONE;
            return;
        }
        //Enable global interrupts
        INTCONbits.GIE = 1;   
    

    }

void initPWM(){
    PORTC = 0;
    LATC = 0;
    TRISCbits.RC2 = 0;
    TRISCbits.RC1 = 0;
    
    PR2 = 24;
    
    //Set CCP0
    CCPR1L = 0b00001111;
    CCP1CONbits.CCP1M3 = 0;
    CCP1CONbits.CCP1M2 = 0;
    
    CCP1CONbits.DC1B1 = 0;
    CCP1CONbits.DC1B0 = 0;
    
    //Set CCP1
    CCPR2L = 0b00001111;
    CCP2CONbits.CCP2M3 = 0;
    CCP2CONbits.CCP2M2 = 0;
    
    CCP2CONbits.DC2B1 = 0;
    CCP2CONbits.DC2B0 = 0;
    //Set timer
    //Prescaler = 1
    T2CONbits.T2CKPS0 = 0;
    T2CONbits.T2CKPS1 = 0;
      
}


void main() {
    //Washing machine is idle as default
    MODE = NONE;
    
    //Initialize PORTA
    PORTA = 0;
    LATA = 0;
    //RA1 - VALVE 1 FOR PREWASH - OUTPUT
    TRISAbits.RA1 = 0;
    //RA2 - VALVE 2 FOR MAINWASH - OUTPUT
    TRISAbits.RA2 = 0;
    //RA3 - VALVE 3 FOR RINSE - OUTPUT
    TRISAbits.RA3 = 0;
    // RA4 - PREWASH LED - OUTPUT
    TRISAbits.RA4 = 0;
    // RA5 - MAINWASH LED - OUTPUT
    TRISAbits.RA5 = 0;   
    // RA6 - RINSE LED - OUTPUT
    TRISAbits.RA6 = 0;
    
    //Initialize PORTC
    PORTC = 0;
    LATC = 0;
    //RC3 - Input for button 1 
    TRISCbits.RC3 = 1;
    //RC4 - Input for button 2
    TRISCbits.RC4 = 1;
    //RC5 - Input for button 3
    TRISCbits.RC5 = 1;
    
    //Initialize interrupts
    INTCON = 0b11010000;
    INTCON3bits.INT1E = 1;
    INTCON3bits.INT2E = 1;
    
    init_T1();
    
    PWMCON0 =0;
    RCONbits.IPEN = 1; 
    INTCON =  0b11110000;    // GIE;GIEL =0;TMR0IE = 1
    TMR0 = 0; 
    
    ADC_init();
    initPWM();
    
    while(1){
        }
    
    return;
}