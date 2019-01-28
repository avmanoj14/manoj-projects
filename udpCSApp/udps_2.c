
/* udp server code - assignment 1*/

/** --------------------- header files inclusion - start ---------------------- */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "common.h"
/** --------------------- header files inclusion - end ---------------------- */
typedef struct verification_db_tt {
  unsigned int ss_no;
  unsigned char technology;
  unsigned char is_paid;
} verification_db_t;

//#define VERIFICATION_DB_MAX_ENTRIES 3
//verification_db_t verification_db[VERIFICATION_DB_MAX_ENTRIES] = {0,};
#define VERIFICATION_DB "verification_db.csv"
verification_db_t *g_verification_db = NULL;
unsigned int g_verification_db_max_entries = 0;

/** --------------------- global, external variables - start ---------------- */
/** --------------------- global, external variables - end   ---------------- */

/** --------------------- function declarations - start ---------------------- */
static bool udps_send_reply_pkt(handle_t *ps_handle, 
                                 data_pkt_t *pRcvd_data_pkt, 
                                 unsigned short int server_reply_code);

static unsigned short int udps_validate_data_pkt(char *buffer, 
                                            int nBytes, 
                                            unsigned char expected_seg_no);

bool udps_assignment2_main(handle_t *ps_handle);
/** --------------------- function declarations - end   ---------------------- */


/** --------------------- function definitions - start ---------------------- */

/**
 * function to send response to client for the received access permission pkt 
 */
static bool udps_send_access_permission_response(handle_t *ps_handle, 
                                 access_pkt_t *pRcvd_access_pkt, 
                                 unsigned short int server_reply_code)
{
  access_pkt_t  out_access_pkt; 
  int nBytes = 0;
  bool status = true;

  status = common_prepare_access_pkt(
                    pRcvd_access_pkt->sop_id,
                    pRcvd_access_pkt->client_id,
                    server_reply_code,
                    pRcvd_access_pkt->segment_no,
                    pRcvd_access_pkt->length,
                    pRcvd_access_pkt->payload_technology,
                    pRcvd_access_pkt->payload_ss_no,
                    pRcvd_access_pkt->eop_id,
                    &out_access_pkt);
  if (true == status) { 
    nBytes = common_udp_send_pkt(UDP_SERVER, ps_handle, 
                                    (void*)&out_access_pkt, sizeof(access_pkt_t));
  } else {
    printf("\nServer: access pkt preparation Failed\n");
    status = false;
  }

  return (status);
} /* udps_send_access_permission_response */


/**
 * function to validate the received packet and identify error code if needed
 */
static unsigned short int udps_verify_access_permission(char *buffer, 
                                                            int nBytes) 
{
  access_pkt_t *pRcvd_access_pkt;
  unsigned int i=0;
  unsigned short int resp_code = SUBSCRIBER_NOT_EXISTS;
  bool is_found = false;

  if ( (buffer == NULL) || (nBytes == 0) ) {
    printf("\r\n server reject at 1");
    return resp_code; //SUBSCRIBER_NOT_EXISTS
  } 

  pRcvd_access_pkt = (access_pkt_t *)buffer;
  for(i=0;i<g_verification_db_max_entries;i++) {
    if ( ((g_verification_db+i)->ss_no == pRcvd_access_pkt->payload_ss_no) &&
         ((g_verification_db+i)->technology == pRcvd_access_pkt->payload_technology) ) {
       is_found = true; 
       break;
    }
  } /* for */

  if ( is_found ) {
   resp_code = (1 == (g_verification_db+i)->is_paid)? SUBSCRIBER_ACC_OK: SUBSCRIBER_NOT_PAID;
  } else {
   resp_code = SUBSCRIBER_NOT_EXISTS;
  }
  return resp_code;
} /* udps_verify_access_permission */

/**
 * uses global variable(s)
 *  g_verification_db_max_entries - Total number of records in DB
 *  g_verification_db - actual entries
 */
static bool udps_load_verification_db_to_ram()
{
 
  FILE *fp;
  long int ss_no;
  int technology;
  int is_paid;
  unsigned int i=0;

  int ch;


  fp = fopen(VERIFICATION_DB, "r+t");
  if (fp == NULL) {
    printf("\nUnable to open file: %s error: %s", VERIFICATION_DB, strerror(errno));
    return false;
  }

  while( (ch = fgetc(fp)) != EOF) {
   if (ch == '\n') { g_verification_db_max_entries++; }
  }

  printf("\nTotal number of records: %d", g_verification_db_max_entries);
  g_verification_db = (verification_db_t *)calloc(g_verification_db_max_entries, sizeof(verification_db_t));
  if (NULL == g_verification_db) {
    return false;
  }

  rewind(fp);

  i = 0;
  while(!feof(fp)) {
    fscanf(fp, "%ld, %d, %d ", &ss_no, &technology, &is_paid);
    printf("\n Read values (ssno=%ld, technology=%d, paid=%d)", ss_no, technology, is_paid);
    (g_verification_db + i)->ss_no      = ss_no;
    (g_verification_db + i)->technology = technology;
    (g_verification_db + i)->is_paid    = is_paid;
    i++;
  }

  fclose(fp);

#if 0
  g_verification_db[0].ss_no      = 4085546805; 
  g_verification_db[0].technology = 4;
  g_verification_db[0].is_paid    = 1;

  g_verification_db[1].ss_no      = 4086668821; 
  g_verification_db[1].technology = 3;
  g_verification_db[1].is_paid    = 0;

  g_verification_db[2].ss_no      = 4086808821; 
  g_verification_db[2].technology = 2;
  g_verification_db[2].is_paid    = 1;
#endif

  return true;
} /* udps_load_verification_db_to_ram */

/**
 * UDP assignment2 main functionality */
bool udps_assignment2_main(handle_t *ps_handle)
{

  int nBytes;
  char buffer[1024];
  access_pkt_t *pRcvd_access_pkt;
  unsigned short int server_reply_code = 0;
  bool is_complete = false;

  int i = 0;

  /* load verification data base to ram for access verification purpose */
  udps_load_verification_db_to_ram();
  
  for(i=0;i<g_verification_db_max_entries;i++) {
    printf("\n DUMP Verification DB:: %d= (ssno=%u, technology=%d, paid=%d)", 
                  i, 
                  (g_verification_db+i)->ss_no, 
                  (g_verification_db+i)->technology, 
                  (g_verification_db+i)->is_paid);
   }

  is_complete = false;
  while(!is_complete){
   printf("\nwaiting for packet at server ...");
    nBytes = common_udp_recv_pkt(UDP_SERVER, ps_handle, buffer,1024);
   printf("\nRcvd packet at server ...");

    /** debug purpose - start */
    pRcvd_access_pkt = (access_pkt_t*)buffer;
    printf("\nUDPS RCVD: ");
    common_dump_access_pkt(pRcvd_access_pkt);
    /** debug purpose - end */

    server_reply_code = udps_verify_access_permission(buffer, nBytes);
    nBytes = udps_send_access_permission_response(ps_handle, pRcvd_access_pkt, server_reply_code);
    //is_complete = true;
  } /* while */

  /* release memory before leaving */
  if (g_verification_db != NULL) { free(g_verification_db); }
  return true; 
} /* udps_assignment2_main */


/** --------------------- function definitions - end   ---------------------- */
