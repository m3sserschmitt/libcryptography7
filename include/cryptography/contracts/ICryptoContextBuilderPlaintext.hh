#ifndef I_CRYPTO_CONTEXT_BUILDER_PLAINTEXT
#define I_CRYPTO_CONTEXT_BUILDER_PLAINTEXT

#include "ICryptoContextBuilderKeyData.hh"

class ICryptoContextBuilderPlaintext
{
public:
    virtual ICryptoContextBuilderKeyData *setPlaintext(ConstBytes data, Size datalen) = 0;

    virtual ICryptoContextBuilderKeyData *noPlaintext() = 0;
};

#endif