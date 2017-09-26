#ifndef protocol_TCM2_H
#define protocol_TCM2_H


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>

#pragma pack (push,4)

struct header {
	unsigned char start_byte[4];
	unsigned int len;
	unsigned int type;
	unsigned int count;
	unsigned int send_time_low;
	unsigned int send_time_high;
};
struct TCM_Data {
    double yaw;
    double pitch;
    double roll;
    double Mx;
    double My;
    double Mz;
    bool errorMagnOutOfRange;
    bool errorInclOutOfRange;
    bool errorMagnDistortion;
    int packetsReceived;
    int packetsInvalid;
    int packetsSended;
};

struct TCM2_protocol_in {
	struct header head;
    struct TCM_Data data;
	unsigned int checksum;
};

#pragma pack (pop)

#endif
