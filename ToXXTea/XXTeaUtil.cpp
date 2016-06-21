//
//  XXTeaUtil.cpp
//  ModelStudio
//
//  Created by howe on 15/7/28.
//
//

#include "XXTeaUtil.h"
#include "xxtea.h"

extern "C" {
    
#include "easylzma/simple.h"
    
};

char* XXTeaUtil::encryptBuffer( char* pbuffer, const size_t size,const std::string &key, size_t *outsize)
{
    if (isXXTeaBuffer(pbuffer, size))
    {
        return nullptr;
    }
    if (size < XXTEAHeader_LEN)
    {
        return nullptr;
    }
    static XXTEAHeader xxteaHeader;
    xxteaHeader.sig[0] ='x';
    xxteaHeader.sig[1] ='x';
    xxteaHeader.sig[2] ='t';
    xxteaHeader.sig[3] ='e';
    xxteaHeader.sig[4] ='a';
    char *header = (char*)&xxteaHeader;
    //encrypt buffer
    size_t out_len;
    auto encryBuffer = xxtea_encrypt( pbuffer , size, key.c_str(), &out_len);
    
    auto len = XXTEAHeader_LEN + out_len ;
    char *ouBuffer = new char[len];
    memset(ouBuffer, 0, len);
    strcpy(ouBuffer, header);
    
    const char *pEnbuffer = (const char *)encryBuffer;
    for (size_t i = XXTEAHeader_LEN;i < len;i++)
    {
        ouBuffer[i] = (char)( pEnbuffer[i-XXTEAHeader_LEN] );
    }
    *outsize = len;
    free(encryBuffer);
    return ouBuffer;
}

char*  XXTeaUtil::decryptBuffer( char* pbuffer, const size_t size,const std::string &key,  size_t *outsize)
{
    if (isXXTeaBuffer(pbuffer, size))
    {
        auto fileLen = size - sizeof(XXTEAHeader);
        
        char *newStr = new char[fileLen];
        
        for (size_t i = XXTEAHeader_LEN;i < size;i++)
        {
            newStr[i - XXTEAHeader_LEN] = pbuffer[i];
        }
        char* outbuffer = (char *)xxtea_decrypt(newStr, fileLen, key.c_str(), outsize);
        delete [] newStr;
        return outbuffer;
    }
    return nullptr;
}

char* XXTeaUtil::compressBuffer( char* pbuffer, const size_t size, size_t *outsize)
{
    if (isLzmaBuffer(pbuffer, size))
    {
        return nullptr;
    }
    if (size < LZMAHeader_LEN) {
        return nullptr;
    }
    static LZMAHeader lzHeader;
    lzHeader.sig[0] ='l';
    lzHeader.sig[1] ='z';
    lzHeader.sig[2] ='m';
    lzHeader.sig[3] ='a';
    
    char *header = (char*)&lzHeader;
    //encrypt buffer
    
    unsigned char * compressbuffer = nullptr;
    size_t outLen;
    int ret = simpleCompress(ELZMA_lzma, (unsigned char*)pbuffer, size, &compressbuffer, &outLen);
    if (ret == ELZMA_E_OK)
    {
        auto len = LZMAHeader_LEN + outLen;
        
        char *ouBuffer = new char[len];
        memset(ouBuffer, 0, len);
        strcpy(ouBuffer, header);
        
        for (size_t i = LZMAHeader_LEN;i < len;i++)
        {
            ouBuffer[i] = (char)( compressbuffer[i-LZMAHeader_LEN] );
        }
        *outsize = len;
        free(compressbuffer);
        return ouBuffer;
    }
    free(compressbuffer);
    return nullptr;
}

 char* XXTeaUtil::unCompressBuffer( char* pbuffer, const size_t size, size_t *outsize)
{
    if (isLzmaBuffer(pbuffer, size))
    {
        auto fileLen = size - sizeof(LZMAHeader);
        char *newStr = new char[fileLen];
        
        for (size_t i = LZMAHeader_LEN;i < size;i++)
        {
            newStr[i - LZMAHeader_LEN] = pbuffer[i];
        }
        unsigned char * uncompressbuffer = nullptr;
        int ret = simpleDecompress(ELZMA_lzma, (unsigned char*)newStr, fileLen, &uncompressbuffer, outsize);
        delete [] newStr;
        return (char*)uncompressbuffer;
    }
    return nullptr;
}



