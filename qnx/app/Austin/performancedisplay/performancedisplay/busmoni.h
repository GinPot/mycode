#ifndef _BUSMONI_H_
#define _BUSMONI_H_

struct BusMoniMems {
	char name[35];
	int rdata;
	char rdatacom[4];
	int wdata;
	char wdatacom[4];
};

extern int GetBusMoni(struct BusMoniMems *busmonidata);



#endif