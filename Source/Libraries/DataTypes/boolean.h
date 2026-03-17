#ifndef _BOOLEAN_H_
#define _BOOLEAN_H_


#ifdef __cplusplus

typedef bool boolean_t;

#else

typedef unsigned short boolean_t;
#define false 0
#define true 1

#endif







#endif /* _BOOLEAN_H_ */