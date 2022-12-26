#ifndef CIPHER_CONTEXT_HH
#define CIPHER_CONTEXT_HH

#include "Constants.hh"
#include "EncrypterData.hh"
#include "EncrypterResult.hh"
#include "RandomDataGenerator.hh"

#include <openssl/evp.h>

class CipherContext
{
    EVP_PKEY *pkey;
    ConstBytes keyData;

    EVP_CIPHER_CTX *cipherContext;

    Bytes encryptedKey;
    int encryptedKeyLength;

    Bytes iv;

    Bytes outBuffer;
    int outBufferSize;

    Bytes tag;

    void init(EVP_PKEY *pkey, Bytes keyData)
    {
        this->setPkey(pkey);
        this->setKeyData(keyData);
        this->setCipherContext(nullptr);
        this->setEncryptedKey(nullptr);
        this->setEncryptedKeyLength(0);
        this->setIV(nullptr);
        this->setOutBuffer(nullptr);
        this->setOutBufferSize(0);
        this->setTag(nullptr);
    }

    void setKeyData(Bytes keyData) { this->keyData = keyData; }

    ConstBytes getKeyData() const { return this->keyData; }

    Bytes getTag() { return this->tag; }

    void setTag(Bytes tag) { this->tag = tag; }

    bool writeTag(ConstBytes tag)
    {
        Bytes localTag = this->getTag();
        if (tag and localTag)
        {
            memcpy(localTag, tag, TAG_SIZE);
            return true;
        }

        return false;
    }

    Bytes getOutBuffer() { return this->outBuffer; }

    void setOutBuffer(Bytes outBuffer) { this->outBuffer = outBuffer; }

    void setOutBufferSize(Size outBufferSize) { this->outBufferSize = outBufferSize; }

    Size getOutBufferSize() const { return this->outBufferSize; }

    int *getOutBufferSizePtr() { return &this->outBufferSize; }

    EVP_PKEY *getPkey() { return this->pkey; }

    void setPkey(EVP_PKEY *pkey) { this->pkey = pkey; }

    EVP_CIPHER_CTX *getCipherContext() { return this->cipherContext; }

    void setCipherContext(EVP_CIPHER_CTX *cipherContext) { this->cipherContext = cipherContext; }

    void setEncryptedKey(Bytes encryptedKey) { this->encryptedKey = encryptedKey; }

    bool writeEncryptedKey(ConstBytes encryptedKey)
    {
        Bytes localEncryptedKey = this->getEncryptedKey();

        if (encryptedKey and localEncryptedKey)
        {
            Size encryptedKeySize = this->getPkeySize();
            memcpy(localEncryptedKey, encryptedKey, encryptedKeySize);
            this->setEncryptedKeyLength(encryptedKeySize);
            return true;
        }

        return false;
    }

    void setEncryptedKeyLength(Size len) { this->encryptedKeyLength = len; }

    Size getEncryptedKeyLength() const { return this->encryptedKeyLength; }

    Bytes getEncryptedKey() { return this->encryptedKey; }

    int *getEncryptedKeyLengthPtr() { return &this->encryptedKeyLength; }

    unsigned char **getEncryptedKeyPtr() { return &this->encryptedKey; }

    EVP_PKEY **getPkeyPtr() { return &this->pkey; }

    void setIV(Bytes iv) { this->iv = iv; }

    Bytes getIV() { return this->iv; }

    bool writeIV(ConstBytes iv)
    {
        Bytes localIV = this->getIV();

        if (localIV and iv)
        {
            memcpy(localIV, iv, IV_SIZE);
            return true;
        }

        return false;
    }

    int getPkeySize()
    {
        EVP_PKEY *pkey = this->getPkey();

        if (pkey)
        {
            return EVP_PKEY_size(pkey);
        }

        return -1;
    }

    void freeIV()
    {
        Bytes iv = this->getIV();

        if (iv)
        {
            memset(iv, 0, IV_SIZE);
            delete[] iv;
            this->setIV(nullptr);
        }
    }

    bool allocateIV()
    {
        if (not this->getIV())
        {
            this->setIV(new Byte[IV_SIZE + 1]);
            return this->getIV() != nullptr;
        }

        return true;
    }

    bool generateIV()
    {
        ConstBytes randomData = RandomDataGenerator::generate(IV_SIZE)->getData();

        return this->writeIV(randomData);
    }

    void freeEncryptedKey()
    {
        Bytes encryptedKey = this->getEncryptedKey();

        if (encryptedKey)
        {
            memset(encryptedKey, 0, SYMMETRIC_KEY_SIZE);
            delete[] encryptedKey;
            this->setEncryptedKey(nullptr);
            this->setEncryptedKeyLength(0);
        }
    }

    bool allocateEncryptedKey()
    {
        int pkeySize = this->getPkeySize();

        if (pkeySize < 0)
        {
            return false;
        }

        this->freeEncryptedKey();
        this->setEncryptedKey(new Byte[pkeySize + 1]);
        this->setEncryptedKeyLength(0);

        return this->getEncryptedKey() != nullptr;
    }

    void freeCipherContext()
    {
        EVP_CIPHER_CTX_free(this->getCipherContext());
        this->setCipherContext(nullptr);
    }

    bool allocateCipherContext()
    {
        this->freeCipherContext();
        this->setCipherContext(EVP_CIPHER_CTX_new());

        return this->getCipherContext() != nullptr;
    }

    void freeOutBuffer()
    {
        Bytes outBuffer = this->getOutBuffer();

        if (outBuffer)
        {
            memset(outBuffer, 0, this->getOutBufferSize());
            delete[] outBuffer;
            this->setOutBuffer(nullptr);
            this->setOutBufferSize(0);
        }
    }

    bool allocateOutBuffer(Size len)
    {
        if (not this->getOutBuffer())
        {
            this->setOutBuffer(new Byte[len + 1]);
            this->setOutBufferSize(0);

            return this->getOutBuffer() != nullptr;
        }

        return true;
    }

    void freeTag()
    {
        Bytes tag = this->getTag();

        if (tag)
        {
            memset(tag, 0, TAG_SIZE);
            delete[] tag;
            this->setTag(nullptr);
        }
    }

    bool allocateTag()
    {
        if (not this->getTag())
        {
            this->setTag(new Byte[TAG_SIZE + 1]);

            return this->getTag() != nullptr;
        }

        return true;
    }

    bool sealEnvelopeAllocateMemory(const EncrypterData *in)
    {
        return this->allocateCipherContext() and this->allocateEncryptedKey() and this->allocateIV() and this->allocateOutBuffer(in->getDataSize()) and this->allocateTag();
    }

    bool openEnvelopeAllocateMemory(const EncrypterData *in)
    {
        Size outBufferSize = in->getDataSize() - this->getPkeySize() - IV_SIZE - TAG_SIZE;
        return this->allocateCipherContext() and this->allocateEncryptedKey() and this->allocateIV() and this->allocateOutBuffer(outBufferSize) and this->allocateTag();
    }

    bool encryptionAllocateMemory(const EncrypterData *in)
    {
        return this->allocateCipherContext() and this->allocateIV() and this->allocateOutBuffer(in->getDataSize()) and this->allocateTag();
    }

    bool decryptionAllocateMemory(const EncrypterData *in)
    {
        Size outBufferSize = in->getDataSize() - IV_SIZE - TAG_SIZE;
        return this->allocateCipherContext() and this->allocateIV() and this->allocateOutBuffer(outBufferSize) and this->allocateTag();
    }

    EncrypterResult *abort()
    {
        this->reset();
        return new EncrypterResult(false);
    }

    Size calculateEnvelopeSize()
    {
        Size encryptedKeySize = this->getEncryptedKeyLength();
        Size outBufferSize = this->getOutBufferSize();

        return encryptedKeySize + IV_SIZE + outBufferSize + TAG_SIZE;
    }

    /**
     * @brief Create a Envelope;
     *
     * Envelope structure:
     * N = size of public key (e.g. 2048 bits key length => N = 256 bytes);
     * P = size of plaintext;
     *
     * Encrypted Key: bytes 0..N-1;
     * Initialization Vector: bytes N..N+11 (AES GCM default IV length of 12 bytes);
     * Encrypted buffer: bytes N+12..N+P+11;
     * Tag: bytes N+P+11..N+P+26 (AES GCM tag size of 16 bytes);
     * @return EncrypterResult* Structure containing envelope data and size;
     */
    EncrypterResult *createEnvelope()
    {
        Size envelopeSize = this->calculateEnvelopeSize();
        Bytes envelope = new Byte[envelopeSize + 1];

        Size N = this->getEncryptedKeyLength();
        Size P = this->getOutBufferSize();

        memcpy(envelope, this->getEncryptedKey(), N);
        memcpy(envelope + N, this->getIV(), IV_SIZE);
        memcpy(envelope + N + IV_SIZE, this->getOutBuffer(), P);
        memcpy(envelope + N + IV_SIZE + P, this->getTag(), TAG_SIZE);

        EncrypterResult *result = new EncrypterResult(envelope, envelopeSize);

        memset(envelope, 0, envelopeSize);
        delete[] envelope;

        return result;
    }

    ConstBytes readEnvelope(const EncrypterData *in, Size &cipherlen)
    {
        cipherlen = 0;

        if(not in or not in->getData())
        {
            return nullptr;
        }

        Size N = this->getPkeySize();
        Size envelopeSize = in->getDataSize();
        ConstBytes envelope = in->getData();
        
        if (not this->writeEncryptedKey(envelope) or not this->writeIV(envelope + N) or not this->writeTag(envelope + envelopeSize - TAG_SIZE))
        {
            return nullptr;
        }

        cipherlen = envelopeSize - N - IV_SIZE - TAG_SIZE;
        return envelope + N + IV_SIZE;
    }

    EncrypterResult *createEncryptedData()
    {
        Size bufferSize = this->getOutBufferSize();
        Size finalDataSize = bufferSize + IV_SIZE + TAG_SIZE;

        Bytes finalData = new Byte[finalDataSize + 1];

        memcpy(finalData, this->getIV(), IV_SIZE);
        memcpy(finalData + IV_SIZE, this->getOutBuffer(), bufferSize);
        memcpy(finalData + IV_SIZE + bufferSize, this->getTag(), TAG_SIZE);

        EncrypterResult *result = new EncrypterResult(finalData, finalDataSize);

        memset(finalData, 0, finalDataSize);
        delete[] finalData;

        return result;
    }

    ConstBytes readEncryptedData(const EncrypterData *in, Size &cipherlen)
    {
        cipherlen = 0;

        if(not in or not in->getData())
        {
            return nullptr;
        }

        Size dataSize = in->getDataSize();
        ConstBytes data = in->getData();

        if(not this->writeIV(data) or not this->writeTag(data + dataSize - TAG_SIZE))
        {
            return nullptr;
        }

        cipherlen = dataSize - IV_SIZE - TAG_SIZE;
        return data + IV_SIZE;
    }

public:
    CipherContext(EVP_PKEY *pkey) { this->init(pkey, nullptr); }

    CipherContext(Bytes keyData) { this->init(nullptr, keyData); }

    EncrypterResult *sealEnvelope(const EncrypterData *in)
    {
        if (not in or not in->getData())
        {
            return this->abort();
        }

        this->reset();

        if (not this->sealEnvelopeAllocateMemory(in))
        {
            return this->abort();
        }

        if (EVP_SealInit(this->getCipherContext(),
                         EVP_aes_256_gcm(),
                         this->getEncryptedKeyPtr(),
                         this->getEncryptedKeyLengthPtr(),
                         this->getIV(),
                         this->getPkeyPtr(), 1) != 1)
        {
            return this->abort();
        }

        int len;
        int len2;

        if (EVP_SealUpdate(this->getCipherContext(), this->getOutBuffer(), &len, in->getData(), in->getDataSize()) != 1)
        {
            return this->abort();
        }

        if (EVP_SealFinal(this->getCipherContext(), this->getOutBuffer() + len, &len2) != 1)
        {
            return this->abort();
        }

        if (EVP_CIPHER_CTX_ctrl(this->getCipherContext(), EVP_CTRL_GCM_GET_TAG, TAG_SIZE, this->getTag()) != 1)
        {
            return this->abort();
        }

        this->setOutBufferSize(len + len2);

        EncrypterResult *result = this->createEnvelope();

        this->reset();

        return result;
    }

    EncrypterResult *openEnvelope(const EncrypterData *in)
    {
        if (not in or not in->getData())
        {
            return this->abort();
        }

        this->reset();

        if (not this->openEnvelopeAllocateMemory(in))
        {
            return this->abort();
        }

        Size cipherlen;
        ConstBytes ciphertext = this->readEnvelope(in, cipherlen);

        if (not ciphertext or not cipherlen)
        {
            return this->abort();
        }

        if (EVP_OpenInit(this->getCipherContext(),
                         EVP_aes_256_gcm(),
                         this->getEncryptedKey(),
                         this->getEncryptedKeyLength(),
                         this->getIV(),
                         this->getPkey()) != 1)
        {
            return this->abort();
        }

        int len;

        if (EVP_OpenUpdate(this->getCipherContext(),
                           this->getOutBuffer(),
                           &len, ciphertext, cipherlen) != 1)
        {
            return this->abort();
        }

        if (EVP_CIPHER_CTX_ctrl(this->getCipherContext(), EVP_CTRL_GCM_SET_TAG, TAG_SIZE, this->getTag()) != 1)
        {
            return this->abort();
        }

        int len2;

        if (EVP_OpenFinal(this->getCipherContext(), this->getOutBuffer() + len, &len2) != 1)
        {
            return this->abort();
        }

        this->setOutBufferSize(len + len2);

        EncrypterResult *result = new EncrypterResult(this->getOutBuffer(), this->getOutBufferSize());

        this->reset();

        return result;
    }

    EncrypterResult *encrypt(const EncrypterData *in)
    {
        if (not in or not in->getData())
        {
            return this->abort();
        }

        this->reset();

        if (not this->encryptionAllocateMemory(in) or not this->generateIV())
        {
            return this->abort();
        }

        if (EVP_EncryptInit_ex(this->getCipherContext(), EVP_aes_256_gcm(), NULL, this->getKeyData(), this->getIV()) != 1)
        {
            return this->abort();
        }

        int len;

        if (EVP_EncryptUpdate(this->getCipherContext(), this->getOutBuffer(), &len, in->getData(), in->getDataSize()) != 1)
        {
            return this->abort();
        }

        int len2;

        if (EVP_EncryptFinal_ex(this->getCipherContext(), this->getOutBuffer() + len, &len2) != 1)
        {
            return this->abort();
        }

        this->setOutBufferSize(len + len2);

        if (EVP_CIPHER_CTX_ctrl(this->getCipherContext(), EVP_CTRL_GCM_GET_TAG, TAG_SIZE, this->getTag()) != 1)
        {
            return this->abort();
        }

        EncrypterResult *result = this->createEncryptedData();

        this->reset();

        return result;
    }

    EncrypterResult *decrypt(const EncrypterData *in)
    {
        if (not in or not in->getData())
        {
            return this->abort();
        }

        this->reset();

        if (not this->decryptionAllocateMemory(in))
        {
            return this->abort();
        }

        Size cipherlen;
        ConstBytes ciphertext = this->readEncryptedData(in, cipherlen);

        if (EVP_DecryptInit_ex(this->getCipherContext(), EVP_aes_256_gcm(), NULL, this->getKeyData(), this->getIV()) != 1)
        {
            return this->abort();
        }

        int len;

        if (EVP_DecryptUpdate(this->getCipherContext(), this->getOutBuffer(), &len, ciphertext, cipherlen) != 1)
        {
            return this->abort();
        }

        if (EVP_CIPHER_CTX_ctrl(this->getCipherContext(), EVP_CTRL_GCM_SET_TAG, TAG_SIZE, this->getTag()) != 1)
        {
            return this->abort();
        }

        int len2;

        if (EVP_DecryptFinal_ex(cipherContext, this->getOutBuffer() + len, &len2) != 1)
        {
            return this->abort();
        }

        this->setOutBufferSize(len + len2);

        EncrypterResult *result = new EncrypterResult(this->getOutBuffer(), this->getOutBufferSize());

        this->reset();

        return result;
    }

    void reset()
    {
        this->freeCipherContext();
        this->freeEncryptedKey();
        this->freeIV();
        this->freeOutBuffer();
        this->freeTag();
    }
};

#endif