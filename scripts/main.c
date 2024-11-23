#include <avr/io.h>
#include <avr/interrupt.h>

/// Pino do primeiro Led. 
#define led1_pin PD2
/// Pino do segundo Led. 
#define led2_pin PD4
/// Pino do terceiro Led. 
#define led3_pin PB2
/// Pino do laser. 
#define laser_pin PD3
/// Pino do sensor LDR. 
#define sensor_pin PC0
/// Pino do sinal PWM do motor esquerdo. 
#define motor_esquerdo PD5
/// Pino do sinal PWM do motor direito. 
#define motor_direito PD6
/// Pino da direção do motor esquerdo para frente. 
#define esquerdo_frente PB0
/// Pino da direção do motor esquerdo para traz. 
#define esquerdo_traz PD7
/// Pino da direção do motor direito para frente.
#define direito_frente PB4
/// Pino da direção do motor direito para traz. 
#define direito_traz PB3
/// Pino do botão de reset
#define botao_reset PB5
/// Macro referente a frequência do Microcontrolador utilizado.
#define F_CPU 16000000
/// Macro referente a velocidade de comunicação que se deseja utilizar no USART.
#define BAUD 9600



/// Variável para contar a quantidade de estouros do TIMER0. Valores possíveis: (0-65535)
volatile uint16_t total_ovf0;
/// Variável para contar a quantidade de estouros do TIMER1. Valores possíveis: (0-65535)
volatile uint16_t total_ovf1;
/// Variável para contar a quantidade de estouros do TIMER2. Valores possíveis: (0-65535)
volatile uint16_t total_ovf2;
/// Variável para controlar o estado do laser. Valores possíveis: (true | false)
volatile bool estado_laser = true; 

/// Variável para contabilizar a quantidade de vidas atuais do carro. Valores possíveis: (0-3)
volatile int quantidade_vidas = 3; 
/// Variável para controlar se o jogo ainda está em andamento ou foi finalizado. Valores possíveis: (true | false)
volatile bool jogo_acabou = false;
/// Variável para controlar se o sensor LDR foi atingido por um laser. Valores possíveis: (true | false)
volatile bool atingido = false;

/// Variável para controlar o estado do motor. Valores possíveis: (true | false)
volatile bool motor_ativado = true;

/// Variável que armazena a letra recebida pelo USART. Valores possíveis: ('l', 'r', 'u', 'd', 's')
volatile char letra_recebida;
/// Variável que controla se o dado foi recebido ou não. Valores possíveis: (true | false)
volatile bool dado_recebido = false;



/// Atualizar a quantidade de vida

/// O parametro <int> numero_de_vidas especifica quantos leds devem acender para indicar a quantidade de vida atual e pode variar de 0-3 vidas. Qualquer outro valor é ignorado. A função faz o uso de uma estrutura "switch-case" para habilitar e desabilitar os registros corretos. 
void atualizar_vidas(int numero_de_vidas)
{
	switch(numero_de_vidas)
	{
		case 0:
			PORTD = 0;
			PORTB = 0;
			break;
		case 1:
			PORTD |= (1 << led1_pin);
			PORTD &= ~(1 << led2_pin);
			PORTB &= ~(1 << led3_pin);
			break;
		case 2:
			PORTD |= (1 << led1_pin);
			PORTD |= (1 << led2_pin);
			PORTB &= ~(1 << led3_pin);
			break;
		case 3:
			PORTD |= (1 << led1_pin);
			PORTD |= (1 << led2_pin);
			PORTB |= (1 << led3_pin);
			break;
    }
}

/// Alternar o estado do laser

/// A função <void> alternar_laser é usada para alternar o estado do laser. Quando executada o estado do laser é alternado. Se antes estava ligado passa a estar desligado e vice-versa. A função utiliza uma operação XOR para alternar o valor referente ao laser.
void alternar_laser()
{
	PORTD ^= (1 << laser_pin);
}

/// Definir o estado do laser
 
/// A função <void> atualizar_estado_laser é usada para definir o estado do laser. O parâmetro <bool> estado só pode ser falso ou verdadeiro, valores nulos não são aceitos e são ignorados. A função verifica o argumento que foi passado à ela e faz o tratamento devido com uma sintaxe "if-else".
void atualizar_estado_laser(bool estado)
{
	if(estado)
	{
		PORTD |= (1 << laser_pin);
	}
	else 
	{
		PORTD &= ~(1 << laser_pin);
	}
}


/// Iniciar o TIMER1

/// A função <void> timer1_init_delay é usada para iniciar o timer. O TIMER1 é utilizado para criar um delay eficiente. O registro TCCR1B define o uso do prescaler para o valor 64, diminuindo o clock de 16MHz para 250KHz. O registro TCNT1 é iniciado com o valor 0 para dar ínicio a contagem do contador. A variável de controle <volatile uint16_t> total_ovf1 é iniciada como zero (ela é responsável por armazenar quantas vezes o TIMER1 estorou).
void timer1_init_delay()
{
	TCCR1B |= ((1 << CS10) | (1 << CS11));

	TCNT1 = 0;

	total_ovf1 = 0;
}

/// Interrupção TIMER1 OVF

/// A interrupção definida no TIMER1 está configurada para incrementar a variavel <volatile uint16_t> total_ovf1 no estouro do contador.
ISR(TIMER1_OVF_vect)
{
	total_ovf1++;
}

/// Atrasar execução do código
 
/// A função <void> atraso_segundos recebe o parâmetro <uint16_t> segundos e para a execução do código pelo tempo específicado em segundos. A variável <uint16_t> ovf_totais é calculada levando em consideração uma frequencia de 16MHz da CPU, e levando em consideração que foi diminuido para 250KHz pelo prescaler, cada ciclo tem um período de 4us. Levando em consideração que o TIMER1 é de 16 bits o atraso máximo possível é de 0.262 segundos. Fazendo uma aproximação concluímos que quatro ciclos de estouro são suficientes para chegar ao tempo de um segundo.
void atraso_segundos(uint16_t segundos)
{
	TIMSK1 |= (1 << TOIE1);

	total_ovf1 = 0;
	uint16_t ovf_totais = segundos * 4;
	
	while(total_ovf1 < ovf_totais)
	{}

	TIMSK1 &= ~(1 << TOIE1);
}

/// Iniciar o TIMER0

/// A função <void> timer0_init_pwm é usada para iniciar o timer. O TIMER0 é utilizado para criar um sinal com PWM. O registro TCCR0A é utilizado para selecionar o modo "Fast PWM" e selecionar o modo de operação não-inversor, além de definir o comportamento do pino de saída do sinal. O registro TCCR0B é usado para definir o prescaler para 0.
void timer0_init_pwm()
{
	TCCR0A |= ((1 << COM0A1) | (1 << WGM00) | (1 << WGM01) | (1 << COM0B1));
		
	TCCR0B |= (1 << CS00);
}

/// Atualizar direção do carrinho
 
/// A função <void> atualizar_direcao é usada para se comunicar com os motores utilizando o TIMER0. O parâmetro <char> direcao é utilizado para definir qual direcao dos motores. Os registros OCR0B e OCR0A são utilizados para definir o ciclo de trabalho do PWM. A direções são mapeadas da seguinte forma: u → Mover para frente; d → Mover para traz; l → Mover para a esquerda; r → Mover para a direita; t → Girar o carrinho; s → Parar o carrinho. A estrutura "switch-case" é utilizada nesse caso.
void atualizar_direcao(char direcao)
{
	if(direcao != 's')
	{
		OCR0B = 254;
		OCR0A = 254;
	} 
	else 
	{
		OCR0B = 0;
		OCR0A = 0;
	}

	switch (direcao)
	{
		case 'u':
			PORTB |= (1 << esquerdo_frente);
			PORTD &= ~(1 << esquerdo_traz);
			PORTB |= (1 << direito_frente);
			PORTB &= ~(1 << direito_traz);
			break;
		case 'd':
			PORTB &= ~(1 << esquerdo_frente);
			PORTD |= (1 << esquerdo_traz);
			PORTB &= ~(1 << direito_frente);
			PORTB |= (1 << direito_traz);
			break;
		case 'l':
			PORTB &= ~(1 << esquerdo_frente);
			PORTD &= ~(1 << esquerdo_traz);
			PORTB |= (1 << direito_frente);
			PORTB &= ~(1 << direito_traz);
			break;
		case 'r':
			PORTB |= (1 << esquerdo_frente);
			PORTD &= ~(1 << esquerdo_traz);
			PORTB &= ~(1 << direito_frente);
			PORTB &= ~(1 << direito_traz);
			break;
		case 't':
			PORTB |= (1 << esquerdo_frente);
			PORTD &= ~(1 << esquerdo_traz);
			PORTB &= ~(1 << direito_frente);
			PORTB |= (1 << direito_traz);
			break;
		case 's':
			PORTB &= ~(1 << esquerdo_frente);
			PORTD &= ~(1 << esquerdo_traz);
			PORTB &= ~(1 << direito_frente);
			PORTB &= ~(1 << direito_traz);
			break;
	}
}

/// Iniciar o TIMER2

/// A função <void> timer2_init_laser é utilizada para iniciar o timer. O TIMER2 é utilizado para controlar o estado do laser que deve ser alternado de um em um segundo. O registro TCCR2B é utilizado para definir o prescaler de 1024. O registro TCNT2 é utilizado para iniciar o contador. O registro TIMSK2 é utilizado para ativar as interrupções quando o contador transbordar. E a variável <uint16_t> total_ovf2 é iniciada com um zero para controlar a quantidade de interações necessárias.
void timer2_init_laser()
{
	
	TCCR2B |= (1<<CS21) | (1<< CS22) | (1<<CS20);
	
	TCNT2 = 0;
	
	TIMSK2 |= (1 <<TOIE2);

	total_ovf2 = 0;
}

/// Interrupção TIMER2 OVF
 
/// A interrupção definida no TIMER2 está configurada para incrementar a variável de controle <uint16_t> total_ovf2 e também alternar o estado do laser caso o tempo de um segundo tenha se passado. Nesse caso foi levado em consideração que foi utilizado um prescaler de 1024 deixando o clock final em 15.625KHz, sendo que cada ciclo leva em torno de 64us para ser completado. Como o TIMER2 tem 8 bits o atraso máximo possível é de 16.384ms, fazendo uma aproximação concluimos que 61 estouros são suficientes para gerar 1 s de atraso.
ISR(TIMER2_OVF_vect)
{
	total_ovf2++;

	if (total_ovf2 >= 61)
	{
		if(estado_laser & !jogo_acabou)
		{
			alternar_laser();
		} else {
			atualizar_estado_laser(false);
		}
		total_ovf2 = 0;	
	}
}

/// Começar a conversão ADC

/// A função <void> comecar_adc_conversao é utilizada para iniciar a conversão do conversor analógico para digital. Não aceita nenhum parâmetro. O registro ADCSRA é utilizado, definindo o bit ADSC a conversão do valor recebido no pino é iniciada.
void comecar_adc_conversao()
{
	ADCSRA |= (1 << ADSC);
}

/// Desligar a interrupção ADC

/// A função <void> desligar_adc é utilizada para desabilitar a interrupção disparada pelo sistema "ADC". Não aceita nenhum parâmetro. O registro ADCSRA é utilizado, limpando o bit ADIE a interrupção não acontece mais.
void desligar_adc()
{
	ADCSRA &= ~(1 << ADIE);
}

/// Ligar a interrupção ADC

/// A função <void> religar_adc é utilizada para habilitar a interrupção disparada pelo sistema "ADC" novamente. Não aceita nenhum parâmetro. O registro ADCSRA é utilizado, definindo o bit ADIE a interrupção volta a ocorrer.
void religar_adc()
{
	ADCSRA |= (1 << ADIE);
	comecar_adc_conversao();
}

/// Iniciar o sistema "ADC"

/// A função <void> adc_init é utilizada para iniciar os registros necessários para o bom funcionamento do conversor de valores analógicos. Não aceita nenhum parâmetro. O registro PRR é utilizado, definindo o bit PRADC ativamos a energia para esse sistema, o registro ADMUX é utilizado pra definirmos qual será a tensão de referência utilizada pelo sistema ADC nesse caso foi escolhido o referencial Vcc, o registro ADCSRA é utilizado para ativar o sistema ADC, definindo o bit ADEN ativamos a conversão ADC e o bit ADIE ativa a interrupção que é executada quando a conversão se finaliza, o registro ADCSRA também é utilizado para definir os bits ADPS0:2 definindo o prescaler para 128, a entrada digital é desativada com o registro DIDR0 para evitar flutuações no valor.
void adc_init()
{
	
	PRR &= ~(1 << PRADC);

	ADMUX |= (1 << REFS0);

	ADCSRA |= ((1 << ADEN) | (1 << ADIE));

	ADCSRA |= ((1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2));

	DIDR0 |= (1 << ADC0D);

	comecar_adc_conversao();
}

/// Interrupção ADC

/// A interrupção definida no sistema ADC está configurada para receber o valor do registro ADC que é composto de 16 bits. Se o valor ultrapassar 70 as rotinas desligar_adc e desligar_usart_interrução são desativadas para evitar conflitos com as rotinas posteriores no "loop" principal do programa. Além disso a variável de controle atingido é definida para verdadeira para o programa executar a rotina ao receber um "tiro". Caso o valor for inferior a 70 a variável atingido é definida para falso, para evitar a rotina errada, e recomeçamos a verificação de tiros reiniciando a conversão ADC.
ISR(ADC_vect)
{
	uint16_t valor = ADCL;
	valor += (ADCH << 8);

	if(valor > 70)
	{
		atingido = true;
		desligar_adc();
		desligar_usart_interrupcao();
	}
	else {
		atingido = false;
		comecar_adc_conversao();
	}

}

/// Interrupção USART

/// A interrupção definida no sistema de comunicação USART é usada para indentificar que um comando foi recebido e armazenar o valor do comando em questão. Assim que recebido o valor é atualizado para a direção do carrinho.
ISR(USART_RX_vect)
{
	letra_recebida = UDR0;
	dado_recebido = true;

	if(dado_recebido & !jogo_acabou)
	{
		atualizar_direcao(letra_recebida);
		dado_recebido = false;
	}
}

/// Iniciar o sistema USART

/// A função <void> usart_init é utilizada para iniciar os registros necessários para o funcionamento do protocolo de comunicação USART. A função aceita o seguinte parâmetro <uint32_t> baud que define a velocidade que o canal USART vai usar para se comunicar com o programa. O baud necessário para o registro UBRR0 é calculado utilizando a fórmula dada pela fabricante do microcontrolador atmega328p, o registro UCSR0B é utilizado, o bit RXEN0 ativa a recepção de valores pelo canal de comunicação, o bit RXCIE0 é utilizado para ativar a interrupção que acontece quando um dado é recebido, já o registro UCSR0C é utilizado, o bit UCSZ00:1 são utilizados para definir o tamanho do dado que será recebido.
void usart_init(uint32_t baud)
{
	uint8_t speed = 16;

	baud = (F_CPU/(speed*baud)) - 1;

	UBRR0H = (baud & 0x0F00) >> 8;
	UBRR0L = (baud & 0x00FF);

	UCSR0B |= ((1 << RXEN0) | (1 << RXCIE0));
	
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
}

/// Desligar a interrupção USART

/// A função <void> desligar_usart_interrupcao é utilizada para desabilitar as interrupções de um dado recebido pelo protocolo USART. Nenhum parâmetro é aceito. O registro UCSR0B é utilizado, limpando o bit RXCIE0 desabilitamos a interrupção. 
void desligar_usart_interrupcao()
{
	UCSR0B &= ~(1 << RXCIE0);
}

/// Ligar a interrupção USART

/// A função <void> ligar_usart_interrupcao é utilizada para habilitar as interrupções de um dado recebido pelo protocolo USART. Nenhum parâmetro é aceito. O registro UCSR0B é utilizado, definindo o bit RXCIE0 habilitamos a interrupção.
void ligar_usart_interrupcao()
{
	UCSR0B |= (1 << RXCIE0);
}

/// Configuração Inicial

/// A função <void> setup_inicial é utilizada para habilitar as interrupções globalmente. Definir a direção de cada pino utilizado no circuito. Iniciar os timers utilizados no programa. Iniciar a comunicação USART. Atualizar as vidas para o valor padrão. E iniciar o sistema ADC. Nenhum parâmetro é aceito.
void setup_inicial()
{
	WDTCSR = (1 << WDCE) | (0 << WDE);

	sei();

	DDRB |= ((1 << led3_pin) | (1 << direito_frente) | (1 << direito_traz) | (1 << esquerdo_frente));
	DDRD |= ((1 << led2_pin) | (1 << led1_pin) | (1 << laser_pin) | (1 << esquerdo_traz) | (1 << motor_esquerdo) | (1 << motor_direito));

	DDRB &= ~(1 << botao_reset);

	PORTB |= (1 << botao_reset);
	
	timer2_init_laser();
	timer1_init_delay();
	timer0_init_pwm();
	usart_init(BAUD);

	atualizar_vidas(quantidade_vidas);

	adc_init();
}

/// Resetar o sistema

/// A função <void> resetar_sistema é usada para resetar o sistema como um todo. o registro WDTCSR é utilizado pra ativar o watchdog e configurar um registro de 15ms para ele ser ativado.
void resetar_sistema()
{	
	WDTCSR = (1 << WDCE) | (1 << WDE);

    WDTCSR = (1 << WDE) | (1 << WDP0); 

    while (1) 
	{}
}

/// Função Principal

/// A função <int> main é utilizada para definir o comportamento principal do programa, ela chama a função setup_inicial e verifica constantemente se alguma variável de controle foi definida, ambas rotinas separadas por if's diferentes funcionam apenas se o valor de "jogo_acabou" estiver como falso. Caso contrário a primeira rotina é responsável por girar o carrinho, desabilitar os lasers e as interrupção que podem causar erro se forem executadas durante a mesma, já a segunda rotina é responsável por controlar o carrinho
int main()
{
	setup_inicial();

	while(1)
	{
		if(atingido & !jogo_acabou)
		{
			atualizar_direcao('s');
			atingido = false;
			estado_laser = false;

			quantidade_vidas--;

			atualizar_vidas(quantidade_vidas);
			atualizar_estado_laser(false);

			if(quantidade_vidas == 0)
			{
				jogo_acabou = true;
			}
			
			atualizar_direcao('t');
			atraso_segundos(2);

			atualizar_direcao('s');
			atraso_segundos(3);

			if(!jogo_acabou)
			{
				religar_adc();
				estado_laser = true;
			}
			ligar_usart_interrupcao();
    	}

		if(!(PINB & (1 << botao_reset)))
		{
			resetar_sistema();
		}

	}

	return 0;
}