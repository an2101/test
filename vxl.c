#include <msp430g2553.h>

// Define control pins for the L298N module
#define IN1 BIT0 // P1.0
#define IN2 BIT1 // P1.1

// Define sensor pins
#define SENSOR1 BIT3 // P2.3
#define SENSOR2 BIT3 // P1.3

// Define switch pins
#define SWITCH1 BIT4 // P1.4
#define SWITCH2 BIT5 // P1.5

void setup() {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    P1DIR |= BIT6; // P1.6 output for LED
    P1DIR |= BIT7; // P1.7 output for buzzer

    // Configure sensor pins as inputs
    P2DIR &= ~BIT0; // P2.0 input (other sensor)
    P2DIR &= ~BIT1; // P2.1 input (fire sensor)
    
    // Configure control pins as outputs
    P1DIR |= IN1 + IN2;
    P1OUT &= ~(IN1 + IN2); // Initially turn off both outputs

    // Configure P2.3 (SENSOR1) as input with pull-up resistor
    P2DIR &= ~SENSOR1;       // P2.3 input
    P2REN |= SENSOR1;        // Enable pull-up/down resistor
    P2OUT |= SENSOR1;        // Select pull-up resistor

    // Configure P1.3 (SENSOR2) as input with pull-up resistor
    P1DIR &= ~SENSOR2;       // P1.3 input
    P1REN |= SENSOR2;        // Enable pull-up/down resistor
    P1OUT |= SENSOR2;        // Select pull-up resistor

    // Configure switch pins as inputs with pull-up resistors
    P1DIR &= ~(SWITCH1 + SWITCH2);
    P1REN |= SWITCH1 + SWITCH2; // Enable pull-up/down resistors
    P1OUT |= SWITCH1 + SWITCH2; // Select pull-up resistors

    // Configure Timer_A for PWM on P1.2 (TA0.1)
    P1DIR |= BIT2;               // P1.2 output
    P1SEL |= BIT2;               // P1.2 select Timer_A output
    TA0CCR0 = 1000 - 1;          // PWM period (1 kHz frequency)
    TA0CCTL1 = OUTMOD_7;         // Reset/Set output mode
    TA0CCR1 = 350;               // Initial duty cycle (25%)
    TA0CTL = TASSEL_2 + MC_1;    // SMCLK, up mode
}

void loop() {
    if (!(P2IN & BIT0)) {
        P1OUT |= BIT6; // Turn on LED1 (P1.6)
    } else {
        P1OUT &= ~BIT6; // Turn off LED1 (P1.6)
    }

    // Check the status of the fire sensor
    if (!(P2IN & BIT1)) {
        P1OUT |= BIT7; // Turn on buzzer (P1.7)
    } else {
        P1OUT &= ~BIT7; // Turn off buzzer (P1.7)
    }

    // Read sensor values
    int sensor1_status = P2IN & SENSOR1;
    int sensor2_status = P1IN & SENSOR2;
    
    // Read switch values
    int switch1_status = P1IN & SWITCH1;
    int switch2_status = P1IN & SWITCH2;

    // Check switch 1 status and adjust motor control and speed accordingly
    if (switch1_status == 0) {
        if ((sensor1_status == 0) || (sensor2_status == 0)) {
            // If either sensor detects an obstacle, move motor in one direction
            P1OUT |= IN2;  // Set IN2 high
            P1OUT &= ~IN1; // Set IN1 low
            TA0CCR1 = 350; // Set PWM duty cycle to 25%
        } else {
            // If no obstacles, stop the motor
            P1OUT &= ~(IN1 + IN2); // Turn off both IN1 and IN2
            TA0CCR1 = 0;           // Set PWM duty cycle to 0%
        }
    } else if (switch2_status == 0) {
        if ((!(sensor1_status))|| (!(sensor2_status))){
           P1OUT &= ~(IN1 + IN2); // Turn off both IN1 and IN2
            TA0CCR1 = 0;           // Set PWM duty cycle to 0%
            
        } else {
         
            // If either sensor detects an obstacle, move motor in the opposite direction
            P1OUT |= IN1;  // Set IN1 high
            P1OUT &= ~IN2; // Set IN2 low
            TA0CCR1 = 350; // Set PWM duty cycle to 25%
        }       
    } else {
        // Control motor based on sensor states
        if ((sensor1_status == 0) || (sensor2_status == 0)) {
            // If both sensors detect an obstacle
            P1OUT |= IN2;  // Set IN2 high
            P1OUT &= ~IN1; // Set IN1 low
            TA0CCR1 = 350; // Set PWM duty cycle to 25%
        } else {
            // If both sensors do not detect an obstacle
            P1OUT |= IN1;  // Set IN1 high
            P1OUT &= ~IN2; // Set IN2 low
            TA0CCR1 = 350; // Set PWM duty cycle to 25%
        } 
    }
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
}