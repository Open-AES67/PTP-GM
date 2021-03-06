/**
 * @file
 * @brief Main entry point for the program
 *
 */

#include <string.h>

#include <lwip/api.h>

/* Inclue FreeRTOS headers */
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

/* Include FreeRTOS-Debug library headers */
#include <FreeRTOS-Debug.h>

/* Configuration includes */
#include <globalConfig.h>

/* Port functions */
#include <port/portCore.h>
#include <port/portEthernet.h>
#include <port/portLeds.h>
#include <port/portSerial.h>

#include <stdio.h>

#include <lwip-ptp.h>

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

// static void print_time(TimerHandle_t timer) {
//     timestamp_t time;
//     LWIP_PTP_GET_TIME(&time);
//     printf("ptp: s - %lu - ns - %lu\n", time.secondsField.lsb,
//                                             time.nanosecondsField);
// }
#define PTP_TO_NSEC(SUBSEC)     (u32_t)(((uint64_t)(SUBSEC) * 1000000000ll)  \
                                                                        >> 31)

#define PTP_TO_SUBSEC(NSEC)     (u32_t)(((uint64_t)(NSEC) * 0x80000000ll) \
                                                                 / 1000000000)

/* Task 1 - Blink System LED */
static void startTask1(void *args __attribute((unused))) {
    // TimerHandle_t timer;

    // timer = xTimerCreate("stamp", 1000, pdTRUE, NULL, print_time);
    // xTimerStart(timer, portMAX_DELAY);

    for (;;) {
        portSystemLEDToggle();
        // unsigned int i = (portReadInput(3) - 2048) * 2750;
        // ptp_update_fine(i);
        // printf('ADJ: %u\n', i);
        // timestamp_t time;
        // time.secondsField.lsb = 1606244247;
        // time.nanosecondsField = 504000400;
        // printf("TNew: s - %lu - ns - %lu\n", time.secondsField.lsb, time.nanosecondsField);
        // time.nanosecondsField = PTP_TO_NSEC(PTP_TO_SUBSEC(time.nanosecondsField));
        // // ptp_set_time(&time);
        // printf("Time: s - %lu - ns - %lu\n", time.secondsField.lsb, time.nanosecondsField);
        // ptp_get_time(&time);
        vTaskDelay(1000);
    }
}

// /* Task 2 - Blink Status LED */
// static void startTask2(void *args __attribute((unused))) {

//     for (;;) {
//         portStatusLEDToggle();
//         // DEBUG_MESSAGE(DEBUG_TYPE_INFO, "Hi World!!!");
//         vTaskDelay(20000);
//     }
// }

/* Task 3 - Blink Warning LED */
static void startTask3(void *args __attribute((unused))) {

    portEthInit(); // Configure Ethernet GPIOs and registers

    struct netconn *conn;
    char msg[] = "alpha";
    struct netbuf *buf;
    char * data;

    // ip_addr_t dest;
    // dest.addr = htonl(0xc0a82a02); // 192.168.42.2

    conn = netconn_new(NETCONN_UDP);
    netconn_bind(conn, IP_ADDR_ANY, 80); //local port

    netconn_connect(conn, IP_ADDR_BROADCAST, 8080);

    for (;;) {
        portWarningLEDToggle();
        buf = netbuf_new();
        data = netbuf_alloc(buf, sizeof(msg));
        memcpy(data, msg, sizeof(msg));
        netconn_send(conn, buf);
        netbuf_delete(buf); // De-allocate packet buffer
        vTaskDelay(10000);
    }
}

/**
 * @brief This is the main entry point to the program
*/
int main(void) {

    portClockInit();  // Configure RCC, System Clock Tree, PLL, etc...

    portLEDInit();   // Set onboard LED GPIOs as outputs

    #if DEBUG_LEVEL >= DEBUG_ERRORS
    debugInitialise(10, portSerialInitialise, portSerialSend, portSystemReset);
    #endif /* DEBUG_LEVEL >= DEBUG_FULL */

    xTaskCreate(startTask1, "task1", 350, NULL, 5, NULL);
    //xTaskCreate(startTask2, "task2", 350, NULL, 5, NULL);
    xTaskCreate(startTask3, "network", 1024, NULL, FREERTOS_PRIORITIES-3, NULL);

    vTaskStartScheduler();

    /* This point is never reached, as the scheduler is blocking! */
    for (;;);
    return 0;
}
