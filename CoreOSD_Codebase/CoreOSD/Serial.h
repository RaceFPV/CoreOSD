#ifndef SERIAL_H_
#define SERIAL_H_

#define UART_NUMBER 1
#define RX_BUFFER_SIZE 64
#define TX_BUFFER_SIZE 128

void    SerialOpen(uint8_t port, uint32_t baud);
uint8_t SerialRead(uint8_t port);
void    SerialWrite(uint8_t port,uint8_t c);
uint8_t SerialAvailable(uint8_t port);
void    SerialEnd(uint8_t port);
uint8_t SerialPeek(uint8_t port);
bool    SerialTXfree(uint8_t port);
uint8_t SerialUsedTXBuff(uint8_t port);
void    SerialSerialize(uint8_t port,uint8_t a);
void    UartSendData(uint8_t port);
void	SerialFlush(uint8_t port);
	
#endif /* SERIAL_H_ */
