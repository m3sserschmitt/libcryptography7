#include <iostream>

#include "cryptography/Aenigma.hh"

using namespace std;

const char *publicKey = "-----BEGIN PUBLIC KEY-----\n"
                        "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAt93z0JRoIKt0f+Yoy6KB\n"
                        "c3AYlN2LiA4NH3EsVtVFdPyOboEpDIKMQwuSP9Gi/+hBHgHnO8YXU/ytBygAzE93\n"
                        "o/BzMtNNgQS+FDDiuD19+65525rI+IZL+vulhvUVsUZgHmW7r0ACB8qxmQdmotLr\n"
                        "zgyRprJo1kCRQajS5ICsjWqx/w/s39k5V8XJnIYCAIcSiG9N22Z3GY3x1ewOfU15\n"
                        "Amw3lb7s6ccOccVUgrDWMqjfaVzYebFmXhyJ99+xp2YOjiIfwL/dDIy2R7chiTSr\n"
                        "uLWhUdX9FPjSpsTCu7vOq0fKitIe9yIXkcA+WZSU4AqxH3h+9eJtlG0/yiK/thkG\n"
                        "OwIDAQAB\n"
                        "-----END PUBLIC KEY-----\n";

const char *privateKey = "-----BEGIN ENCRYPTED PRIVATE KEY-----\n"
                         "MIIFHDBOBgkqhkiG9w0BBQ0wQTApBgkqhkiG9w0BBQwwHAQIgeAn0zx2m+8CAggA\n"
                         "MAwGCCqGSIb3DQIJBQAwFAYIKoZIhvcNAwcECKD9Qn6/xcfoBIIEyF6JEPQKoF09\n"
                         "AzqhJx4ypTbtZ7jSqm20D8G2Vre0qA3Nh4Z1/raZr1XNpCt5fpO4ctBo5tGfd0AD\n"
                         "NvEpde2NQwy9Y/iZbStVcOTKiFFoCaINoIEirHLHFmyMoTkpbia/vHRYhSvM5fkJ\n"
                         "M1CU4Nr+ffxB4V1XlZ4FQ1G9KJjOdBx1wdlsRuBrvIfTnxJM8WTeQcEPSXl2OQRt\n"
                         "CJHnUKVyiYcGj23BX0avBwDb3XH8ADmc7UDwMqYnZB3q3u3qW9/dTjMCHAh/TX17\n"
                         "ttQmQjr7kak5+m2kpnilE4kUovuOearLwB2fPl/sSXuW0AISSzWaIlKczFw9k++X\n"
                         "WT7/73jGdvkPJXjZ4ZR7nEUxbTTON8cgkRFBx3Je1rzng/5Dvru+gc+Md/ZMMiGQ\n"
                         "EQv/OBdfpkrgtzEPINytdV/R7DXoatejXoz1M6sBLfjoykcuDkkfIsfcXAzhh+4u\n"
                         "cfc7zrfjmjFjMRQdxj9saEg582FQ5yu6k39GYDhGJugzxSiznBXFUJip66pD6U3S\n"
                         "B0NAyGC2BYoII+5hoGB8RuBFO/tigO6JDdGINXM3b9v4muOQcL0nV0GNPWJM4lqH\n"
                         "roXSn9zcdLdMNNYr8uOV81KXmIoPZZ5TlNAOAc+Vm4Jc5OCsu5pHlTbr6YArXvdg\n"
                         "uWR9WKwS6QOy6+IXSRh1KStw5CnzOsSrtPVTmmfF/D6roSyvvABq9ieI1AXkOg2W\n"
                         "motfNczhzgbHEdC2IsM4NV9u+4lZgoRfgwxlJDwt3QXLkmYo0L5dszx3ghHOjEHL\n"
                         "4Ts1gKpKmh3fZWhRzEM5jOapEO7pDTtmW/JBHIisgUoZRHrwstrSLsqqyj+x3VMo\n"
                         "C75GTqfziCyF1qUQE7IyfGWMRypJfcCEzNj+0Alk9E+jgw2a66DVAgOaAP73JHJ5\n"
                         "QxssdBwd8NmGy2GKz8W2gznFUHla5Rhi18GH9lOg2txrpu+OvxhWd8Syb5RdUZ6H\n"
                         "7Bu/PnFwGK9vguvl9qZ6aC58bPrnHCRyqCwZr/bVE4mDw2vvnVk+zGjZrl5RN3lm\n"
                         "xudl/SUxCvq8QuO9HjIwhBdpmGx+or1GhoyRwC51OUHtlmRbUadWTB4CpGHzab66\n"
                         "TbDzC08+4MkmvDGsBZ6ubGxHVrv5EeiKeVwydPZ5Ay6C3BQRA9/3OdelDFvBZv8x\n"
                         "sVJpUARBV9raYpFrebddydLFwpjc0BeLkmdWIz308gQBp4e3ww7Nkz5H7v/3PsdS\n"
                         "zelvdVXlXmdKiMG5V55yNu2zZWklq7+R8t+X56OEoLA5EXB/b9wtIw5t9pbymxMB\n"
                         "L2xLWezG/yX6mWfQN2oiHNIkpkTb8WgUQjlGaUCnxcaiB+BYZVSCmAzF+I+JAOqf\n"
                         "hkfd11tj+95OIn5L9SQxWIvmBwmCbl5xwuuCbN+O/TaH/FPDD1EXeGG0muRuwRNP\n"
                         "sALPDkmBGBzYv/t+PJk4R/ETpxZQc+x/JqUnpzo2HadKT+ZRzSQ+0vPaV+X7CTRz\n"
                         "9TP2bkBD96+UQYMdb2pbzq+d8kWbxAon8cq1EBfILn/2l7lMVhLTWYUp4PhlCF1F\n"
                         "8BRQIr328o5OIEaUyWrgGUw2kPfY6JvI0qRUaIxc8KIZm4pJlIXICWTmufvpgG2H\n"
                         "jyOYYdWz/iFvQPvkJvXhRQ==\n"
                         "-----END ENCRYPTED PRIVATE KEY-----\n";

int main()
{
    const char *data = "data to be encrypted";
    CryptoContext *encrctx = CreateAsymmetricEncryptionContext(publicKey);

    const EncrypterResult *encrypted = EncryptDataEx(encrctx, (unsigned char *)data, strlen(data));

    if (encrypted->isError())
    {
        cout << "Encryption error" << endl;
        return -1;
    }

    // We need to copy the output into a separate array because
    // the data will be freed when we call cleanup() method on CryptoContext object
    int encryptedDataSize = encrypted->getDataSize();
    unsigned char *encryptedDataCopy = new unsigned char[encryptedDataSize];
    memcpy(encryptedDataCopy, encrypted->getData(), encryptedDataSize);

    encrctx->cleanup();

    encrctx->setCryptoOp(Decrypt);
    encrctx->allocateMemory();
    encrctx->setKeyData(privateKey, (char *)"12345678");

    const EncrypterResult *decrypted = DecryptDataEx(encrctx, encryptedDataCopy, encryptedDataSize);

    if (decrypted->isError())
    {
        cout << "Decryption error" << endl;
        return -1;
    }

    if (!memcmp(decrypted->getData(), data, decrypted->getDataSize()))
    {
        cout << "Success!" << endl;
    }
    else
    {
        cout << "Failure!" << endl;
    }

    delete[] encryptedDataCopy;
    FreeContext(encrctx);

    return 0;
}
