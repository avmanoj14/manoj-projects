
#include<stdio.h>

#include "common.h"

/*********************************************************************
 *  Assignment 1 Test data - START
 *********************************************************************/
data_pkt_t g_udpc_test_data[] = 
{
  /* SOP_ID, Client_ID, PKT_TYPE_DATA, segmentNo, length of payload, payload, EOP_ID */

  /** Good Case (All OK) - 1 */
  {0xFFFF, 1, 0xFFF1, 1,  1,  "1"     , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 2,  2,  "22"    , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 3,  3,  "333"   , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 4,  4,  "4444"  , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 5,  5,  "55555" , 0xFFFF},

  /** Good Case (All OK) - 2 */
  {0xFFFF, 1, 0xFFF1, 6,  6,  "666666"     , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 7,  7,  "7777777"    , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 8,  8,  "88888888"   , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 9,  9,  "999999999"  , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 10, 10, "0000000000" , 0xFFFF},

  /** Error Case - 1 i.e. Sequence number mismatch */
  {0xFFFF, 1, 0xFFF1, 11, 11, "AAAAAAAAAAA"      , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 13, 13, "CCCCCCCCCCCCC"    , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 14, 14, "DDDDDDDDDDDDDD"   , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 15, 15, "EEEEEEEEEEEEEEE"  , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 16, 16, "FFFFFFFFFFFFFFFF" , 0xFFFF},

  /** Error Case - 2 i.e. pkt length and payload mismatch */
  {0xFFFF, 1, 0xFFF1, 12, 11, "GGGGGGGGGGG"      , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 13,  0, "HHHHHHHHHHHHH"    , 0xFFFF},  /* sending only 0 instead of 13 */
  {0xFFFF, 1, 0xFFF1, 14, 14, "IIIIIIIIIIIIII"   , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 15, 15, "JJJJJJJJJJJJJJJ"  , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 16, 16, "KKKKKKKKKKKKKKKK" , 0xFFFF},

  /** Error Case - 3 i.e. pkt without EOP_ID */
  {0xFFFF, 1, 0xFFF1, 13, 17, "LLLLLLLLLLLLLLLLL"     , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 14, 18, "MMMMMMMMMMMMMMMMMM"    , 0xFFFF},  
  {0xFFFF, 1, 0xFFF1, 15, 19, "NNNNNNNNNNNNNNNNNNN"   , 0xEEEE},  /* sending 0xEEEE instead of 0xFFFF */
  {0xFFFF, 1, 0xFFF1, 16, 20, "OOOOOOOOOOOOOOOOOOOO"  , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 17, 21, "PPPPPPPPPPPPPPPPPPPPP" , 0xFFFF},
  
  /** Error Case - 4 i.e. pkt with duplicate segment number */
  {0xFFFF, 1, 0xFFF1, 15, 22, "QQQQQQQQQQQQQQQQQQQQQQ"     , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 16, 23, "RRRRRRRRRRRRRRRRRRRRRRR"    , 0xFFFF},  
  {0xFFFF, 1, 0xFFF1, 17, 24, "SSSSSSSSSSSSSSSSSSSSSSSS"   , 0xFFFF},  
  {0xFFFF, 1, 0xFFF1, 18, 25, "TTTTTTTTTTTTTTTTTTTTTTTTT"  , 0xFFFF},  
  {0xFFFF, 1, 0xFFF1, 17, 26, "UUUUUUUUUUUUUUUUUUUUUUUUUU" , 0xFFFF},  /* segment number is same as previous */
  
  /** Good case - (All OK) once again finally */
  {0xFFFF, 1, 0xFFF1, 19, 2, "VV"     , 0xFFFF},
  {0xFFFF, 1, 0xFFF1, 20, 3, "WWW"    , 0xFFFF},  
  {0xFFFF, 1, 0xFFF1, 21, 4, "XXXX"   , 0xFFFF},  
  {0xFFFF, 1, 0xFFF1, 22, 5, "YYYYY"  , 0xFFFF},  
  {0xFFFF, 1, 0xFFF1, 23, 6, "ZZZZZZ" , 0xFFFF}
  
};

const int UDPC_TEST_DATA_PKTS_COUNT =  (sizeof(g_udpc_test_data)/sizeof(g_udpc_test_data[0]));
/*********************************************************************
 *  Assignment 1 Test data - END
 *********************************************************************/

/*********************************************************************
 *  Assignment 2 Test data - START
 *********************************************************************/
access_pkt_t g_udpc_test_access_data[] = 
{
  /* SOP_ID(2), Client_ID(1), PKT_TYPE(2), segmentNo(1), length(1), Payload Technology(1), payload_ss_no(4), EOP_ID(2) */

  {0xFFFF,  1,  0xFFF8,  10,   5,   4,   4085546805,  0xFFFF},
  {0xFFFF,  1,  0xFFF8,  50,   5,   4,   4089999999,  0xFFFF},
  {0xFFFF,  1,  0xFFF8,  20,   5,   3,   4086668821,  0xFFFF},
  {0xFFFF,  1,  0xFFF8,  30,   5,   2,   4086808821,  0xFFFF},
  {0xFFFF,  1,  0xFFF8,  40,   5,   5,   4085546805,  0xFFFF}
};

const int UDPC_TEST_ACCESS_PKTS_COUNT =  (sizeof(g_udpc_test_access_data)/sizeof(g_udpc_test_access_data[0]));
/*********************************************************************
 *  Assignment 2 Test data - END
 *********************************************************************/



