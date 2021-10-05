#include "Header.hpp"


string hashFunction(string text)
{
    unsigned long long h = seed;
    if (text.length() == 0)
    {
        return "ac5171ee1fcf8ad4a986e4d6ab3c0e3c2eb38b79fb557376ea497bee17f8dd7e"; //random hashas
    }
    if (text.length() < 32)
    {
        unsigned long long num = 263239;
        for (int i = 0; i < text.length(); i++)
        {
            num = (num * 5) ^ (text[i]);
        }
        srand(num);
        for (int i = text.length() - 1; i < 32; i++)
        {
            text += std::to_string(rand() % num);
        }
    }
    std::stringstream ss;
    for (int i = 0; i < text.length(); i++)
    {
        h = (h * A) ^ (text[i] * B);
        ss << std::hex << h % C;
    }
    string hash = "";
    string all_chars = ss.str();
    for (int i = all_chars.length() - 1; i > all_chars.length() - 65; i--)
    {
        hash += all_chars[i];
    }
    return hash;
}
Node::Node(string TxHash)
{
    this->left=NULL;
    this->right=NULL;
    this->TxHash = TxHash;
}
MerkleTree::MerkleTree(const vector<Transaction*> transactions)
{
    vector<shared_ptr<Node>> allTransactionsNodes;
    
    for (auto &i:transactions)
    {
        shared_ptr<Node> pointer = make_shared<Node>(Node(i->getTransactionHash()));
        allTransactionsNodes.push_back(pointer);
    }
    vector<shared_ptr<Node>> temp;
    while (allTransactionsNodes.size() != 1)
    {
        
        if (allTransactionsNodes.size()%2!=0)
        {
            shared_ptr<Node> pointer = make_shared<Node>(Node(allTransactionsNodes.at(allTransactionsNodes.size()-1)->getTxHash()));
            allTransactionsNodes.push_back(pointer);
        }
        for (int i = 0, n = 0; i < allTransactionsNodes.size(); i+=2, n++)
        {
            shared_ptr<Node> pointer = make_shared<Node>(Node(hashFunction(allTransactionsNodes[i]->getTxHash()+allTransactionsNodes[i+1]->getTxHash())));
            temp.push_back(pointer);
            temp[n]->setLeft(allTransactionsNodes[i]);
            temp[n]->setRight(allTransactionsNodes[i+1]);
            
        }
        allTransactionsNodes = temp;
        temp.clear();
    }
    this->root = allTransactionsNodes[0];
}
void MerkleTree::TraverseMerkleTree(shared_ptr<Node> root)
{
    if (root)
    {
        cout << root->getTxHash()<<'\n';
        TraverseMerkleTree(root->getLeft());
        TraverseMerkleTree(root->getRight());
    }
}

// Transaction constructor
Transaction::Transaction(string SenderAddress, string ReceiverAddress, double amount)
{
    this->SenderAddress = SenderAddress;
    this->ReceiverAddress = ReceiverAddress;
    this->amount = amount;
    this->transactionID = hashFunction(this->SenderAddress+this->ReceiverAddress+std::to_string(this->amount));
}
string Block::CalculateHash()
{
    return hashFunction(timestamp+version+std::to_string(difficultyTarget)+previous_hash+std::to_string(nonce)+MerkleRootHash);
}
string Block::getMerkleRootHash()
{
    MerkleTree* merkleTree = new MerkleTree(this->transactions);
    string rootHash = merkleTree->getRoot()->getTxHash();
    delete merkleTree;
    return rootHash;
}

void Block::mineBlock()
{
    string difficulty = string(this->difficultyTarget,'0');
    while (this->hash.substr(0,this->difficultyTarget)!=difficulty)
    {
        this->nonce++;
        this->hash = this->CalculateHash();
    }
    
}
// Block constructor
Block::Block(const vector<Transaction*>& transactions ,int difficultyTarget, string version,string previous_hash)
{
    std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    this->timestamp = ctime(&time);
    this->version = version;
    this->difficultyTarget = difficultyTarget;
    this->previous_hash = previous_hash;
    this->nonce = 0;
    this->transactions = transactions;
    this->MerkleRootHash = this->getMerkleRootHash();
    this->hash = CalculateHash();
}
Block::~Block()
{
    for (auto&i:this->transactions)
    {
        delete i;
    }
}
// Blockchain constructor
Blockchain::Blockchain()
{
    this->chain.push_back(this->CreateGenesisBlock());
}
Blockchain::~Blockchain()
{
    for (auto&i:this->chain)
    {
        delete i;
    }
}
Block* Blockchain::CreateGenesisBlock(){
    Transaction* transaction = new Transaction("0", "0",100);
    vector<Transaction*> t;
    t.push_back(transaction);
    Block* genesis_block = new Block (t);
    genesis_block->mineBlock();
    return genesis_block;
}
