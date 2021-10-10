#include "Header.hpp"
#include <climits>
#include <ctime>
#include <fstream>


int main()
{
    std::ofstream rz;
    vector<User*> users;
    std::stringstream ss;
    for (int i = 0; i < 1000; i++)
    {
        string  name = "User"+to_string(i);
        users.push_back(new User(name));
    }
    for (auto &i: users)
    {
        ss << i->getName() << " " <<i->getBalance() << endl;
    }
    rz.open("StartingData.txt");
    rz << ss.rdbuf();
    ss.clear();
    vector<shared_ptr<Transaction>> transactions;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist (0, 999);
    std::uniform_real_distribution<double> dist2 (0, 100000);
    for (int i = 0; i < 10000; i++)
    {
        int sender = dist(mt);
        int receiver = dist(mt);
        shared_ptr<Transaction> pointer = make_shared<Transaction>(Transaction(users[sender]->getPublicKey(),users[receiver]->getPublicKey(),dist2(mt),users[sender]->getBalance()));
        pointer->addSignature(users[sender]->Sign(pointer->getTransactionHash()));
    }
    User miner("Miner");
    cout << miner.getName() << " "<< miner.getBalance() << endl;
    Blockchain* blockchain = new Blockchain;
    while (transactions.size()!=0)
    {
    std::shuffle(transactions.begin(),transactions.end(),mt);
    vector<shared_ptr<Transaction>> selectedTransactions;
    for (int i = 0; i < 100; i++)
    {
        selectedTransactions.push_back(transactions[i]);
    }
    blockchain->getpendingTransactions() = selectedTransactions;
    blockchain->addBlock(users,miner.getPublicKey());
    transactions.erase(transactions.begin(),transactions.begin()+99);
    selectedTransactions.clear();
    }
    cout << miner.getName() << " "<< miner.getBalance() << endl;
    for (auto &i: users)
    {
        ss << i->getName() << " " <<i->getBalance() << endl;
    }
    rz.open("ResultData.txt");
    rz << ss.rdbuf();
    ss.clear();
    
    delete blockchain;
    for (auto &i: users)
    {
        delete i;
    }
    
    // User user;
    // User userr;
    // if (userr.getPublicKey().GetModulus()==user.getPublicKey().GetModulus())
    // {
    //     cout << "users are the same"<<endl;
    // }
    // else{
    //     cout << "users are not the same"<<endl;
    // }
    
    
    // string a = ss.str();
    // for (int i = 0; i < a.length(); i++)
    // {
    //     if (a[i]<'0'||a[i]>'9')
    //     {
    //         cout << a[i]<<" " <<i<<" ERROR: input consisnt not allowed characters!";
    //     } 
    // }
    // cout << user.getPublicKey() << endl;
    // cout << user.getN() << endl;
    // cout << user.getBalance() << endl;
    //User a;
    // vector<Transaction*> transactions;
    // for (int i = 0; i < 6; i++)
    // {
    //     transactions.push_back(new Transaction("1"+i,"2"+i,50+i));
    // }
    // vector<string>  allTransactions;
    // vector<string>  temp;
    // for(auto &i:transactions)
    // {
    //     allTransactions.push_back(i->getTransactionHash());
    // }
    // int level = 0;
    // while (allTransactions.size()!=1)
    // {
    //     if (allTransactions.size()%2!=0)
    //     {
    //         allTransactions.push_back(allTransactions[allTransactions.size()-1]);
    //     }
    //     cout <<"Level: "<< level<<"\n";
    //     for (int i = 0; i < allTransactions.size(); i++)
    //     {
    //         cout <<allTransactions[i]<<"\n";
    //     }
    //     for (int i = 0; i < allTransactions.size(); i+=2)
    //     {
            
    //         temp.push_back(hashFunction(allTransactions[i]+allTransactions[i+1]));
    //     }
    //     allTransactions = temp;
    //     temp.clear();
    //     level++;
    // }
    // cout <<"\nRoot:\n";
    // cout << allTransactions[0] <<"\n\n";
    
    // MerkleTree* tree = new MerkleTree(transactions);
    // tree->TraverseMerkleTree(tree->getRoot());
    // for (auto &i:transactions)
    // {
    //     delete i;
    // }
    // delete tree;
    return 0;
}
