#ifndef __UDPC_H__
#define __UDPC_H__

#include "common.h"

#define UDPC_TXQ_LENGTH       5
#define UDPC_MAX_RETRY_COUNT  3

typedef enum udpc_txq_operation_tt {
  UDPC_TXQ_DUMP               = 10,
  UDPC_TXQ_ADD_PKT            = 20,
  UDPC_TXQ_UPDATE_PENDING_ACK = 30,
  UDPC_TXQ_UPDATE_RETRYCOUNT  = 40
}udpc_txq_operation_t;

typedef struct udpc_txq_tt {

 /* true - waiting for ACK, false - slot available for new pkt */
 bool is_ack_pending;     

 /* range: 0 to 255. Segment Number of the packet to recognize */
 unsigned char segment_no; 
 
 /* range: 3 to 0. 3 means retransmission not yet triggered, 
                   0 means no more transmission */
 unsigned char retry_ctr; 

 /* pkt for due to ACK or retransmission */
 char buffer[sizeof(data_pkt_t)];

 /* datapkt size in bytes */
 unsigned short int length;
}udpc_txq_t;



#endif /* __UDPC_H__ */
