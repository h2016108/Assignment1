#ifndef QUERY_IOCTL_H
#define QUERY_IOCTL_H
#include <linux/ioctl.h>
 
typedef struct
{
    int lowerrange,upperrange,random;
} query_arg_t;
 
#define QUERY_GET_VARIABLES _IOR('q', 1, query_arg_t *)
#define QUERY_CLR_VARIABLES _IO('q', 2)
#define QUERY_SET_VARIABLES _IOW('q', 3, query_arg_t *)
#define QUERY_READWRITE_VARIABLES _IOWR('q', 4, query_arg_t *)

 
#endif
