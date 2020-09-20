/**
 * @file
 * @brief Main entry point for the program
 *
 */

#include <string.h>

#include <lwip/api.h>

/* Inclue FreeRTOS Headers */
#include <FreeRTOS.h>
#include <task.h>

#include <FreeRTOS-Debug.h>

/* Configuration includes */
#include <globalConfig.h>
#include <port.h>

// #include "ptpd.h" /// @todo Improve ptpd public interface

/* Stack Overflow Handler - move elsewhere */
extern void vApplicationStackOverflowHook(
    xTaskHandle *pxTask,
    signed portCHAR *pcTaskName);

void vApplicationStackOverflowHook(
  xTaskHandle *pxTask __attribute((unused)),
  signed portCHAR *pcTaskName __attribute((unused))
) {
    for(;;);    // Loop forever here..
}

/* Task 1 - Blink System LED */
static void startTask1(void *args __attribute((unused))) {

    for (;;) {
        portSystemLEDToggle();
        vTaskDelay(100);
	}
}

/* Task 2 - Blink Status LED */
static void startTask2(void *args __attribute((unused))) {

    for (;;) {
        portStatusLEDToggle();
        vTaskDelay(500);
	}
}

/* Task 3 - Blink Warning LED */
// static void startTask3(void *args __attribute((unused))) {

//     debugFreeze();

//     portEthInit(); // Configure Ethernet GPIOs and registers

//     // ptpd_init();    // Initialise PTPd Daemon (not final interface)

//     struct netconn *conn;
//     char msg[] = "alpha";
//     struct netbuf *buf;
//     char * data;

//     conn = netconn_new(NETCONN_UDP);
//     netconn_bind(conn, IP_ADDR_ANY, 1234); //local port

//     netconn_connect(conn, IP_ADDR_BROADCAST, 1235);

//     for (;;) {
//         portWarningLEDToggle();
//         buf = netbuf_new();
//         data =netbuf_alloc(buf, sizeof(msg)); // NOT THREAD SAFE!!!!!
//         memcpy(data, msg, sizeof(msg));
//         netconn_send(conn, buf);
//         netbuf_delete(buf); // De-allocate packet buffer
//         vTaskDelay(10000);
// 	}
// }

/**
 * @brief This is the main entry point to the program
*/
int main(void) {

    portClockInit();  // Configure RCC, System Clock Tree, PLL, etc...

    portLEDInit();   // Set onboard LED GPIOs as outputs

    #if DEBUG_LEVEL >= DEBUG_ERRORS
        debug_ptr = debugInitialise(10, portSerialInitialise, portSerialSend);
    #endif /* DEBUG_LEVEL >= DEBUG_FULL */

    xTaskCreate(startTask1, "task1", 350, NULL, 5, NULL);
    xTaskCreate(startTask2, "task2", 350, NULL, 5, NULL);
    //xTaskCreate(startTask3, "lwip", 1024, NULL, FREERTOS_PRIORITIES-3, NULL);

    vTaskStartScheduler();

    /* This point is never reached, as the scheduler is blocking! */
    for (;;);
    return 0;
}