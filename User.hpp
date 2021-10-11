#ifndef USER_HPP
#define USER_HPP
#include "cryptopp/files.h"
#include "cryptopp/osrng.h"
#include "cryptopp/cryptlib.h"
#include "cryptopp/rsa.h"
#include "cryptopp/filters.h"
#include "cryptopp/pssr.h"
#include "cryptopp/hex.h"
#include "cryptopp/rw.h"
#include <random>


using CryptoPP::FileSink;
using CryptoPP::FileSource;
using CryptoPP::RandomNumberGenerator;
using CryptoPP::AutoSeededRandomPool;
using CryptoPP::Exception;
using CryptoPP::StringSink;
using CryptoPP::StreamTransformation;
using CryptoPP::StreamTransformationFilter;
using namespace CryptoPP;

class User
{
private:
    std::string Name;
    std::string PrivateKey;
    std::string PublicKey;
    RSASS<PSS, SHA256>::Signer signer;
    double Balance;
public:
    User(std::string Name = "Null");
    inline std::string getName()
    {
        return this->Name;
    }
    inline std::string getPublicKey()
    {
        return this->PublicKey;
    }
    inline double getBalance()
    {
        return this->Balance;
    }
    void setBalance(double Balance);
    std::string Sign(std::string transactionID);
    ~User();
};

#endif

