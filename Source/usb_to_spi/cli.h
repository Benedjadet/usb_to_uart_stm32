#ifndef CLI_H
#define CLI_H



#ifdef __cplusplus
extern "C" {
#endif


#define CLI_MAX_LINE     256
#define CLI_HISTORY_SIZE 16



void CLI_Input(uint8_t ch);
void CLI_Header(void);


#ifdef __cplusplus
}
#endif




#endif /* CLI_H */