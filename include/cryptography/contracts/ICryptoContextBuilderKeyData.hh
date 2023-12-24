#ifndef I_CRYPTO_CONTEXT_BUILDER_KEY_DATA
#define I_CRYPTO_CONTEXT_BUILDER_KEY_DATA

#include "ICryptoContextBuilder.hh"
#include "../Types.hh"

class ICryptoContextBuilderKeyData
{
public:
    virtual ICryptoContextBuilder *setKey256(const unsigned char *key) = 0;
    virtual ICryptoContextBuilder *setKey(const char *key) = 0;
    virtual ICryptoContextBuilder *setKey(const char *Key, char *passphrase) = 0;
    virtual ICryptoContextBuilder *readKeyData(const char *path, char *passphrase) = 0;
    virtual ICryptoContextBuilder *readKeyData(const char *path) = 0;
};

#endif