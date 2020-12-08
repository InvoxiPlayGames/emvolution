#include <bslug.h>
#include <rvl/cache.h>
#include <io/fat-sd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "dvd.h"
#include "replacements.h"

BSLUG_MODULE_GAME("????");
BSLUG_MODULE_NAME("Emvolution");
BSLUG_MODULE_VERSION("1.0");
BSLUG_MODULE_AUTHOR("InvoxiPlayGames");
BSLUG_MODULE_LICENSE("MIT");

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
const int numreplacements = ARRAY_SIZE(replacements);

bool mounted;
static uint32_t fdlookup[ARRAY_SIZE(replacements)];

static int MountSDCard() {
    printf("SD_Mount\n");
    int r = SD_Mount();
    mounted = (r == 0);
    return r;
}

static bool MyDVDOpen(const char* path, DVDFileInfo* fileInfo) {
    printf("DVDOpen %s\n", path);
    if (!mounted) MountSDCard();
    for (int i = 0; i < numreplacements; i++) {
        if (strcmp(path, replacements[i][0]) == 0 || strcmp(path, replacements[i][0]+1) == 0) {
            int realentry = i;
            printf("SDOpen %s\n", replacements[realentry][1]);
            static FILE_STRUCT fs;
            fdlookup[realentry] = SD_open(&fs, replacements[realentry][1], O_RDONLY);
            if (fdlookup[realentry] == -1) return DVDOpen(path, fileInfo); //fall back to the DVD if SD_open fails
            fileInfo->start = 0xFFFF0000 + (realentry);
            fileInfo->filesize = fs.filesize;
            return true;
        }
    }
   bool response = DVDOpen(path, fileInfo);
   return response;
}
BSLUG_REPLACE(DVDOpen, MyDVDOpen);

static int32_t MyDVDConvertPathToEntrynum(const char* path) {
    printf("DVDConvertPathToEntrynum %s\n", path);
    if (!mounted) MountSDCard();
    for (int i = 0; i < numreplacements; i++) {
        if (strcmp(path, replacements[i][0]) == 0 || strcmp(path, replacements[i][0]+1) == 0) {
            return 10000 + i;
        }
    }
    int32_t entrynum = DVDConvertPathToEntrynum(path);
    return entrynum;
}
BSLUG_REPLACE(DVDConvertPathToEntrynum, MyDVDConvertPathToEntrynum);

static bool MyDVDFastOpen(int32_t entrynum, DVDFileInfo* fileInfo) {
    printf("DVDFastOpen %i\n", entrynum);
    if (!mounted) MountSDCard();
    if (entrynum >= 10000) {
        int realentry = entrynum - 10000;
        printf("SDFastOpen %s\n", replacements[realentry][1]);
        static FILE_STRUCT fs;
        fdlookup[realentry] = SD_open(&fs, replacements[realentry][1], O_RDONLY);
        if (fdlookup[realentry] == -1) return false;//we don't have the original path/entrynum, so return false
        fileInfo->start = 0xFFFF0000 + (realentry);
        fileInfo->filesize = fs.filesize;
        return true;
    }
    bool response = DVDFastOpen(entrynum, fileInfo);
    return response;
}
BSLUG_REPLACE(DVDFastOpen, MyDVDFastOpen);

static bool MyDVDClose(DVDFileInfo* fileInfo) {
    if (fileInfo->start >= 0xFFFF0000) {
        int realentry = fileInfo->start - 0xFFFF0000;
        return (SD_close(fdlookup[realentry]) == 0);
    }
    return DVDClose(fileInfo);
}
BSLUG_REPLACE(DVDClose, MyDVDClose);

static int MyDVDReadPrio(DVDFileInfo* fileInfo, void* buf, int len, int offset, int prio) {
    if (fileInfo->start >= 0xFFFF0000) {
        int realentry = fileInfo->start - 0xFFFF0000;
        printf("SDReadPrio (%i, %i) off %i len %i... ", fdlookup[realentry], realentry, offset, len);
        SD_seek(fdlookup[realentry], offset, 0);
        int bytes = SD_read(fdlookup[realentry], buf, len);
        DCFlushRange(buf, len);
        printf("read %i bytes\n", bytes);
        return bytes;
    }
    return DVDReadPrio(fileInfo, buf, len, offset, prio);
}
BSLUG_REPLACE(DVDReadPrio, MyDVDReadPrio);