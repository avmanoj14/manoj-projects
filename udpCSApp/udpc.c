
/** udp client code - main */

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
udp_error_msgs_t g_udp_error_msgs[8] = 
{
  { 0xFFF4, "Segment Out of Sequence" },
  { 0xFFF5, "Packet Length mismatch"  },
  { 0xFFF6, "End of Packet missing"   },
  { 0xFFF7, "Duplicate packet"        },
  { 0xFFF8, "Access Permission"       },
  { 0xFFF9, "Not Paid         "       },
  { 0xFFFA, "Not exist        "       },
  { 0xFFFB, "Access_OK        "       }
};
const int UDP_MAX_ERRORS = (sizeof(g_udp_error_msgs)/sizeof(udp_error_msgs_t));

/** global transmit queue for holding complete packet for 
    ACK verification and retransmission purpose */
udpc_txq_t g_txq[UDPC_TXQ_LENGTH];

/** --------------------- global, external variables - end   ---------------- */

/** --------------------- function declarations - start ---------------------- */
unsigned int common_udp_create_connection(who_t who,
                                    handle_t *pHandle);
extern bool udpc_assignment1_main(handle_t *pc_handle);
extern bool udpc_assignment2_main(handle_t *pc_handle);
/** --------------------- function declarations - end   ---------------------- */


/** --------------------- function definitions - start ---------------------- */
/**
 * udpc transmit queue - dump, update, ack clear, retry update operations 
 */
bool udpc_txq_operation(udpc_txq_operation_t operation, 
                              unsigned char rcvd_segment_no,
                              char *pkt,
                              int pkt_length,
                              unsigned char *pack_pending_pkts_count,
                              handle_t *pc_handle
                              )
{
  int i = 0;
  bool ret_val = true;

  switch(operation) {
    case UDPC_TXQ_DUMP:
      printf("\nSegment_No ACK_Status Retry_Counter  Pkt   Pkt_length");
      for(i=0;i<UDPC_TXQ_LENGTH;i++) {
        printf("\n%.3d\t %.1d\t  %.3d\t %p\t %.4d",
             g_txq[i].segment_no,
             g_txq[i].is_ack_pending,
             g_txq[i].retry_ctr,
             g_txq[i].buffer,
             g_txq[i].length);
      } /* for */
      break;

    case UDPC_TXQ_ADD_PKT:
      ret_val = false;
      for(i=0;i<UDPC_TXQ_LENGTH;i++) {
        if(false == g_txq[i].is_ack_pending) {
          g_txq[i].segment_no = rcvd_segment_no;
          g_txq[i].retry_ctr = UDPC_MAX_RETRY_COUNT;
          g_txq[i].length = pkt_length;
          memcpy(&g_txq[i].buffer[0], pkt, pkt_length);
          g_txq[i].is_ack_pending = true;
          ret_val = true;
          break;
        } /* if */
      } /* for */
      break;


    case UDPC_TXQ_UPDATE_PENDING_ACK:
      for(i=0;i<UDPC_TXQ_LENGTH; i++) {
         if((true == g_txq[i].is_ack_pending) &&
            (rcvd_segment_no == g_txq[i].segment_no)) {
           (*pack_pending_pkts_count)--;
           g_txq[i].segment_no = 0x0;
           g_txq[i].retry_ctr = UDPC_MAX_RETRY_COUNT;
           g_txq[i].length = 0;
           memset(&g_txq[i].buffer[0], 0, sizeof(g_txq[i].buffer));
           g_txq[i].is_ack_pending = false;
         } /* if */
       } /* for */
       break;

    case UDPC_TXQ_UPDATE_RETRYCOUNT:
       for(i=0;i<UDPC_TXQ_LENGTH; i++) {
         if(true == g_txq[i].is_ack_pending) {
           if (0 != g_txq[i].retry_ctr) {
             g_txq[i].retry_ctr--;
             printf("\n Pkt with Segment#=%.3d Retransmission#=%d",
                            g_txq[i].segment_no, (UDPC_MAX_RETRY_COUNT - g_txq[i].retry_ctr));
             ret_val=common_udp_send_pkt(UDP_CLIENT, pc_handle, g_txq[i].buffer, g_txq[i].length);
           } else {
             printf("\nServer does not respond for Segment#=%.3d (max.retries hit)", g_txq[i].segment_no);
             (*pack_pending_pkts_count)--;
             g_txq[i].segment_no = 0x0;
             g_txq[i].retry_ctr = UDPC_MAX_RETRY_COUNT;
             g_txq[i].length = 0;
             memset(&g_txq[i].buffer[0], 0, sizeof(g_txq[i].buffer));
             g_txq[i].is_ack_pending = false;
           } /* if (0 != g_txq[i].retry_ctr) */
         } /* if(true == g_txq[i].is_ack_pending) */
       } /* for */
       break;
  } /* switch */

  return (ret_val);
} /* udpc_txq_operation */

/**-----------------------------------------------*/
/**
 * udpc ack timer expiry function for outstanding ACK pending pkts
 */
bool udpc_ack_timer_expiry(handle_t *pc_handle, 
                             unsigned char ack_pending_count)
{

  char buffer[1024];
  int nBytes = 0;
  int i = 0;
  int j = 0;
  int ret_val = 0;
  unsigned char ack_pending_pkts_count = ack_pending_count; 
  unsigned char rcvd_segment_no = 0; 
  struct timeval tv;

  ack_pkt_t *pack_pkt=NULL;
  reject_pkt_t *preject_pkt=NULL;
  access_pkt_t *paccess_pkt=NULL;
  fd_set rfds;
  
  while(ack_pending_pkts_count) {
  //printf("\n ack_pending_pkts_count = %d", ack_pending_pkts_count);

    /* Wait up to three seconds. */
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    FD_ZERO(&rfds);
    FD_SET(pc_handle->socket, &rfds);
    ret_val = select(((pc_handle->socket) + 1), &rfds, NULL, NULL, &tv);
    //printf("\n ack_pending_pkts_count = %d (after select)", ack_pending_pkts_count);

    if (ret_val == -1) {
       perror("select()");
    } else if (ret_val) {
       /** Data is available now */
       /* FD_ISSET(0, &rfds) will be true. */
       if (FD_ISSET(pc_handle->socket, &rfds)) {
         memset(buffer, 0, sizeof(buffer));
         nBytes = common_udp_recv_pkt(UDP_CLIENT, pc_handle, buffer,1024);
         //printf("\n Client RCVD RESP--> pkt=%d bytes=%d", i, nBytes);
         switch(nBytes) {
           case sizeof(ack_pkt_t):
             pack_pkt = (ack_pkt_t *)buffer;
             common_dump_ack_pkt(pack_pkt);
             rcvd_segment_no = pack_pkt->rcvd_segment_no;
             break;

           case sizeof(reject_pkt_t):
             preject_pkt = (reject_pkt_t *)buffer;
             common_dump_reject_pkt(preject_pkt);
             rcvd_segment_no = preject_pkt->rcvd_segment_no;

             /* decode reject sub code to 'reject message' */
             for(j=0;j<UDP_MAX_ERRORS;j++) {
                if (g_udp_error_msgs[j].code == preject_pkt->reject_sub_code) {
                  printf(" %s ", g_udp_error_msgs[j].msg);
                  break;
                } /* if */
             } /* for */
             break;

           case sizeof(access_pkt_t):
             paccess_pkt = (access_pkt_t *)buffer;
             common_dump_access_pkt(paccess_pkt);
             rcvd_segment_no = paccess_pkt->segment_no;

             /* decode reject sub code to 'reject message' */
             for(j=0;j<UDP_MAX_ERRORS;j++) {
                if (g_udp_error_msgs[j].code == paccess_pkt->pkt_type) {
                  printf(" %s ", g_udp_error_msgs[j].msg);
                  break;
                } /* if */
             } /* for */
             break;

           default:
             common_dump_buffer(buffer, nBytes);
             break;
         } /* switch*/
       } /* if FD_ISSET */
       //printf("\nResponse received for segment_no=%d", rcvd_segment_no);
       /* update the g_txq and reduce 'ack_pending_pkts_count' */
       ret_val = udpc_txq_operation(UDPC_TXQ_UPDATE_PENDING_ACK, 
                                      rcvd_segment_no, 
                                      NULL, 0, 
                                      &ack_pending_pkts_count,
                                      NULL);
    } else {
       //printf("No data within three seconds, retransmitting all the ack pending pkts..\n");
       ret_val = udpc_txq_operation(UDPC_TXQ_UPDATE_RETRYCOUNT, 
                                      0, 
                                      NULL, 0, 
                                      &ack_pending_pkts_count,
                                      pc_handle);
    } /* else */
  } /* while */
} /* udpc_ack_timer_expiry */


/** --------------------- function definitions - end   ---------------------- */
int main(){
  int nBytes;
  char buffer[1024];

  handle_t c_handle;
  bool ret_val = true;
  bool is_complete = false;


  printf("\nUDP Client ---- START \n");
  memset(&c_handle, 0, sizeof(handle_t));

  ret_val = common_udp_create_connection(UDP_CLIENT, &c_handle);
  printf("\nudpc_create_connection status: %d\n", ret_val);

  is_complete = false;
  while(!is_complete){
#if ASSIGNMENT == 1
    ret_val = udpc_assignment1_main(&c_handle);
    printf("\nudpc_assignment1_main status = %d", ret_val);
#elif ASSIGNMENT == 2
    ret_val = udpc_assignment2_main(&c_handle);
    printf("\nudpc_assignment2_main status = %d", ret_val);
#endif
    is_complete = true;
  } /* while */

  printf("\nUDP Client ---- END \n");
  return 0;
} /* main */
