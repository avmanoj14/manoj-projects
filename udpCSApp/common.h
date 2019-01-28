
#ifndef __COMMON_H_
#define __COMMON_H_

#include <sys/socket.h>
#include <netinet/in.h>

/** --------------------- common definitions - start ---------------------- */
#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#define UDP_PORT         7891
#define UDP_IPADDR       "127.0.0.1"


#define MAX_PAYLOAD_SIZE 255

#define SOP_ID         0xFFFF
#define EOP_ID         0xFFFF


/** --------------------- common definitions - end   ---------------------- */


/** ------------------------ common types - start ------------------------- */
typedef unsigned char bool;

/* used in generic functions */
typedef enum who_tt {
  UDP_CLIENT = 1,
  UDP_SERVER = 2
}who_t;

/* All error messages in the system and 
 * decode error number to message */
typedef struct udp_error_msgs_tt {
  unsigned short int code;
  char msg[45];
}udp_error_msgs_t;

/** connection handle template for client, server */
typedef struct handle_tt {
  int socket;
  struct sockaddr_in server_addr;
  socklen_t addr_size;

  /** following using only at server side */
  struct sockaddr_storage server_storage;
}handle_t;

/** -------------------- assignment1 pkt types - start --------------------- */
#define PKT_TYPE_DATA  0xFFF1
#define PKT_TYPE_ACK   0xFFF2
#define PKT_TYPE_REJ   0xFFF3

#define REJ_SUBCODE_OUT_OF_SEQUENCE  0xFFF4
#define REJ_SUBCODE_LENGTH_MISSMATCH 0xFFF5
#define REJ_SUBCODE_EOP_MISSING      0xFFF6
#define REJ_SUBCODE_DUPLICATE_PACKET 0xFFF7

/* DATA pkt type */
typedef struct __attribute__((packed, aligned(1))) data_pkt_tt {
  unsigned short int sop_id;
  unsigned char      client_id;
  unsigned short int pkt_type;
  unsigned char      segment_no;
  unsigned char      length;
           char      payload[MAX_PAYLOAD_SIZE];
  unsigned short int eop_id;
} data_pkt_t;

/* ACK pkt type */
typedef struct __attribute__((packed, aligned(1))) ack_pkt_tt {
  unsigned short int sop_id;
  unsigned char      client_id;
  unsigned short int pkt_type;
  unsigned char      rcvd_segment_no;
  unsigned short int eop_id;
} ack_pkt_t;

/* REJECT pkt type */
typedef struct __attribute__((packed, aligned(1))) reject_pkt_tt {
  unsigned short int sop_id;
  unsigned char      client_id;
  unsigned short int pkt_type;
  unsigned short int reject_sub_code;
  unsigned char      rcvd_segment_no;
  unsigned short int eop_id;
} reject_pkt_t;

void common_dump_data_pkt(void *pBuffer);

bool common_prepare_data_pkt(unsigned short int sop_id,  
                        unsigned char client_id, 
                        unsigned short int pkt_type,
                        unsigned char segment_no,
                        unsigned char length,
                                 char *payload,
                        unsigned short int eop_id,
                            data_pkt_t *out_datapkt);

void common_dump_ack_pkt(void *pBuffer);

bool common_prepare_ack_pkt(unsigned char client_id,
                       unsigned char rcvd_segment_no,
                       ack_pkt_t *out_ackpkt);

void common_dump_reject_pkt(void *pBuffer);

bool common_prepare_reject_pkt(unsigned char client_id,
                          unsigned short int reject_sub_code,
                          unsigned char rcvd_segment_no,
                          reject_pkt_t *out_rejectpkt);

/** -------------------- assignment1 pkt types - end  --------------------- */

/** -------------------- assignment2 pkt types - start --------------------- */
#define SUBSCRIBER_ACC_PER    0xFFF8
#define SUBSCRIBER_NOT_PAID   0XFFF9
#define SUBSCRIBER_NOT_EXISTS 0XFFFA
#define SUBSCRIBER_ACC_OK     0XFFFB

/* ACCESS pkt type */
typedef struct __attribute__((packed, aligned(1))) access_pkt_tt {
  unsigned short int sop_id;             /* 2 bytes */
  unsigned char      client_id;          /* 1 byte */
  unsigned short int pkt_type;           /* 2 bytes */
  unsigned char      segment_no;         /* 1 byte - max value will be 255 */
  unsigned char      length;             /* 1 byte - value is 5 always */
  unsigned char      payload_technology; /* 1 byte character */
  unsigned int       payload_ss_no;      /* 4 byte integer */
  unsigned short int eop_id;             /* 2 bytes */
} access_pkt_t;

bool common_prepare_access_pkt(unsigned short int sop_id,
                        unsigned char client_id,
                        unsigned short int pkt_type,
                        unsigned char segment_no,
                        unsigned char length,
                        unsigned char payload_technology,
                        unsigned int  payload_ss_no,
                        unsigned short int eop_id,
                            access_pkt_t *out_accesspkt);

void common_dump_access_pkt(void *pBuffer);
/** -------------------- assignment2 pkt types - end  --------------------- */

/** ------------------------ common types - end   ------------------------- */

/** ------------------------ common function prototypes - start ----------- */
void common_dump_buffer(char *buffer, unsigned int length);


unsigned int common_udp_send_pkt(who_t who, 
                                    handle_t *handle,
                                    void *pkt, 
                                    unsigned int pkt_length);

unsigned int common_udp_recv_pkt(who_t who, 
                                   handle_t *handle, 
                                   void *buffer, 
                                   unsigned int buffer_length);

/** ------------------------ common function prototypes - end   ----------- */

#endif /* __COMMON_H_ */
