
/** udp client code - assignment 1*/

/** --------------------- header files inclusion - start ---------------------- */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "common.h"
#include "udpc.h"
/** --------------------- header files inclusion - end ---------------------- */

/** --------------------- global, external variables - start ---------------- */

/** error codes and respective messages */
extern udp_error_msgs_t g_udp_error_msgs[]; 
extern const int UDP_MAX_ERRORS;

/** global transmit queue for holding complete packet for 
    ACK verification and retransmission purpose */
udpc_txq_t g_txq[UDPC_TXQ_LENGTH];

/** test data pkts - declared in another file */
extern access_pkt_t g_udpc_test_access_data[];

/** number of test data packets - declared in another file */
extern const int UDPC_TEST_ACCESS_PKTS_COUNT; 

/** --------------------- global, external variables - end   ---------------- */

/** --------------------- function declarations - start ---------------------- */
extern bool udpc_txq_operation(udpc_txq_operation_t operation, 
                              unsigned char rcvd_segment_no,
                              char *pkt,
                              int pkt_length,
                              unsigned char *pack_pending_pkts_count,
                              handle_t *pc_handle
                              );

extern bool udpc_ack_timer_expiry(handle_t *pc_handle, 
                             unsigned char ack_pending_count);

extern bool udpc_create_connection(handle_t *pc_handle);

bool udpc_assignment2_main(handle_t *pc_handle);
/** --------------------- function declarations - end   ---------------------- */


/** --------------------- function definitions - start ---------------------- */

/**
 * UDP client assignment 2 main function
 */
bool udpc_assignment2_main(handle_t *pc_handle)
{

  access_pkt_t accesspkt;
  int tx_bytes = 0;
  int i=0;
  int ret_val = 0;
  unsigned char ack_pending_ctr = 0; /* track number of pkts pending for ACK */



   /* Transmit 1 pkts at a time to server */
    for(i=0;i<UDPC_TEST_ACCESS_PKTS_COUNT;i++) {
      ret_val  = common_prepare_access_pkt(g_udpc_test_access_data[i].sop_id,
                                          g_udpc_test_access_data[i].client_id,
                                          g_udpc_test_access_data[i].pkt_type,
                                          g_udpc_test_access_data[i].segment_no,
                                          g_udpc_test_access_data[i].length,
                                          g_udpc_test_access_data[i].payload_technology,
                                          g_udpc_test_access_data[i].payload_ss_no,
                                          g_udpc_test_access_data[i].eop_id, 
                                          &accesspkt);
      tx_bytes = common_udp_send_pkt(UDP_CLIENT, pc_handle, 
                                  (char*)&accesspkt, sizeof(accesspkt));
      printf("\n---DEBUG-INFO---Transmitted Pkt:%.2d Segment#:%.3d tx_bytes:%.3d", 
                         i, accesspkt.segment_no, tx_bytes);

      /* TODO: every iteration 1) take 1 pkts at a time and 2) clear entire txq */
      if ( tx_bytes > 0 ) {
        /* pkt TXed over network, Now save the packet in Transmit Q for 
             a) ACK verification
             b) retransmission purpose if needed */
        ret_val=udpc_txq_operation(UDPC_TXQ_ADD_PKT, 
                                       accesspkt.segment_no, 
                                       (char*)&accesspkt, tx_bytes,
                                       NULL,
                                       NULL);
        ack_pending_ctr++;
      } /* tx_bytes */

      /** add pkt to the Transmit Queue for ACK processing and retransmission, if needed */
      if (ack_pending_ctr >= 1) {
        printf("\n---DEBUG-INFO---Transmitted: %d pkts, Processing for ACK\n", ack_pending_ctr);
        /** dump entire txq */
        //ret_val = udpc_txq_operation(UDPC_TXQ_DUMP, 0, NULL, 0, NULL, NULL);
        ret_val = udpc_ack_timer_expiry(pc_handle, ack_pending_ctr);
         
        /** every time after receiving ACK/REJECT for last 5 pkts, 
         *    clear 'ack_pending_ctr' and 'g_txq' for next iteration */
        ack_pending_ctr = 0;
        /* initialize g_txq to 0 */
        memset(g_txq, 0, sizeof(g_txq));
      }
      printf("\nPress any key to Continue,...");
      getchar();
    } /* for */
  
   return true;
} /* udpc_assignment2_main */

/** --------------------- function definitions - end   ---------------------- */

