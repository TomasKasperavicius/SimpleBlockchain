#ifndef USER_HPP
#define USER_HPP
#include "cryptopp/files.h"
#include "cryptopp/osrng.h"
#include "cryptopp/cryptlib.h"
#include "cryptopp/rsa.h"
#include "cryptopp/filters.h"
#include "cryptopp/pssr.h"
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
    CryptoPP::RSA::PrivateKey privateKey;
    CryptoPP::RSA::PublicKey publicKey;
    CryptoPP::RSASS<CryptoPP::PSS, CryptoPP::SHA256>::Signer signer;
    double Balance;
public:
    User(std::string Name = "Null");
    inline std::string getName()
    {
        return this->Name;
    }
    inline CryptoPP::RSA::PublicKey getPublicKey()
    {
        return this->publicKey;
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

