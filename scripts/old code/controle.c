#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Definições para os pinos PWM
#define PWM1_PIN PD6 // Motor 1 (OC0A)
#define PWM2_PIN PD5 // Motor 2 (OC0B)

// Definições de controle dos motores
#define MOTOR1_DIR1_PIN PB0 // Direção Motor 1
#define MOTOR1_DIR2_PIN PB1 // Direção Motor 1
#define MOTOR2_DIR1_PIN PB2 // Direção Motor 2
#define MOTOR2_DIR2_PIN PB3 // Direção Motor 2

// Prototipação de funções
void UART_init(uint16_t baud);
char UART_receive(void);
void PWM_init(void);
void motor_control(uint8_t motor, uint8_t direction, uint8_t speed);
int8_t map_joystick_value(uint8_t value);

int main(void) {
    // Inicialização de UART e PWM
    UART_init(9600); // Comunicação serial com baudrate 9600
    PWM_init(); // Inicializa os PWMs
    
    // Configuração dos pinos de direção dos motores como saída
    DDRB |= (1 << MOTOR1_DIR1_PIN) | (1 << MOTOR1_DIR2_PIN) | (1 << MOTOR2_DIR1_PIN) | (1 << MOTOR2_DIR2_PIN);

    // Configuração inicial dos motores
    motor_control(1, 0, 0); // Motor 1 parado
    motor_control(2, 0, 0); // Motor 2 parado

    uint8_t joystick_x = 127; // Posição neutra no eixo X
    uint8_t joystick_y = 127; // Posição neutra no eixo Y

    while (1) {
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
void UART_init(uint16_t baud) {
    uint16_t ubrr = F_CPU / 16 / baud - 1; // Calcula o valor para UBRR
    UBRR0H = (ubrr >> 8); // Configuração do baud rate (parte alta)
    UBRR0L = ubrr; // Configuração do baud rate (parte baixa)
    UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Habilita RX e TX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Modo assíncrono, 8 bits de dados, 1 stop bit
}

// Recepção de um caractere pela UART
char UART_receive(void) {
    while (!(UCSR0A & (1 << RXC0))); // Aguarda até receber o dado
    return UDR0; // Retorna o dado recebido
}

// Inicialização do PWM para controle dos motores
void PWM_init() {
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
void motor_control(uint8_t motor, uint8_t direction, uint8_t speed) {
    if (motor == 1) { // Motor 1
        if (direction == 1) { // Para frente
            PORTB |= (1 << MOTOR1_DIR1_PIN);
            PORTB &= ~(1 << MOTOR1_DIR2_PIN);
        } else if (direction == 2) { // Para trás
            PORTB |= (1 << MOTOR1_DIR2_PIN);
            PORTB &= ~(1 << MOTOR1_DIR1_PIN);
        } else { // Parado
            PORTB &= ~((1 << MOTOR1_DIR1_PIN) | (1 << MOTOR1_DIR2_PIN));
        }
        OCR0A = speed; // Ajusta o PWM do Motor 1
    } else if (motor == 2) { // Motor 2
        if (direction == 1) { // Para frente
            PORTB |= (1 << MOTOR2_DIR1_PIN);
            PORTB &= ~(1 << MOTOR2_DIR2_PIN);
        } else if (direction == 2) { // Para trás
            PORTB |= (1 << MOTOR2_DIR2_PIN);
            PORTB &= ~(1 << MOTOR2_DIR1_PIN);
        } else { // Parado
            PORTB &= ~((1 << MOTOR2_DIR1_PIN) | (1 << MOTOR2_DIR2_PIN));
        }
        OCR0B = speed; // Ajusta o PWM do Motor 2
    }
}

// Mapeia o valor do joystick de 0-255 para -100 a +100
int8_t map_joystick_value(uint8_t value) {
    return ((int16_t)value - 127) * 100 / 127;
}
