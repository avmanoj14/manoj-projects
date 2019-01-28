

all: udpC_all udpS_all

udpC_all: udpC_1 udpC_2 
udpS_all: udpS_1 udpS_2 

udpC_1: libcommon.a udpc.c udpc_1.c udpc_test_data.c
		gcc -c udpc_1.c -o udpc_1.o -I.
		gcc -c udpc_test_data.c -o udpc_test_data.o -I.
		gcc -c udpc.c -o udpc.o -I. -DASSIGNMENT=1
		gcc -o udpC_1 udpc_1.o udpc_test_data.o udpc.o -L. -lcommon

udpC_2: libcommon.a udpc.c udpc_2.c udpc_test_data.c
		gcc -c udpc_2.c -o udpc_2.o -I.
		gcc -c udpc_test_data.c -o udpc_test_data.o -I.
		gcc -c udpc.c -o udpc.o -I. -DASSIGNMENT=2
		gcc -o udpC_2 udpc_2.o udpc_test_data.o udpc.o -L. -lcommon

udpS_1: libcommon.a udps.c udps_1.c 
		gcc -c udps_1.c -o udps_1.o -I.
		gcc -c udps.c -o udps.o -I.  -DASSIGNMENT=1
		gcc -o udpS_1 udps_1.o udps.o -L. -lcommon

udpS_2: libcommon.a udps.c udps_2.c 
		gcc -c udps_2.c -o udps_2.o -I.
		gcc -c udps.c -o udps.o -I. -DASSIGNMENT=2
		gcc -o udpS_2 udps_2.o udps.o -L. -lcommon

libcommon.a: common.c
		gcc -c common.c -o common.o -I.
		ar crv libcommon.a common.o

clean: 
		rm -rf udpC_1 udpC_2 udpS_1 udpS_2 common.o udpc.o udpc_1.o udpc_2.o udps.o udps_1.o udps_2.o udpc_test_data.o libcommon.a

