
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
extern udpc_txq_t g_txq[UDPC_TXQ_LENGTH];

/** test data pkts - declared in another file */
extern data_pkt_t g_udpc_test_data[];

/** number of test data packets - declared in another file */
extern const int UDPC_TEST_DATA_PKTS_COUNT; 

/** --------------------- global, external variables - end   ---------------- */

/** --------------------- function declarations - start ---------------------- */
bool udpc_assignment1_main(handle_t *pc_handle);
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
/** --------------------- function declarations - end   ---------------------- */


/** --------------------- function definitions - start ---------------------- */


/**
 * UDP client assignment 1 main function
 */
bool udpc_assignment1_main(handle_t *pc_handle)
{

  data_pkt_t datapkt;
  int tx_bytes = 0;
  int i=0;
  int ret_val = 0;
  unsigned char ack_pending_ctr = 0; /* track number of pkts pending for ACK */



   /* Transmit 5 pkts at a time to server */
    for(i=0;i<UDPC_TEST_DATA_PKTS_COUNT;i++) {
      ret_val  = common_prepare_data_pkt(g_udpc_test_data[i].sop_id,
                                          g_udpc_test_data[i].client_id,
                                          g_udpc_test_data[i].pkt_type,
                                          g_udpc_test_data[i].segment_no,
                                          g_udpc_test_data[i].length,
                                          g_udpc_test_data[i].payload,
                                          g_udpc_test_data[i].eop_id, 
                                          &datapkt);
      tx_bytes = common_udp_send_pkt(UDP_CLIENT, pc_handle, 
                                  (char*)&datapkt, sizeof(datapkt));
      printf("\n---DEBUG-INFO---Transmitted Pkt:%.2d Segment#:%.3d tx_bytes:%.3d", 
                         i, datapkt.segment_no, tx_bytes);

      /* TODO: every iteration 1) take 5 pkts at a time and 2) clear entire txq */
      if ( tx_bytes > 0 ) {
        /* pkt TXed over network, Now save the packet in Transmit Q for 
             a) ACK verification
             b) retransmission purpose if needed */
        ret_val=udpc_txq_operation(UDPC_TXQ_ADD_PKT, 
                                       datapkt.segment_no, 
                                       (char*)&datapkt, tx_bytes,
                                       NULL,
                                       NULL);
        ack_pending_ctr++;
      } /* tx_bytes */

      /** add pkt to the Transmit Queue for ACK processing and retransmission, if needed */
      if (ack_pending_ctr >= 5) {
        printf("\n---DEBUG-INFO---Transmitted: %d pkts, Processing for ACK", ack_pending_ctr);
        /** dump entire txq */
        //ret_val = udpc_txq_operation(UDPC_TXQ_DUMP, 0, NULL, 0, NULL, NULL);
        ret_val = udpc_ack_timer_expiry(pc_handle, ack_pending_ctr);
         
        /** every time after receiving ACK/REJECT for last 5 pkts, 
         *    clear 'ack_pending_ctr' and 'g_txq' for next iteration */
        ack_pending_ctr = 0;
        /* initialize g_txq to 0 */
        memset(g_txq, 0, sizeof(g_txq));
      }
    } /* for */
  
   return true;
} /* udpc_assignment1_main */

/** --------------------- function definitions - end   ---------------------- */

