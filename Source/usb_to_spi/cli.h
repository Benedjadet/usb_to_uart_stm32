#ifndef CLI_H
#define CLI_H



#ifdef __cplusplus
extern "C" {
#endif


#define CLI_MAX_LINE     64
#define CLI_HISTORY_SIZE 5



void CLI_Input(uint8_t ch);


#ifdef __cplusplus
}
#endif




#endif /* CLI_H */