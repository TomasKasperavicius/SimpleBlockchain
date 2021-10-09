#include "User.hpp"
#include <random>
#include <algorithm>
#include <cmath>

cpp_int gcd(cpp_int a, cpp_int b) {
   if (b == 0)
   {
       return a;
   }
   return gcd(b, a % b);
}
cpp_int checkPrime(cpp_int a)
{
    string s = a.str();
    unsigned long long sum = 0;
    for (int i = 0; i < s.length(); i++)
    {
        sum+=int(s[i]-'0');
    }
    char endings[4]={'7','1','3','9'};
    if (sum%3==0||s[s.length()-1]!=endings[0]||s[s.length()-1]!=endings[1]||s[s.length()-1]!=endings[2]||s[s.length()-1]!=endings[3]
    ||int(s[s.length()-1]-'0')+(int(s[s.length()-2]-'0')*10)%4==0||(int(s[s.length()-1]-'0')+int(s[s.length()-2]-'0')*10+int(s[s.length()-3]-'0')*100)%8==0)
    {
        char current = s[s.length()-1];
        s[s.length()-1]=endings[rand()%4];
        sum=sum+int(s[s.length()-1]-'0')-int(current-'0');
        unsigned long long sumt = sum;
        while (sumt%3==0||int(s[s.length()-1]-'0')+(int(s[s.length()-2]-'0')*10)%4==0||(int(s[s.length()-1]-'0')+int(s[s.length()-2]-'0')*10+int(s[s.length()-3]-'0')*100)%8==0)
        {
            sumt = 0;
            if (int(s[s.length()-2])+1==10)
            {
                s[s.length()-2] = '0';
                
            }
            else{
                s[s.length()-2]=s[s.length()-2]+char(1);
            }
            if (int(s[s.length()-3])+1==10)
            {
                s[s.length()-3] = '0';
                
            }
            else{
                s[s.length()-3]=s[s.length()-3]+char(1);
            }
            for (int i = 0; i < s.length(); i++)
            {
                sumt+=int(s[i]-'0');
            }
        }
        
    }
    else{
        return a;
    }
    cpp_int g{s};
    return g;
}
string generateKeys(){
    string keys;
    srand(time(NULL));
    std::random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<cpp_int> ui(cpp_int(1) << 128, cpp_int(1) << 512);
    cpp_int p = ui(mt);
    p = checkPrime(p);
    cpp_int q = ui(mt);
    q = checkPrime(q);
    cpp_int n = p*q;
    cpp_int phi = (p-1)*(q-1);
    cpp_int i = 2;
    cpp_int e;
    while (gcd(i,phi)!=1)
    {
        i++;
    }
    e = i;
    if (e>=phi)
    {
        string es = e.str();
        string phis = phi.str();
        size_t diff = es.length() - phis.length();
        ++diff;
        es.resize(es.size()-diff);
        cpp_int newE{es};
        cpp_int k = 2;
        cpp_int d;
        d = (1 + (k*phi))/newE;
        keys+= e.str();
        keys += d.str();
        return keys;
    }
    cpp_int k = 2;
    cpp_int d;
    d = (1 + (k*phi))/e;
    keys+= e.str();
    keys += d.str();
    return keys;
}
User::User()
{
    cout << "aaa";
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(100, 1000000);
    //cout << "aaa";
    string keys = generateKeys();
    cout << "aaa";
    int i = 0;
    string privateK, publicK;
    while (keys[i]!=' ')
    {
        publicKey += keys[i];
        i++;
    }
    i =keys.size()-1;
    while (keys[i]!=' ')
    {
        privateK += keys[i];
        i--;
    }
    this->publicKey = cpp_int{publicK};
    this->privateKey = cpp_int{privateK};  
    this->Balance = dist(mt);
}
void User::setBalance(double Balance)
{
    this->Balance = Balance;
}
User::~User()
{
}