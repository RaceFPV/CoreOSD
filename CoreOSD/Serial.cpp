#include "Arduino.h"
#include "Serial.h"

static volatile uint8_t serialHeadRX[UART_NUMBER],serialTailRX[UART_NUMBER];
static uint8_t serialBufferRX[RX_BUFFER_SIZE][UART_NUMBER];
static volatile uint8_t serialHeadTX[UART_NUMBER],serialTailTX[UART_NUMBER];
static uint8_t serialBufferTX[TX_BUFFER_SIZE][UART_NUMBER];

// *******************************************************
// Interrupt driven UART transmitter - using a ring buffer
// *******************************************************


ISR(USART_UDRE_vect) {  // Serial 0 on a PROMINI
		uint8_t t = serialTailTX[0];
		if (serialTailTX[0] == serialHeadTX[0]) { 
			UCSR0B &= ~(1<<UDRIE0); // Check if all data is transmitted . if yes disable transmitter UDRE interrupt
		}
		else {
			unsigned char c = serialBufferTX[t][0];;
			serialTailTX[0] = (serialTailTX[0]+1) % TX_BUFFER_SIZE;
			UDR0 = c;  // Transmit next byte in the ring
		}
}

void UartSendData(uint8_t port) {
		UCSR0B |= (1<<UDRIE0);
}

void SerialOpen(uint8_t port, uint32_t baud) {
		//uint8_t h = ((F_CPU  / 4 / baud -1) / 2) >> 8;
		uint8_t l = ((F_CPU  / 4 / baud -1) / 2);
		UCSR0A  = (1<<U2X0); 
		UBRR0H = l >> 8; 
		UBRR0L = l; 
		UCSR0B |= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
		UCSR0B &= ~(1<<UDRIE0);
}

void SerialEnd(uint8_t port) {
		while (serialHeadRX[port] != serialTailRX[port])
		;
		UCSR0B &= ~((1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<UDRIE0));
		serialHeadRX[port] = serialTailRX[port];
}

void store_uart_in_buf(uint8_t data, uint8_t portnum) {

		uint8_t h = serialHeadRX[portnum];
		if (++h >= RX_BUFFER_SIZE) h = 0;
		if (h == serialTailRX[portnum]) return; // we did not bite our own tail?
		serialBufferRX[serialHeadRX[portnum]][portnum] = data;
		serialHeadRX[portnum] = h;
}

//ISR(USART_RX_vect)  { store_uart_in_buf(UDR0, 0); }
		
ISR(USART_RX_vect)  { 
		if (bit_is_clear(UCSR0A, UPE0)) {
			store_uart_in_buf(UDR0, 0);
		} else {
		    unsigned char c = UDR0;
		}
}	


uint8_t SerialRead(uint8_t port) {
		
		uint8_t t = serialTailRX[port];
		uint8_t c = serialBufferRX[t][port];
		if (serialHeadRX[port] != t) {
			if (++t >= RX_BUFFER_SIZE) t = 0;
			serialTailRX[port] = t;
		}
		return c;
}

uint8_t SerialAvailable(uint8_t port) {
		return ((uint8_t)(serialHeadRX[port] - serialTailRX[port]))%RX_BUFFER_SIZE;
}

uint8_t SerialUsedTXBuff(uint8_t port) {
		return ((uint8_t)(serialHeadTX[port] - serialTailTX[port]))%TX_BUFFER_SIZE;
}

void SerialSerialize(uint8_t port,uint8_t a) {
		uint8_t t = serialHeadTX[port];
		if (++t >= TX_BUFFER_SIZE) t = 0;
		serialBufferTX[t][port] = a;
		serialHeadTX[port] = t;
}

void SerialWrite(uint8_t port,uint8_t c){
		SerialSerialize(port,c);UartSendData(port);
}	

void SerialFlush(uint8_t port){
		  while (serialHeadRX[port] != serialTailRX[port])
		  ;
}
