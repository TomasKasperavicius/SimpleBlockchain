#include "Header.hpp"

int main()
{
    vector<Transaction*> transactions;
    for (int i = 0; i < 6; i++)
    {
        transactions.push_back(new Transaction("1"+i,"2"+i,50+i));
    }
    vector<string>  allTransactions;
    vector<string>  temp;
    for(auto &i:transactions)
    {
        allTransactions.push_back(i->getTransactionHash());
    }
    int level = 0;
    while (allTransactions.size()!=1)
    {
        if (allTransactions.size()%2!=0)
        {
            allTransactions.push_back(allTransactions[allTransactions.size()-1]);
        }
        cout <<"Level: "<< level<<"\n";
        for (int i = 0; i < allTransactions.size(); i++)
        {
            cout <<allTransactions[i]<<"\n";
        }
        for (int i = 0; i < allTransactions.size(); i+=2)
        {
            
            temp.push_back(hashFunction(allTransactions[i]+allTransactions[i+1]));
        }
        allTransactions = temp;
        temp.clear();
        level++;
    }
    cout <<"\nRoot:\n";
    cout << allTransactions[0] <<"\n\n";
    
    MerkleTree* tree = new MerkleTree(transactions);
    tree->TraverseMerkleTree(tree->getRoot());
    for (auto &i:transactions)
    {
        delete i;
    }
    delete tree;
    return 0;
}
