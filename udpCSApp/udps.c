
/* udp server code - main */

/** --------------------- header files inclusion - start ---------------------- */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
/** --------------------- header files inclusion - end ---------------------- */

/** --------------------- global, external variables - start ---------------- */
/** --------------------- global, external variables - end   ---------------- */

/** --------------------- function declarations - start ---------------------- */
extern unsigned int common_udp_create_connection(who_t who,
                                    handle_t *pHandle);
extern bool udps_assignment1_main(handle_t *ps_handle);
extern bool udps_assignment2_main(handle_t *ps_handle);
/** --------------------- function declarations - end   ---------------------- */


/** --------------------- function definitions - start ---------------------- */
/** --------------------- function definitions - end   ---------------------- */

int main(){

  int i;
  int ret_val = 0;
  handle_t s_handle;

  printf("\nUDP Server - START \n");
  memset(&s_handle, 0, sizeof(handle_t));
  ret_val = common_udp_create_connection(UDP_SERVER, &s_handle);
  printf("UDP Server connection create status: %d\n", ret_val);

#if ASSIGNMENT == 1
  printf("\n UDP Server(Assignment 1)\n");
  ret_val = udps_assignment1_main(&s_handle);
#elif ASSIGNMENT == 2
  printf("\n UDP Server(Assignment 2)\n");
  ret_val = udps_assignment2_main(&s_handle);
#endif
  printf("\nUDP Server - END \n");
  return 0;
} /* main */
