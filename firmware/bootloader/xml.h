#ifndef __XML_H__
#define __XML_H__

#define XML_BUF_SIZE 128

typedef enum {
  XML_STATE_CHARACTERS,
  XML_STATE_ELEMENT_NAME,
  XML_STATE_ELEMENT_OPEN,
  XML_STATE_ELEMENT_OPEN_EMPTY,
  XML_STATE_ATTRIBUTE_NAME,
  XML_STATE_ATTRIBUTE_WAIT_EQ,
  XML_STATE_ATTRIBUTE_WAIT_QUOT,
  XML_STATE_ATTRIBUTE_VALUE,
  XML_STATE_ELEMENT_CLOSE,
  XML_STATE_ELEMENT_CLOSE_NAME,
  XML_STATE_PROC_INST_WAIT_CLOSE,
  XML_STATE_ERROR
} XML_STATE;

typedef struct {
  char buf[XML_BUF_SIZE];
  int buf_pos;
  int attr_use_apos;
  int is_proc_inst;
  XML_STATE state;
} XML_CONTEXT;

typedef struct {
  void (*StartElement)(const char* name);
  void (*EndElement)(const char* name);
  void (*StartAttribute)(const char* name);
  void (*EndAttribute)();
  void (*Characters)(const char* characters, int size);
  void (*Error)();
} XML_CALLBACKS;

void XMLInit(XML_CONTEXT* state);
void XMLProcess(const char* data, int size, XML_CONTEXT* context, XML_CALLBACKS* callbacks);


#endif  // __XML_H__
