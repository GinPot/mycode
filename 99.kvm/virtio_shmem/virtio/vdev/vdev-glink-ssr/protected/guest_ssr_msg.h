
#include <sys/neutrino.h>

#define SSR_ACK_ATTACH_POINT "ssr_ack"
#define SSR_ACK_HEADER 0x0101
#define SSR_EVENT_ATTACH_POINT "ssr_event"
#define SSR_EVENT_HEADER 0x0202

typedef struct _pulse msg_header_t;

struct ssr_ack_msg {
	msg_header_t hdr;
	int data;
};
struct ssr_event_msg {
	msg_header_t hdr;
	int data;
};
