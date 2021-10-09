#ifndef USER_HPP
#define USER_HPP
#include "Header.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>


using namespace boost::multiprecision;
using namespace boost::random;
using boost::multiprecision::cpp_int;

cpp_int gcd(cpp_int a, cpp_int b);
cpp_int checkPrime(cpp_int a);
string generateKeys();
class User
{
private:
    cpp_int publicKey;
    cpp_int privateKey;
    double Balance;
public:
    User();
    inline cpp_int getPrivateKey()
    {
        return this->privateKey;
    }
    inline cpp_int getPublicKey()
    {
        return this->publicKey;
    }
    inline double getBalance()
    {
        return this->Balance;
    }
    void setBalance(double Balance);
    ~User();
};


#endif

