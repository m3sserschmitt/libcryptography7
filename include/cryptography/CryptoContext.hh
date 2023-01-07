#ifndef CRYPTO_CONTEXT_HH
#define CRYPTO_CONTEXT_HH

#include "EncryptionMachine.hh"
#include "DecryptionMachine.hh"
#include "SymmetricKey.hh"
#include "AsymmetricKey.hh"

enum CryptoType
{
    SymmetricCryptography,
    AsymmetricCryptography
};

enum CryptoOp
{
    Encrypt,
    Decrypt,
    Sign,
    SignVerify
};

class CryptoContext
{
    CryptoType cryptoType;
    CryptoOp cryptoOp;

    Key *key;
    EvpContext *cipher;
    CryptoMachine *cryptoMachine;

    CryptoContext(const CryptoContext &);
    const CryptoContext &operator=(const CryptoContext &);

    void setCryptoType(CryptoType cryptoType) { this->cryptoType = cryptoType; }

    void setCryptoOp(CryptoOp cryptoOp) { this->cryptoOp = cryptoOp; }

    void setKey(Key *key) { this->key = key; }

    Key *getKey() { return this->key; }

    void setCryptoMachine(CryptoMachine *cryptoMachine) { this->cryptoMachine = cryptoMachine; }

    CryptoMachine *getCryptoMachine() { return this->cryptoMachine; }

    const CryptoMachine *getCryptoMachine() const { return this->cryptoMachine; }

    EvpContext *getCipher() { return this->cipher; }

    void setCipher(EvpContext *cipher) { this->cipher = cipher; }

    bool notNullCryptoMachine() const { return this->cryptoMachine != nullptr; }

    bool notNullKey() const { return this->key != nullptr; }

    bool notNullCipher() const { return this->cipher != nullptr; }

    void freeKey()
    {
        delete this->getKey();
        this->setKey(nullptr);
    }

    bool allocateKey();

    bool initKey()
    {
        this->freeKey();
        return this->allocateKey();
    }

    void freeCipher()
    {
        delete this->getCipher();
        this->setCipher(nullptr);
    }

    bool allocateCipher();

    bool initCipher()
    {
        this->freeCipher();
        return this->allocateCipher();
    }

    void freeCryptoMachine()
    {
        delete this->getCryptoMachine();
        this->setCryptoMachine(nullptr);
    }

    bool allocateCryptoMachine();

    bool initCryptoMachine()
    {
        this->freeCryptoMachine();
        return this->allocateCryptoMachine();
    }

    void init()
    {
        this->setCryptoMachine(nullptr);
        this->setKey(nullptr);
        this->setCipher(nullptr);
    }

public:
    CryptoContext(CryptoType cryptoType, CryptoOp cryptoOp)
    {
        this->init();
        this->setup(cryptoType, cryptoOp);
    }

    CryptoContext() { this->init(); }

    ~CryptoContext() { this->cleanup(); }

    bool setup(CryptoType cryptoType, CryptoOp cryptoOp)
    {
        this->setCryptoType(cryptoType);
        this->setCryptoOp(cryptoOp);

        return this->initKey() and
               this->initCipher() and
               this->initCryptoMachine();
    }

    CryptoOp getCryptoOp() const { return this->cryptoOp; }

    CryptoType getCryptoType() const { return this->cryptoType; }

    bool setKeyData(ConstBytes key, Size keylen)
    {
        return this->notNullKey() and this->getKey()->setKeyData(key, keylen);
    }

    bool readKeyData(ConstPlaintext path, Plaintext passphrase)
    {
        return this->notNullKey() and this->getKey()->readKeyFile(path, passphrase);
    }

    bool readKeyData(ConstPlaintext path) { return this->readKeyData(path, nullptr); }

    bool isSetForEncryption() const
    {
        return this->notNullCryptoMachine() and this->getCryptoOp() == Encrypt;
    }

    bool isSetForSigning() const
    {
        return this->notNullCryptoMachine() and this->getCryptoOp() == Sign;
    }

    bool setPlaintext(ConstBytes data, Size datalen)
    {
        return (this->isSetForEncryption() or this->isSetForSigning()) and this->getCryptoMachine()->setInput(data, datalen);
    }

    bool isSetForDecryption() const
    {
        return this->notNullCryptoMachine() and this->getCryptoOp() == Decrypt;
    }

    bool isSetForVerifying() const
    {
        return this->notNullCryptoMachine() and this->getCryptoOp() == SignVerify;
    }

    const EncrypterData *getPlaintext() const
    {
        return this->isSetForDecryption() or this->isSetForVerifying() ? this->getCryptoMachine()->getOutput() : nullptr;
    }

    bool setCiphertext(ConstBytes data, Size datalen)
    {
        return (this->isSetForDecryption() or this->isSetForVerifying()) and this->getCryptoMachine()->setInput(data, datalen);
    }

    const EncrypterData *getCiphertext() const
    {
        return this->isSetForEncryption() or this->isSetForSigning() ? this->getCryptoMachine()->getOutput() : nullptr;
    }

    bool run() { return this->notNullCryptoMachine() and this->getCryptoMachine()->run(); }

    void cleanup()
    {
        this->freeCryptoMachine();
        this->freeKey();
    }
};

#endif