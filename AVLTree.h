/**
 * AVLTree.h
 */

#ifndef AVLTREE_H
#define AVLTREE_H
#include <optional>
#include <string>
#include <vector>

using namespace std;

class AVLTree {
public:
    using KeyType = std::string;
    using ValueType = size_t;

    bool insert(const KeyType& key, const ValueType&); // insert method
    bool remove(const KeyType& key); // remove method
    bool contains(const KeyType& key) const;

    optional<ValueType> get(const KeyType& key) const;
    size_t& operator[](const KeyType& key);

    vector<KeyType> findRange(const KeyType& lowKey, const KeyType& highKey) const;

    vector<KeyType> keys() const;
    ValueType size() const;
    ValueType getHeight() const;

    AVLTree(); // Default constructor
    AVLTree(const AVLTree& other); // Copy constructor

    AVLTree& operator=(const AVLTree& other); // Assignment operator
    ~AVLTree(); // deconstructor

    size_t getTreeHeight() const;

    friend ostream& operator<<(ostream& os, const AVLTree& avlTree);


protected:
    class AVLNode {
    public:
        KeyType key;
        ValueType value;
        size_t height;

        AVLNode* left;
        AVLNode* right;

        // 0, 1 or 2
        size_t numChildren() const;
        // true or false
        bool isLeaf() const;
        // number of hops to deepest leaf node
        size_t getHeight() const;
    };

private:
    AVLNode* root;
    int getNodeHeight(AVLNode* node) const;
    bool insertNode(AVLNode*& current, const KeyType& key, const ValueType& value);
    bool containsNode(AVLNode* node, const KeyType& key) const;
    optional<ValueType> getNode(AVLNode* node, const KeyType& key) const;
    AVLNode* nodeOperator(AVLNode*& node, const KeyType& key);

    void searchAndDestroy(AVLNode* node); // (get it? Like Metallica >.<)  helper: finds node and deletes it
    /* Helper methods for remove */
    // removeNode contains the logic for actually removing a node based on the numebr of children
    bool removeNode(AVLNode*& current);
    // this overloaded remove will do the recursion to remove the node
    bool remove(AVLNode*& current, KeyType key);
    // You will implement this, but it is needed for removeNode()
    void balanceNode(AVLNode*& node);
    //helpers
    void rotateToRight(AVLNode*& node);
    void rotateToLeft(AVLNode*& node);

    AVLNode* deepCopy(AVLNode* node);
};

#endif //AVLTREE_H
