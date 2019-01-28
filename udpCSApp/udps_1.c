
/* udp server code - assignment 1*/

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
static bool udps_send_reply_pkt(handle_t *ps_handle, 
                                 data_pkt_t *pRcvd_data_pkt, 
                                 unsigned short int server_reply_code);

static unsigned short int udps_validate_data_pkt(char *buffer, 
                                            int nBytes, 
                                            unsigned char expected_seg_no);

bool udps_assignment1_main(handle_t *ps_handle);
/** --------------------- function declarations - end   ---------------------- */


/** --------------------- function definitions - start ---------------------- */

/**
 * function to send reply to client. i.e. either ACK pkt or REJECT pkt
 */
static bool udps_send_reply_pkt(handle_t *ps_handle, 
                                 data_pkt_t *pRcvd_data_pkt, 
                                 unsigned short int server_reply_code)
{
  ack_pkt_t    ack_pkt;
  reject_pkt_t reject_pkt;
  int nBytes = 0;
  bool status = true;

  switch(server_reply_code) {
    case 0x00:
      status = common_prepare_ack_pkt(pRcvd_data_pkt->client_id, 
                                  pRcvd_data_pkt->segment_no,
                                  &ack_pkt);
      if (true == status) { 
        nBytes = common_udp_send_pkt(UDP_SERVER, ps_handle, 
                                    (void*)&ack_pkt, sizeof(ack_pkt_t));
      }
      break;

    case REJ_SUBCODE_OUT_OF_SEQUENCE:
    case REJ_SUBCODE_LENGTH_MISSMATCH:
    case REJ_SUBCODE_EOP_MISSING:
    case REJ_SUBCODE_DUPLICATE_PACKET:
      status = common_prepare_reject_pkt(pRcvd_data_pkt->client_id,
                                     server_reply_code,
                                     pRcvd_data_pkt->segment_no,
                                     &reject_pkt);
      if (true == status) {
        nBytes = common_udp_send_pkt(UDP_SERVER, ps_handle, 
                                     (void*)&reject_pkt,sizeof(reject_pkt_t));
      }
      break;
  } /* switch */

} /* udps_send_reply_pkt */


/**
 * function to validate the received packet and identify error code if needed
 */
static unsigned short int udps_validate_data_pkt(char *buffer, 
                                            int nBytes, 
                                            unsigned char expected_seg_no)
{
  data_pkt_t *rcvd_pkt;
  
  if ( (buffer == NULL) || (nBytes == 0) || (expected_seg_no == 0) ) {
    printf("\r\n server reject at 1");
    return REJ_SUBCODE_LENGTH_MISSMATCH;
  } 

  rcvd_pkt = (data_pkt_t *)buffer;
  /*------ sequence number verification - Start ----------*/
  if (expected_seg_no > rcvd_pkt->segment_no) { 
    /* case-4: old pkt has come once again i.e. duplicate */
    printf("\n---DEBUG-INFO--- server reject at 2");
    return REJ_SUBCODE_DUPLICATE_PACKET;
  } else if (expected_seg_no < rcvd_pkt->segment_no) {
    /* case-1: out of seqeunce */
    printf("\n---DEBUG-INFO--- server reject at 3 exp_seg_no=%d Rcvd_seg_no=%d", 
                                   expected_seg_no, rcvd_pkt->segment_no);
    return REJ_SUBCODE_OUT_OF_SEQUENCE;
  } else {
    /* Sequence number looks OK*/
    //server_error_code = 0x0;
  } 
  /*------ sequence number verification - End ----------*/

  /*------ End of Packet Id verification - Start ----------*/
  if (rcvd_pkt->eop_id != EOP_ID) {
    /* case-3: no EOP_ID in pkt */
    printf("\n---DEBUG-INFO--- server reject at 4");
    return REJ_SUBCODE_EOP_MISSING;
  }
  /*------ End of Packet Id verification - End ----------*/

  /*------ packet length and payload verification - Start ----------*/
  if ( strlen(rcvd_pkt->payload) != rcvd_pkt->length ) {
    /* case-2: rcvd pkt length and payload mismatch */
    printf("\n---DEBUG-INFO--- server reject at 5 nBytes=%d rcvdpkt_length=%d payload_size=%zd", 
                 nBytes, rcvd_pkt->length, strlen(rcvd_pkt->payload));
    return REJ_SUBCODE_LENGTH_MISSMATCH;
  }
  /*------ packet length and payload verification - Start ----------*/

  /** looks like no error, hence return 0x0 */
  return 0x0;
} /* udps_validate_data_pkt */

/**
 * UDP assignment1 main functionality */
bool udps_assignment1_main(handle_t *ps_handle)
{

  int nBytes;
  char buffer[1024];
  data_pkt_t *pRcvd_data_pkt;
  unsigned char expected_seg_no = 1;
  unsigned short int server_reply_code = 0;
  bool is_complete = false;

  is_complete = false;
  while(!is_complete){
    #ifdef UDPS_WITH_SLEEP 
      /* to simulate retransmissions from client */
      sleep(10);
    #endif
    nBytes = common_udp_recv_pkt(UDP_SERVER, ps_handle, buffer,1024);

    /** debug purpose - start */
    pRcvd_data_pkt = (data_pkt_t*)buffer;
    printf("\nUDPS RCVD PKT: ");
    common_dump_data_pkt(pRcvd_data_pkt);
    /** debug purpose - end */

    server_reply_code = udps_validate_data_pkt(buffer, nBytes, expected_seg_no);
    nBytes = udps_send_reply_pkt(ps_handle, pRcvd_data_pkt, server_reply_code);
    if (0x0 == server_reply_code) { 
      expected_seg_no++;
      if (expected_seg_no == 255) { expected_seg_no = 0x1; }
    }

    //is_complete = true;
  } /* while */
  return true; 
} /* udps_assignment1_main */


/** --------------------- function definitions - end   ---------------------- */
