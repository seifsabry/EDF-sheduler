/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"
#include "queue.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

/*-----------------------------------------------------------*/

/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/


void Button_1_Monitor (void *pvParameters);
void Button_2_Monitor (void *pvParameters);
void Periodic_Transmitter (void *pvParameters);
void Uart_Receiver (void *pvParameters);
void Load_1_Simulation (void *pvParameters);
void Load_2_Simulation (void *pvParameters);


static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	//c.onfigTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}

QueueHandle_t xQueueButton1 = NULL;
QueueHandle_t xQueueButton2 = NULL;
QueueHandle_t xQueuePeriodicTransmiter = NULL;

	signed char *  Button_1_State;
	signed char *  Button_2_State;
	signed char *  PeriodicString;


/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	
	prvSetupHardware(); 
	
	xTaskPeriodicCreate( Button_1_Monitor, ( const char * ) "Button_1_Monitor",configMINIMAL_STACK_SIZE, NULL,1, NULL, pdMS_TO_TICKS(50) );
	xTaskPeriodicCreate( Button_2_Monitor, ( const char * ) "Button_1_Monitor",configMINIMAL_STACK_SIZE, NULL,1, NULL, pdMS_TO_TICKS(50) );
	xTaskPeriodicCreate( Periodic_Transmitter, ( const char * ) "Periodic_Transmitter",configMINIMAL_STACK_SIZE, NULL,1, NULL, pdMS_TO_TICKS(100) );
	xTaskPeriodicCreate( Uart_Receiver, ( const char * ) "Uart_Receiver",configMINIMAL_STACK_SIZE, NULL,1, NULL, pdMS_TO_TICKS(20) );
	xTaskPeriodicCreate( Load_1_Simulation, ( const char * ) "Load_1_Simulation",configMINIMAL_STACK_SIZE, NULL,1, NULL, pdMS_TO_TICKS(10) );
	xTaskPeriodicCreate( Load_2_Simulation, ( const char * ) "Load_2_Simulation",configMINIMAL_STACK_SIZE, NULL,1, NULL, pdMS_TO_TICKS(100) );
	
	xQueueButton1    = xQueueCreate( 1,sizeof(Button_1_State));
	xQueueButton2    = xQueueCreate( 1,sizeof(Button_2_State) );
	xQueuePeriodicTransmiter    = xQueueCreate( 1,sizeof(PeriodicString));
	
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	while(1)
		{}
}

void Button_1_Monitor (void *pvParameters)
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
	vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(50));
	GPIO_write(PORT_0, PIN0, PIN_IS_HIGH);
		
		if(GPIO_read(PORT_0, PIN6) == 0 )
		{
			Button_1_State = "Falling edge of button 1\n";
		}
		else if (GPIO_read(PORT_0, PIN6) == 1  )
		{
			Button_1_State = "Rising edge of button 1 \n";
		}
			
		xQueueSend (xQueueButton1,&Button_1_State,0);
		
		GPIO_write(PORT_0, PIN0, PIN_IS_LOW);
	}
}


void Button_2_Monitor (void *pvParameters)
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	
	while(1)
	{
		vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(50));
		
		GPIO_write(PORT_0, PIN1, PIN_IS_HIGH);
		
		if(GPIO_read(PORT_0, PIN7) == 0 )
		{
			Button_2_State = "Falling edge of button 2\n";
		}
		else if (GPIO_read(PORT_0, PIN7) == 1)
		{
			Button_2_State = "Rising edge of button 2 \n";
		}
		xQueueSend(xQueueButton2,&Button_2_State,0);
		GPIO_write(PORT_0, PIN1, PIN_IS_LOW);
		
	}
}


void Periodic_Transmitter (void *pvParameters)
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(100));
		
		GPIO_write(PORT_0, PIN2, PIN_IS_HIGH);
		
		PeriodicString = "periodic string every 100ms\n" ;
		
		xQueueSend (xQueuePeriodicTransmiter,&PeriodicString,0);
		
		GPIO_write(PORT_0, PIN2, PIN_IS_LOW);	
	}
}

signed char *  Button_1_buffer ;
signed char *  Button_2_buffer ;
signed char *  PeriodicStringBuffer ;

void Uart_Receiver (void *pvParameters)
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{		
		vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(20));
		
		GPIO_write(PORT_0, PIN3, PIN_IS_HIGH);
		
		xQueueReceive(xQueueButton1,&Button_1_buffer,0);
		xQueueReceive(xQueueButton2,&Button_2_buffer,0);
		xQueueReceive(xQueuePeriodicTransmiter,&PeriodicStringBuffer,0);
		
//		vSerialPutString(Button_1_buffer,sizeof(Button_1_buffer));
//		vSerialPutString(Button_2_buffer,sizeof(Button_2_buffer));
//		vSerialPutString(PeriodicStringBuffer,sizeof(PeriodicStringBuffer));
															 
		GPIO_write(PORT_0, PIN3, PIN_IS_LOW);
		
	}
}

void Load_1_Simulation (void *pvParameters)
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(10));
		
		GPIO_write(PORT_0, PIN4, PIN_IS_HIGH);
		
		int i;
		for(i=0;i<33200;i++)
		{i=i;}
		
		GPIO_write(PORT_0, PIN4, PIN_IS_LOW);
		
	}
}

void Load_2_Simulation (void *pvParameters)
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(100));
		
		GPIO_write(PORT_0, PIN5, PIN_IS_HIGH);
		int i;
		for(i=0;i<79680;i++)
		{i=i;}
		GPIO_write(PORT_0, PIN5, PIN_IS_LOW);
		
	}
}

void vApplicationIdleHook( void )
{
	GPIO_write(PORT_0, PIN8, PIN_IS_HIGH);
	GPIO_write(PORT_0, PIN8, PIN_IS_LOW);
}

 void vApplicationTickHook( void )
 {
	GPIO_write(PORT_0, PIN9, PIN_IS_HIGH);
	GPIO_write(PORT_0, PIN9, PIN_IS_LOW);
 }
