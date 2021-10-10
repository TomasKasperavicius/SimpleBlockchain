#include "Header.hpp"


string hashFunction(string text)
{
    unsigned long long h = PrimeNumberSeed;
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
        h = (h * PrimeNumber1) ^ (text[i] * PrimeNumber2);
        ss << std::hex << h % PrimeNumber3;
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
MerkleTree::MerkleTree(const vector<shared_ptr<Transaction>> transactions)
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
Transaction::Transaction(RSA::PublicKey SenderAddress, RSA::PublicKey ReceiverAddress, double amount, double Balance, string signature)
{
    this->SenderAddress = SenderAddress;
    this->ReceiverAddress = ReceiverAddress;
    this->verifier = (SenderAddress);;
    this->amount = amount;
    this->signature = signature;
    string senderPubKey = to_string(this->SenderAddress.GetModulus().ConvertToLong())+to_string(this->SenderAddress.GetPublicExponent().ConvertToLong());
    string receiverPubKey = to_string(this->ReceiverAddress.GetModulus().ConvertToLong())+to_string(this->ReceiverAddress.GetPublicExponent().ConvertToLong());
    this->transactionID = hashFunction(senderPubKey+receiverPubKey+std::to_string(this->amount));
    this->BalanceError = Balance - amount < 0 ? true : false;
}
string Transaction::getTransactionHash(){
    string senderPubKey = to_string(this->SenderAddress.GetModulus().ConvertToLong())+to_string(this->SenderAddress.GetPublicExponent().ConvertToLong());
    string receiverPubKey = to_string(this->ReceiverAddress.GetModulus().ConvertToLong())+to_string(this->ReceiverAddress.GetPublicExponent().ConvertToLong());
    this->transactionID = hashFunction(senderPubKey+receiverPubKey+std::to_string(this->amount));
    return this->transactionID;
}
bool Transaction::verifyTransaction()
{
    string recovered;
    try
    {
        StringSource ss2(this->getTransactionHash() + this->signature, true,new SignatureVerificationFilter(this->verifier,new StringSink(recovered),
                             SignatureVerificationFilter::THROW_EXCEPTION |
                                 SignatureVerificationFilter::PUT_MESSAGE) 
        );                                                                 
    }
    catch(const std::exception& e)
    {
    std::cerr << e.what() << '\n';
    return false;
    }
    return true;
}
void Transaction::addSignature(string signature)
{
    this->signature = signature;
}
void Transaction::setAmount(double amount)
{
    this->amount=amount;
}
string Block::CalculateHash()
{
    return hashFunction(timestamp+version+std::to_string(difficultyTarget)+previous_hash+std::to_string(nonce)+this->getMerkleRootHash());
}
string Block::getMerkleRootHash()
{
    MerkleTree* merkleTree = new MerkleTree(this->getTransactions());
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
Block::Block(const vector<shared_ptr<Transaction>>& transactions ,int difficultyTarget, string version,string previous_hash)
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
bool Block::allTransactionsValid()
{
    for (auto&i:this->getTransactions())
    {
        if (!i->verifyTransaction())
        {
            return false;
        }
    }
    return true;
}
Block::~Block()
{

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
    vector<shared_ptr<Transaction>> t;
    User user1;
    User user2("Creator");
    shared_ptr<Transaction> pointer = make_shared<Transaction>(Transaction(user1.getPublicKey(),user2.getPublicKey(),200.00,user1.getBalance()));
    pointer->addSignature(pointer->getTransactionHash());
    t.push_back(pointer);
    Block* genesis_block = new Block (t);
    genesis_block->mineBlock();
    this->validateTransactions();//even though genesis block doesn't have transactions
    this->getpendingTransactions().clear();
    return genesis_block;
}
void Blockchain::addBlock(const vector<User*>& users,RSA::PublicKey MinerPublicKey, string MinerName)
{
    User user;
    shared_ptr<Transaction> t = make_shared<Transaction>(Transaction(user.getPublicKey(),MinerPublicKey,this->miningReward,user.getBalance()+this->miningReward));
    t->addSignature(t->getTransactionHash());
    this->getpendingTransactions().push_back(t);
    this->validateTransactions();
    Block* newBlock = new Block(this->pendingTransactions,2,"Version 1.0",this->chain[this->chain.size()-1]->CalculateHash());
    newBlock->mineBlock();
    for (auto&i:newBlock->getTransactions())
    {
        for (auto &j:users)
        {
            if (j->getPublicKey().GetModulus()==i->getSenderAddress().GetModulus())
            {
                j->setBalance(j->getBalance()-i->getAmount());
            }
            if (j->getPublicKey().GetModulus()==i->getReceiverAddress().GetModulus())
            {
                j->setBalance(j->getBalance()+i->getAmount());
            }
        }
    }
    this->getpendingTransactions().clear();
    this->chain.push_back(newBlock);
}
void Blockchain::setMiningReward(){
    this->miningReward != 0.0 ? this->miningReward/=2.0 : this->miningReward = 0;
}
void Blockchain::validateTransactions(){
    int index = 0;
    for (auto& i :this->pendingTransactions)
    {
        if(!i->verifyTransaction())
        {
            this->pendingTransactions.erase(this->pendingTransactions.begin()+index);
        }
        if (i->getBalanceError())
        {
            this->pendingTransactions.erase(this->pendingTransactions.begin()+index);
        }
        index++;
    }
}
bool Blockchain::isBlockChainValid()
{
    string previousHash; 
    bool firstBlock = true;
    for (auto &i:this->chain)
    {
        if (firstBlock)
        {
            previousHash=this->chain[0]->CalculateHash();
            firstBlock = false;
            continue;
        }
        if (!i->allTransactionsValid())
        {
            return false;
        }
        
        if (previousHash != i->getPreviousBlockHash())
        {
             return false;
        }
        if (i->getBlockHash() != i->CalculateHash())
        {
            return false;
        }
        previousHash = i->CalculateHash();
    }
    return true;   
}
