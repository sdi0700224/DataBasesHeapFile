#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "hp_file.h"
#include "record.h"

#define CALL_BF(call)       \
{                           \
  BF_ErrorCode code = call; \
  if (code != BF_OK) {      \
    BF_PrintError(code);    \
    return HP_ERROR;        \
  }                         \
}

#define CALL_BF_N(call)     \
{                           \
  BF_ErrorCode code = call; \
  if (code != BF_OK) {      \
    BF_PrintError(code);    \
    return NULL;            \
  }                         \
}

int HP_CreateFile(char *fileName)
{
  int fd1;
  BF_Block *block;
  BF_Block_Init(&block);

  CALL_BF(BF_CreateFile(fileName));
  CALL_BF(BF_OpenFile(fileName, &fd1));

  void* data;
  CALL_BF(BF_AllocateBlock(fd1, block));
  data = BF_Block_GetData(block);

  HP_info* info = data;
  info->fileType = HEAP;
  info->num_of_blocks = 1;
  info->entries_per_block = ((BF_BLOCK_SIZE - sizeof(HP_info)) / sizeof(Record));

  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));

  BF_Block_Destroy(&block);
  CALL_BF(BF_CloseFile(fd1));

  printf("HP_CreateFile was successful\n");
  return HP_SUCCESS;
}

HP_info* HP_OpenFile(char *fileName, int *file_desc)
{
  BF_Block *block;
  BF_Block_Init(&block);

  CALL_BF_N(BF_OpenFile(fileName, file_desc));
  CALL_BF_N(BF_GetBlock(*file_desc, 0, block));
  void* data = BF_Block_GetData(block);

  if (((HP_info*)data)->fileType != HEAP)
  {
    printf("Error opening file, wrong file type.\n");
    return NULL;
  }
  
  BF_Block_Destroy(&block);
  return (HP_info*)data;
}


int HP_CloseFile(int file_desc, HP_info* hp_info)
{
  if (hp_info->fileType != HEAP)
  {
    printf("Error closing file, wrong file type.\n");
    return HP_ERROR;
  }

  BF_Block *block;
  BF_Block_Init(&block);
  
  CALL_BF(BF_GetBlock(file_desc, 0, block));
  CALL_BF(BF_UnpinBlock(block));

  BF_Block_Destroy(&block);
  CALL_BF(BF_CloseFile(file_desc));

  return HP_SUCCESS;
}

int HP_InsertEntry(int file_desc, HP_info* hp_info, Record record)
{
  if (hp_info->fileType != HEAP)
  {
    printf("Error inserting record to file, wrong file type.\n");
    return HP_ERROR;
  }

  BF_Block *block;
  BF_Block_Init(&block);

  if (hp_info->num_of_blocks <= 1)
  {
    //Create new block, copy record, update HP_Block_info
    CALL_BF(BF_AllocateBlock(file_desc, block));
    void* new_block_data = BF_Block_GetData(block);
    memcpy(new_block_data, (void*)&record, sizeof(Record));

    Record* record_handler = (Record*)new_block_data;
    record_handler += hp_info->entries_per_block;
    HP_Block_info* block_Info_handler = (HP_Block_info*)record_handler;
    block_Info_handler->entries_num = 1;

    BF_Block_SetDirty(block);
    CALL_BF(BF_UnpinBlock(block));

    //Update first block HP_info
    hp_info->num_of_blocks++;

    CALL_BF(BF_GetBlock(file_desc, 0, block));
    BF_Block_SetDirty(block);
  }
  else
  {
    //Get last block with data
    CALL_BF(BF_GetBlock(file_desc, hp_info->num_of_blocks - 1, block));
    void* last_block_data = BF_Block_GetData(block);

    // Move to HP_Block_info
    Record* record_handler = (Record*)last_block_data;
    record_handler += hp_info->entries_per_block;
    HP_Block_info* block_Info_handler = (HP_Block_info*)record_handler;

    //Check if new record can fit in last block
    if (block_Info_handler->entries_num < hp_info->entries_per_block)
    {
      //Copy record
      record_handler = (Record*)last_block_data;
      record_handler += block_Info_handler->entries_num;
      memcpy((void*)record_handler, (void*)&record, sizeof(Record));

      //Update block's HP_Block_info
      block_Info_handler->entries_num++;

      BF_Block_SetDirty(block);
      CALL_BF(BF_UnpinBlock(block));
    }
    else
    {
      //Unpin previous block used to check
      CALL_BF(BF_UnpinBlock(block));

      //Allocate, copy data, update HP_Block_info
      CALL_BF(BF_AllocateBlock(file_desc, block));
      void* new_block_data = BF_Block_GetData(block);
      memcpy(new_block_data, (void*)&record, sizeof(Record));

      record_handler = (Record*)new_block_data;
      record_handler += hp_info->entries_per_block;
      HP_Block_info* new_block_Info_handler = (HP_Block_info*)record_handler;
      new_block_Info_handler->entries_num = 1;

      BF_Block_SetDirty(block);
      CALL_BF(BF_UnpinBlock(block));

      //Update first block's info
      hp_info->num_of_blocks++;

      CALL_BF(BF_GetBlock(file_desc, 0, block));
      BF_Block_SetDirty(block);
    }
  }

  BF_Block_Destroy(&block);
  return hp_info->num_of_blocks;
}

int HP_GetAllEntries(int file_desc, HP_info* hp_info, int value)
{
  if (hp_info->fileType != HEAP)
  {
    printf("Error searching file, wrong file type.\n");
    return HP_ERROR;
  }

  BF_Block *block;
  BF_Block_Init(&block);
  int counter = -1;

  for (int i = 1; i < hp_info->num_of_blocks; i++)
  {
    CALL_BF(BF_GetBlock(file_desc, i, block));
    Record* block_data = (Record*)BF_Block_GetData(block);
    for (int j = 0; j < ((HP_Block_info*)(block_data + hp_info->entries_per_block))->entries_num; j++)
    {
      if ((block_data + j)->id == value)
      {
        printRecord(*(block_data + j));
        counter = ((i - 1) * hp_info->entries_per_block) + j + 1;
      }
    }
    CALL_BF(BF_UnpinBlock(block));
  }

  BF_Block_Destroy(&block);
  return counter;
}
