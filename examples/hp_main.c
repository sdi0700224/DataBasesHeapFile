#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bf.h"
#include "hp_file.h"

#define RECORDS_NUM 1000 // you can change it if you want
#define FILE_NAME "data.db"

#define CALL_OR_DIE(call)     \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK) {      \
      BF_PrintError(code);    \
      exit(code);             \
    }                         \
  }

int main() 
{
  CALL_OR_DIE(BF_Init(LRU));

  if (HP_CreateFile(FILE_NAME) == HP_ERROR) { return EXIT_FAILURE; }
  int file_desc;

  HP_info* hp_info2 = HP_OpenFile(FILE_NAME, &file_desc);

  if (hp_info2 == NULL) { return EXIT_FAILURE; }
  
  Record record;
  srand(12569874);

  printf("Insert Entries\n");
  for (int id = 0; id < RECORDS_NUM; ++id) 
  {
    record = randomRecord();
    if (HP_InsertEntry(file_desc, hp_info2, record) == HP_ERROR) { return EXIT_FAILURE; }
  }

  printf("RUN PrintAllEntries\n");
  int id = rand() % RECORDS_NUM;
  
  printf("\nSearching for: %d\n", id);

  int entries = HP_GetAllEntries(file_desc, hp_info2, id);
  if (entries == HP_ERROR) { return EXIT_FAILURE; }

  printf ("Read: %d entries to print all entries with id: %d\n", entries, id);

  if (HP_CloseFile(file_desc, hp_info2) == HP_ERROR) { return EXIT_FAILURE; }

  CALL_OR_DIE(BF_Close());
}
