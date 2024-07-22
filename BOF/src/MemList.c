#include <windows.h>
#include <stdio.h>
#include "bofdefs.h"
#include "beacon.h"
#include "Base.c"

struct FileInfo *pFileInfo = NULL;

/* This bof will list file in memory */
void go(char *args, int argc)
{
    /* if(!bofstart()) */
    /*     return; */

    /* internal_printf("[+] Start list memfiles\n"); */
    pFileInfo = BeaconGetValue(MF_FILE_INFO_KEY);
    if(!pFileInfo)
    {
        BeaconPrintf(CALLBACK_ERROR, "[X] Failed to call BeaconGetValue! Maybe no run meminit?\n");
        return;
    }

    if(pFileInfo->numFiles > 0)
    {
        int filesfetched = 0;
        for (int i = 0; i < 100; i++)
        {
            if(pFileInfo->filehandle[i] != NULL)
            {
                size_t required_size = WideCharToMultiByte(CP_UTF8, 0, 
                        pFileInfo->filename[i], -1, NULL, 0, NULL, NULL);

                char* filename = calloc(required_size + 1, sizeof(char));

				WideCharToMultiByte(CP_UTF8, 0, 
                        pFileInfo->filename[i], -1, filename, required_size, NULL, NULL);

                /* internal_printf("\n[+] File: %d\n", i); */
                /* internal_printf("[+] Name: %s\n", filename); */
                /* internal_printf("[+] Handle: %p\n", pFileInfo->filehandle[i]); */
                /* internal_printf("[+] DataLen: %d\n", pFileInfo->filedatalen[i]); */
                /* internal_printf("[+] AllocationSize: %d\n", pFileInfo->fileallocationlen[i]); */
                /* internal_printf("[+] FileClosed: "); */

				BeaconPrintf(CALLBACK_OUTPUT, "\nFile: %d\nName: %s\nHandle: %p\nDatalen: %d\nAllocationsize: %d\nFileclosed: %s\n", i, filename, pFileInfo->filehandle[i], pFileInfo->filedatalen[i], pFileInfo->fileallocationlen[i], pFileInfo->fileclosed[i] ? "TRUE" : "FALSE");
                /* if (pFileInfo->fileclosed[i]) */
                /*     internal_printf("TRUE\n"); */
                /* else */
                /*     internal_printf("FALSE\n"); */
                
                free(filename);
            }
        }

        /* internal_printf("\nNumber of files currently stored by MemFiles: %d\n" */
        /*         "Total files stored during MemFiles lifetime: %d\n", */ 
        /*         pFileInfo->numFiles, pFileInfo->totalFiles); */

    }else{
        BeaconPrintf(CALLBACK_OUTPUT, "[-] No files currently stored by MemFiles!\n");
        /* internal_printf("[-] No files currently stored by MemFiles!\n"); */
    }
   
    /* printoutput(TRUE); */
    return;

}
