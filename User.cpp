#include "User.hpp"

User::User(std::string Name)
{
    this->Name = Name;
    AutoSeededRandomPool rng;
    RSA::PrivateKey privateKey;
    privateKey.GenerateRandomWithKeySize(rng, 1024);
    RSA::PublicKey publicKey(privateKey);
    publicKey.Save(HexEncoder(new StringSink(this->PublicKey)).Ref());
    privateKey.Save(HexEncoder(new StringSink(this->PrivateKey)).Ref());
    this->signer = (privateKey);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist (100, 1000000);
    this->Balance = dist(mt);
}
void User::setBalance(double Balance)
{
    this->Balance = Balance;
}
std::string User::Sign(std::string transactionID)
{
    std::string signature;
    AutoSeededRandomPool rng;
    StringSource ss(transactionID, true,new SignerFilter(rng, this->signer,new HexEncoder(new StringSink(signature))));
    return signature;
}

User::~User()
{
}