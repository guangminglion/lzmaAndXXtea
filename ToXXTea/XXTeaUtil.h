//
//  XXTeaUtil.h
//  ModelStudio
//
//  Created by howe on 15/7/28.
//
//

#ifndef __ModelStudio__XXTeaUtil__
#define __ModelStudio__XXTeaUtil__

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include "Platform.h"
#define XXTEA_KEY       "DragonAndHunters"

#define CC_BREAK_IF(cond)           if(cond) break

struct XXTEAHeader
{
    unsigned char   sig[5];             /** Signature. Should be 'xxtea' 5 bytes. */
};

struct LZMAHeader
{
    unsigned char   sig[4];             /** Signature. Should be 'lzma' 5 bytes. */
};

static int XXTEAHeader_LEN =sizeof(XXTEAHeader);

static int LZMAHeader_LEN =sizeof(LZMAHeader);

class XXTeaUtil
{
public:
    
    static void saveFile (const std::string &filepath,char* newBuffer,size_t size)
    {
        if (newBuffer)
        {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
            FILE *fp;
            fopen_s(&fp,filepath.c_str(), "wb");
#else
            FILE *fp = fopen(filepath.c_str(), "wb");
#endif
            if (fp)
            {
                fwrite(newBuffer, 1, size, fp);
                fclose(fp);
                printf("The file %s  has saved!\n",filepath.c_str());
            }
        }
    }
    
    static unsigned char* getFileData(const std::string& fullPath, const char* mode, size_t *size)
    {
        unsigned char * buffer = nullptr;
        *size = 0;
        do
        {
            // read the file from hardware
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
            FILE *fp;
            fopen_s(&fp,fullPath.c_str(), mode);
#else
            FILE *fp = fopen(fullPath.c_str(), mode);
#endif
            CC_BREAK_IF(!fp);
            
            fseek(fp,0,SEEK_END);
            *size = ftell(fp);
            fseek(fp,0,SEEK_SET);
            buffer =new unsigned char[*size];
            *size = fread(buffer,sizeof(unsigned char), *size,fp);
            fclose(fp);
        } while (0);
        return buffer;
    }
    
    static bool isFileExist(const std::string& fullPath)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        FILE *fp;
        fopen_s(&fp,fullPath.c_str(), "rb");
#else
        FILE *fp = fopen(fullPath.c_str(), "rb");
#endif
        if (fp)
        {
            fclose(fp);
            return true;
        }
        return false;
    }
    
    static char* encryptBuffer( char* pbuffer, const size_t size,const std::string &key,  size_t *outsize);
    
    static char* decryptBuffer( char* pbuffer, const size_t size,const std::string &key,  size_t *outsize);
    
    static char* compressBuffer( char* pbuffer, const size_t size, size_t *outsize);
    
    static char* unCompressBuffer( char* pbuffer, const size_t size, size_t *outsize);
    
    static bool isLzmaBuffer(const char *buffer, size_t len)
    {
        if (static_cast<size_t>(len) < sizeof(struct LZMAHeader))
        {
            return false;
        }
        struct LZMAHeader *header = (struct LZMAHeader*) buffer;
        return header->sig[0] == 'l' && header->sig[1] == 'z' && header->sig[2] == 'm' && (header->sig[3] == 'a');
    }
    
    static bool isXXTeaBuffer(const char *buffer, size_t len)
    {
        if (static_cast<size_t>(len) < sizeof(struct XXTEAHeader))
        {
            return false;
        }
        struct XXTEAHeader *header = (struct XXTEAHeader*) buffer;
        return header->sig[0] == 'x' && header->sig[1] == 'x' && header->sig[2] == 't' && (header->sig[3] == 'e' && header->sig[4] == 'a');
    }
};



#endif /* defined(__ModelStudio__XXTeaUtil__) */
