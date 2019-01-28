
/** --------------------- header files inclusion - start ---------------------- */
#include <stdio.h>
#include <string.h>
#include "common.h"
/** --------------------- header files inclusion - end ---------------------- */

/** --------------------- function definitions - start ---------------------- */


/**-----------------------------------------------------------------------
 *        COMMON UDP CONNECTION CREATION (used from Client & Server)
 **-----------------------------------------------------------------------*/
unsigned int common_udp_create_connection(who_t who, 
                                    handle_t *pHandle) 
{
  bool status = false;

  if (pHandle == NULL) {return status;}

  pHandle->socket = socket(PF_INET, SOCK_DGRAM, 0);
  if (-1 != pHandle->socket) {
    pHandle->server_addr.sin_family = AF_INET;
    pHandle->server_addr.sin_port = htons(UDP_PORT);
    pHandle->server_addr.sin_addr.s_addr = inet_addr(UDP_IPADDR);
    memset(pHandle->server_addr.sin_zero, '\0', sizeof pHandle->server_addr.sin_zero);

    if (UDP_CLIENT == who) {
      /*Initialize size variable to be used later on*/
      pHandle->addr_size = sizeof pHandle->server_addr;
      status = true;
    } /* UDP_CLIENT */

    if (UDP_SERVER == who) {
      /*Bind socket with address struct*/
      bind(pHandle->socket, (struct sockaddr *) &pHandle->server_addr, sizeof(pHandle->server_addr));

      /*Initialize size variable to be used later on*/
      pHandle->addr_size = sizeof (struct sockaddr_storage);
      status = true;
    } /* UDP_SERVER */
  } /* if (-1 != pHandle->socket) */
  return status;
} /* common_udp_create_connection */

/**-----------------------------------------------------------------------
 *        COMMON UDP SEND PKT  (used from Client & Server)
 **-----------------------------------------------------------------------*/
unsigned int common_udp_send_pkt(who_t who, 
                                    handle_t *pHandle, 
                                    void *pkt, 
                                    unsigned int pkt_length)
{
   int sent_bytes = 0;

   switch(who) {
     case UDP_CLIENT:
      //sendto(clientSocket, (char*)&datapkt, sizeof(datapkt), 0, (struct sockaddr *)&serverAddr,addr_size);
      sent_bytes = sendto(pHandle->socket, 
                             (char*)pkt, pkt_length, 
                             0, 
                             (struct sockaddr *)&(pHandle->server_addr), 
                             pHandle->addr_size);
     break;

     case UDP_SERVER:
      sent_bytes = sendto(pHandle->socket, 
                             (char*)pkt, pkt_length, 
                             0, 
                             (struct sockaddr *)&(pHandle->server_storage), 
                             pHandle->addr_size);
     break;
  } 
  return sent_bytes;
} /* common_udp_send_pkt */

/**-----------------------------------------------------------------------
 *        COMMON UDP RECEIVE PKT  (used from Client & Server)
 **-----------------------------------------------------------------------*/
unsigned int common_udp_recv_pkt(who_t who, 
                                   handle_t *pHandle, 
                                   void *buffer, 
                                   unsigned int buffer_length)
{
   int rcvd_bytes = 0;

   switch(who) {
     case UDP_CLIENT:
      //nBytes = recvfrom(clientSocket,buffer,1024,0,NULL, NULL);
      rcvd_bytes = recvfrom(pHandle->socket, 
                             (char*)buffer, buffer_length, 
                             0, 
                             NULL,
                             NULL);
     break;

     case UDP_SERVER:
      //nBytes = recvfrom(udpSocket,buffer,1024,0,(struct sockaddr *)&server_storage, &addr_size);
      rcvd_bytes = recvfrom(pHandle->socket, 
                             (char*)buffer, buffer_length, 
                             0, 
                             (struct sockaddr *)&(pHandle->server_storage), 
                             &(pHandle->addr_size));
     break;
  } 
  return rcvd_bytes;
} /* common_udp_recv_pkt */

/**-----------------------------------------------------------------------
 *        COMMON DUMP BUFFER 
 **-----------------------------------------------------------------------*/
void common_dump_buffer(char *buffer, unsigned int length) 
{
  unsigned int i = 0;
#if 0
  printf("\n[D PKT]SOP=%.4X CID=%.3u TYPE=%.4X SEG#=%.3u Len=%.3u Payload={",
        datapkt.sop_id, datapkt.client_id, datapkt.data, 
        datapkt.segment_no, datapkt.length);
#endif
  printf("\nERROR PKT DUMP={ ");
  printf("%.2hhx%.2hhx %.2hhx %.2hhx%.2hhx", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
  printf(" %.2hhx %.2hhx  ", buffer[5], buffer[6]);

  printf("\n");
  for(i=7;i<30/*length*/;i++) printf(" %.2hhx", buffer[i]);
  printf("}\n ");

} /* common_dump_buffer */

/**=========================================================================
 *        ASSIGNMENT 1  COMMON FUNCTIONS  - START
 **=========================================================================*/

/**-----------------------------------------------------------------------
 *        COMMON DUMP DATA PACKET
 **-----------------------------------------------------------------------*/
void common_dump_data_pkt(void *pBuffer) 
{
  unsigned int i = 0;
  data_pkt_t *pdatapkt;

  pdatapkt = (data_pkt_t *)pBuffer;
  printf("[D PKT]SOP=%.4X CID=%.3u TYPE=%.4X SEG#=%.3u Len=%.3u payload_size=%zd Payload={",
        pdatapkt->sop_id, pdatapkt->client_id, pdatapkt->pkt_type, 
        pdatapkt->segment_no, pdatapkt->length, strlen(pdatapkt->payload));

  for(i=0;i<pdatapkt->length;i++) printf("%c", pdatapkt->payload[i]);

  printf("} EOP=%.4X", pdatapkt->eop_id);
} /* common_dump_data_pkt */

/**-----------------------------------------------------------------------
 *        COMMON PREPARE DATA PACKET
 **-----------------------------------------------------------------------*/
bool common_prepare_data_pkt(unsigned short int sop_id, 
                        unsigned char client_id, 
                        unsigned short int pkt_type,
                        unsigned char segment_no,
                        unsigned char length,
                                 char *payload,
                        unsigned short int eop_id,
                            data_pkt_t *out_datapkt)
{

  if (out_datapkt == NULL) {
     return false;
  }

  out_datapkt->sop_id     = sop_id; 
  out_datapkt->client_id  = client_id;
  out_datapkt->pkt_type   = pkt_type;
  out_datapkt->segment_no = segment_no;
  out_datapkt->length     = length; 
  strcpy(&out_datapkt->payload[0], payload);
  out_datapkt->eop_id     = eop_id;

  return true;

} /* common_prepare_data_pkt2 */


/**-----------------------------------------------------------------------
 *        COMMON DUMP ACK PACKET
 **-----------------------------------------------------------------------*/
void common_dump_ack_pkt(void *pBuffer) 
{
  ack_pkt_t *packpkt;
  packpkt = (ack_pkt_t*)pBuffer;
  printf("\n[A PKT]SOP=%.4X CID=%.3u TYPE=%.4X RCVD.SEG#=%.3u EOP=%.4X",
        packpkt->sop_id, packpkt->client_id, packpkt->pkt_type, 
        packpkt->rcvd_segment_no, packpkt->eop_id);
} /* common_dump_ack_pkt */

/**-----------------------------------------------------------------------
 *        COMMON PREPARE ACK PACKET
 **-----------------------------------------------------------------------*/
bool common_prepare_ack_pkt(unsigned char client_id, 
                       unsigned char rcvd_segment_no, 
                       ack_pkt_t *out_ackpkt)
{
  if (out_ackpkt == NULL) { return false; }

  out_ackpkt->sop_id          = SOP_ID;
  out_ackpkt->client_id       = client_id;
  out_ackpkt->pkt_type        = PKT_TYPE_ACK;
  out_ackpkt->rcvd_segment_no = rcvd_segment_no;
  out_ackpkt->eop_id          = EOP_ID;
  return true;
} /* common_prepare_ack_pkt */


/**-----------------------------------------------------------------------
 *        COMMON REJECT DUMP PACKET
 **-----------------------------------------------------------------------*/
void common_dump_reject_pkt(void *pBuffer) 
{
  reject_pkt_t *prejectpkt;
  prejectpkt = (reject_pkt_t*)pBuffer;

  printf("\n[R PKT]SOP=%.4X CID=%.3u TYPE=%.4X RejectSubcode=%.4X RCVD.SEG#=%.3u EOP=%.4X",
        prejectpkt->sop_id, prejectpkt->client_id, prejectpkt->pkt_type, 
        prejectpkt->reject_sub_code, prejectpkt->rcvd_segment_no, prejectpkt->eop_id);
} /* common_dump_reject_pkt */

/**-----------------------------------------------------------------------
 *        COMMON PREPARE REJECT PACKET
 **-----------------------------------------------------------------------*/
bool common_prepare_reject_pkt(unsigned char client_id, 
                          unsigned short int reject_sub_code,
                          unsigned char rcvd_segment_no, 
                          reject_pkt_t *out_rejectpkt)
{

  if (out_rejectpkt == NULL) { return false;}

  out_rejectpkt->sop_id          = SOP_ID;
  out_rejectpkt->client_id       = client_id;
  out_rejectpkt->pkt_type        = PKT_TYPE_REJ;
  out_rejectpkt->reject_sub_code = reject_sub_code;
  out_rejectpkt->rcvd_segment_no = rcvd_segment_no;
  out_rejectpkt->eop_id          = EOP_ID;
  return true;
} /* common_prepare_ack_pkt */
/**=========================================================================
 *        ASSIGNMENT 1  COMMON FUNCTIONS  - END
 **=========================================================================*/

/**=========================================================================
 *        ASSIGNMENT 2  COMMON FUNCTIONS  - START
 **=========================================================================*/
bool common_prepare_access_pkt(unsigned short int sop_id,
                        unsigned char client_id,
                        unsigned short int pkt_type,
                        unsigned char segment_no,
                        unsigned char length,
                        unsigned char payload_technology,
                        unsigned int  payload_ss_no,     
                        unsigned short int eop_id,     
                            access_pkt_t *out_accesspkt)
{
  if (out_accesspkt == NULL) { return false;}

  out_accesspkt->sop_id             = SOP_ID;
  out_accesspkt->client_id          = client_id;
  out_accesspkt->pkt_type           = pkt_type;
  out_accesspkt->segment_no         = segment_no;
  out_accesspkt->length             = sizeof(out_accesspkt->payload_technology) + 
                                       sizeof(out_accesspkt->payload_ss_no) ;
  out_accesspkt->payload_technology = payload_technology;
  out_accesspkt->payload_ss_no      = payload_ss_no;
  out_accesspkt->eop_id             = EOP_ID;
  return true;
} /* common_prepare_access_pkt */

void common_dump_access_pkt(void *pBuffer) 
{
  access_pkt_t *paccesspkt;

  paccesspkt = (access_pkt_t *)pBuffer;
  printf("[ACCESS PKT]SOP=%.4X CID=%.3u TYPE=%.4X SEG#=%.3u Len=%.3u payload{technology=%.2u SrcSubscriberNo=%.10u} EOP=%.4X",
        paccesspkt->sop_id, paccesspkt->client_id, paccesspkt->pkt_type, 
        paccesspkt->segment_no, paccesspkt->length, 
        paccesspkt->payload_technology, paccesspkt->payload_ss_no,
        paccesspkt->eop_id);

} /* common_dump_access_pkt */
/**=========================================================================
 *        ASSIGNMENT 2  COMMON FUNCTIONS  - END
 **=========================================================================*/

/** --------------------- function definitions - end   ---------------------- */
