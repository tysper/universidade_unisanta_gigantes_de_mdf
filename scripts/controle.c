#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Definições para os pinos PWM
#define PWM1_PIN PD5 // Motor 1 (OC0A)
#define PWM2_PIN PD6 // Motor 2 (OC0B)

// Definições de controle dos motores
#define MOTOR1_DIR1_PIN PD7 // Direção Motor 1
#define MOTOR1_DIR2_PIN PB0 // Direção Motor 1
#define MOTOR2_DIR1_PIN PB3 // Direção Motor 2
#define MOTOR2_DIR2_PIN PB4 // Direção Motor 2

// definições pinos LEDs, Botão e Laser
#define LED1 PD2
#define LED2 PD4
#define LED3 PB2
#define Laser PD3
#define BotaoLED PB5

// Prototipação de funções
void UART_init(uint16_t baud);
char UART_receive(void);
void PWM_init(void);
void motor_control(uint8_t motor, uint8_t direction, uint8_t speed);
int8_t map_joystick_value(uint8_t value);

volatile uint16_t total_ovf; // variável para contar os overflows para ligar o laser
void timer2_init_laser(void);
void ligaLED(void);
void pinosConfigurados(void);

int main(void)
{
    // Inicialização de UART e PWM
    UART_init(9600); // Comunicação serial com baudrate 9600
    PWM_init();      // Inicializa os PWMs

    // Configuração dos pinos de direção dos motores como saída
    DDRB |= (1 << MOTOR1_DIR1_PIN) | (1 << MOTOR1_DIR2_PIN) | (1 << MOTOR2_DIR1_PIN) | (1 << MOTOR2_DIR2_PIN);

    // Configuração inicial dos motores
    motor_control(1, 0, 0); // Motor 1 parado
    motor_control(2, 0, 0); // Motor 2 parado

    uint8_t joystick_x = 127; // Posição neutra no eixo X
    uint8_t joystick_y = 127; // Posição neutra no eixo Y

    // Funções para controle dos pinos e  timer que controla o laser
    pinosConfigurados();
    timer2_init_laser();
    sei(); // habilita interrupções globais

    while (1)
    {
        // Recebe os 2 primeiros bytes do pacote (eixos do joystick)
        joystick_x = UART_receive();
        joystick_y = UART_receive();

        // Converte os valores do joystick para uma faixa de -100 a +100
        int8_t x_mapped = map_joystick_value(joystick_x);
        int8_t y_mapped = map_joystick_value(joystick_y);

        // Calcula a velocidade de cada motor com base nos valores mapeados
        int8_t motor1_speed = y_mapped + x_mapped; // Motor esquerdo
        int8_t motor2_speed = y_mapped - x_mapped; // Motor direito

        // Ajusta os valores para ficarem na faixa de 0 a 255
        uint8_t motor1_pwm = (motor1_speed > 0) ? motor1_speed : -motor1_speed;
        uint8_t motor2_pwm = (motor2_speed > 0) ? motor2_speed : -motor2_speed;

        // Define a direção dos motores
        motor_control(1, (motor1_speed >= 0) ? 1 : 2, motor1_pwm);
        motor_control(2, (motor2_speed >= 0) ? 1 : 2, motor2_pwm);
    }

    return 0;
}

// Inicialização da UART
void UART_init(uint16_t baud)
{
    uint16_t ubrr = F_CPU / 16 / baud - 1;  // Calcula o valor para UBRR
    UBRR0H = (ubrr >> 8);                   // Configuração do baud rate (parte alta)
    UBRR0L = ubrr;                          // Configuração do baud rate (parte baixa)
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // Habilita RX e TX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Modo assíncrono, 8 bits de dados, 1 stop bit
}

// Recepção de um caractere pela UART
char UART_receive(void)
{
    while (!(UCSR0A & (1 << RXC0))); // Aguarda até receber o dado
    return UDR0; // Retorna o dado recebido
}

// Inicialização do PWM para controle dos motores
void PWM_init()
{
    // Configura os pinos PD6 (OC0A) e PD5 (OC0B) como saída
    DDRD |= (1 << PD6) | (1 << PD5);

    // Configura o Timer/Counter0 no modo Fast PWM
    // Modo Fast PWM: WGM02 = 0, WGM01 = 1, WGM00 = 1
    TCCR0A |= (1 << WGM01) | (1 << WGM00);
    TCCR0B &= ~(1 << WGM02);

    // Habilita a saída PWM em OC0A (PD6) e OC0B (PD5)
    // Configura o modo Clear on Compare Match
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1); // Saídas PD6 e PD5

    // Configura o prescaler para 8 (frequência adequada para PWM em motores)
    TCCR0B |= (1 << CS01);
    TCCR0B &= ~((1 << CS02) | (1 << CS00));

    // Inicializa os valores dos PWMs com duty cycle 0
    OCR0A = 0; // Motor 1
    OCR0B = 0; // Motor 2
}

// Controle dos motores
void motor_control(uint8_t motor, uint8_t direction, uint8_t speed)
{
    if (motor == 1)
    { // Motor 1
        if (direction == 1)
        { // Para frente
            PORTB |= (1 << MOTOR1_DIR1_PIN);
            PORTB &= ~(1 << MOTOR1_DIR2_PIN);
        }
        else if (direction == 2)
        { // Para trás
            PORTB |= (1 << MOTOR1_DIR2_PIN);
            PORTB &= ~(1 << MOTOR1_DIR1_PIN);
        }
        else
        { // Parado
            PORTB &= ~((1 << MOTOR1_DIR1_PIN) | (1 << MOTOR1_DIR2_PIN));
        }
        OCR0B = speed; // Ajusta o PWM do Motor 1
    }
    else if (motor == 2)
    { // Motor 2
        if (direction == 1)
        { // Para frente
            PORTB |= (1 << MOTOR2_DIR1_PIN);
            PORTB &= ~(1 << MOTOR2_DIR2_PIN);
        }
        else if (direction == 2)
        { // Para trás
            PORTB |= (1 << MOTOR2_DIR2_PIN);
            PORTB &= ~(1 << MOTOR2_DIR1_PIN);
        }
        else
        { // Parado
            PORTB &= ~((1 << MOTOR2_DIR1_PIN) | (1 << MOTOR2_DIR2_PIN));
        }
        OCR0A = speed; // Ajusta o PWM do Motor 2
    }
}

// Mapeia o valor do joystick de 0-255 para -100 a +100
int8_t map_joystick_value(uint8_t value)
{
    return ((int16_t)value - 127) * 100 / 127;
}

// Função que controla o Laser
void timer2_init_laser()
{
    // prescale de 1024
    TCCR2B |= (1 << CS21) | (1 << CS22) | (1 << CS20);
    TCNT2 = 0;
    TIMSK2 |= (1 << TOIE2);
    total_ovf = 0;
}
ISR(TIMER2_OVF_vect)
{
    total_ovf++;
    // 16Mhz/1024=15625 -> 1/15625 = 64us-> 255*64us = 16,32ms -> 1/16,32ms = 61
    if (total_ovf >= 61)
    {
        PORTD ^= (1 << Laser);
        total_ovf = 0;
    }
}
void ligaLED()
{
    PORTD |= (1 << LED1) | (1 << LED2);
    PORTB |= (1 << LED3);
}
void pinosConfigurados()
{
    DDRB |= (1 << LED3);
    DDRD |= (1 << LED1) | (1 << LED2) | (1 << Laser);
    // habilitar pull up do botao
    PORTB |= (1 << PB5);
    // configuracao da interrupção do botao
    PCICR |= (1 << PCIE0);   // registrador PORTB
    PCMSK0 |= (1 << PCINT5); // pino PB5
}
ISR(PCINT0_vect)
{
    if (!(PINB & (1 << BotaoLED)))
    {
        ligaLED();
    }
}
