#include "AVLTree.h"

#include <functional>
#include <string>
#include <bits/locale_facets_nonio.h>

// ----CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT OPERATOR---------------------------------------

// Default constructor
AVLTree::AVLTree() : root(nullptr), treeSize(0) {}

// Copy constructor
AVLTree::AVLTree(const AVLTree& other) : root(nullptr), treeSize(other.treeSize) {
    root = deepCopy(other.root);
}

// operator assignment
AVLTree& AVLTree::operator=(const AVLTree& other) {
    // Check if self-assigned
    if (this == &other) {
        return *this;
    }

    // Free existing tree
    searchAndDestroy(root);
    root = deepCopy(other.root); // deep copy from other tree
    treeSize = other.treeSize;
    return *this;
}

// Destructor
AVLTree::~AVLTree() {
    searchAndDestroy(root);
    root = nullptr;
    treeSize = 0;
}

// AVLNode methods------------------------------------------------------

size_t AVLTree::AVLNode::numChildren() const {
    // Count left and right children (increment) and return the num
    size_t numChildren = 0;
    if (left) {
        numChildren++;
    }
    if (right) {
        numChildren++;
    }
    return numChildren;
}

bool AVLTree::AVLNode::isLeaf() const {
    return (left == nullptr && right == nullptr);
}

size_t AVLTree::AVLNode::getHeight() const {
    return height;
}

// HEIGHT HELPERS------------------------------------------------------------

int AVLTree::getNodeHeight(AVLNode* node) const {
    return node ? node->height : 0;
}

size_t AVLTree::getTreeHeight() const {
    return root ? root->height : 0;
}

// PUBLIC WRAPPERS----------------------------------------------------------------

// node parameter is always the root node
bool AVLTree::insert(const KeyType& key, const ValueType& value) {
    bool insert = insertNode(root, key, value);
    if (insert) {
        treeSize++;
    }

    return insert;
}

bool AVLTree::remove(const KeyType& key) {
    bool removed = remove(root, key);
    if (removed) {
        treeSize--;
    }

    return removed;
}

bool AVLTree::contains(const KeyType& key) const {
    return containsNode(root, key);
}

optional<size_t> AVLTree::get(const KeyType& key) const {
    return getNode(root, key);
}

size_t& AVLTree::operator[](const KeyType& key) {

    // if node exists, return reference to it
    // if it does not exist, insert a default value of 0 and return
    AVLNode* nodeFound = nodeOperator(root, key);
    if (nodeFound) {
        return nodeFound->value;
    }

    // insert default
    bool insert = insertNode(root, key, ValueType(0));
    if (insert) {
        treeSize++;
    }

    // Got-em
    nodeFound = nodeOperator(root, key);
    return nodeFound->value;
}

// keys, size, findRange

vector<size_t> AVLTree::findRange(const KeyType& lowKey, const KeyType& highKey) const {

    vector<size_t> res;
    findRangeHelper(root, lowKey, highKey, res);
    return res;
}

size_t AVLTree::size() const {
    return treeSize; // insert treeSize++, delete treeSize--
}

vector<string> AVLTree::keys() const {
    vector<KeyType> res;
    getKeys(root, res); // convert to vector<string>
    return res;
}

// INSERT RECURSION--------------------------------------------------------

bool AVLTree::insertNode(AVLNode*& current, const KeyType& key, const ValueType& value) {

    // Base case: recurse and find null spot.
    if (current == nullptr) {
        current = new AVLNode{key, value};
        current->height = 1;
        return true; // go back to parent now
    }

    // Check for duplicate key
    if (key == current->key) {
        return false;
    }

    bool success;

    if (key < current->key) {
        success = insertNode(current->left, key, value);
    } else {
        success = insertNode(current->right, key, value);
    }

    // If insertion failed because of a duplicate then stop
    if (!success) {
        return false;
    }

    size_t leftHeight = 0, rightHeight = 0;
    if (current->left != nullptr) {
        leftHeight = current->left->getHeight();
    }

    if (current->right != nullptr) {
        rightHeight = current->right->getHeight();
    }

    current->height = 1 + std::max(leftHeight, rightHeight);

    // Rebalance
    balanceNode(current);
    return true; // After successful balance AND insertion

}

// REMOVE RECURSION---------------------------------------------------------

bool AVLTree::removeNode(AVLNode*& current){
    if (!current) {
        return false; // Nothing to delete
    }

    //AVLNode* toDelete = current;

    auto nChildren = current->numChildren();

    // CASE ONE: NO CHILD
    if (current->isLeaf()) {
        // case 1 we can delete the node
        delete current;
        current = nullptr; // Parent pointer points to nullptr now
        treeSize--;
        return true;
    }

    // CASE 2: ONE CHILD
    if (current->numChildren() == 1) {
        // case 2 - replace current with its only child

        // pick one child
        AVLNode* child = nullptr;
        if (current->left != nullptr) {
            child = current->left;
        } else {
            child = current->right;
        }

        delete current; // Delete original node
        current = child; // Replace node with its child
        treeSize--;
        return true;
    }
    // CASE 3: TWO CHILDREN
        // case 3 - we have two children,
        // get smallest key in right subtree by
        // getting right child and go left until left is null
        AVLNode* smallestInRight = current->right;
        // I could check if smallestInRight is null,
        // but it shouldn't be since the node has two children
        while (smallestInRight->left != nullptr) {
            smallestInRight = smallestInRight->left;
        }

        // Copy successor pair into this current node
        std::string newKey = smallestInRight->key;
        size_t newValue = smallestInRight->value;
        current->key = newKey;
        current->value = newValue;

        // DON'T NEED THIS: my remove function will utilize balance and height
        // current->height = current->getHeight();
        // balanceNode(current);

    // Remove successor node from the right subtree
    bool removedNode = remove(current->right, smallestInRight->key);

    return removedNode;

    }

// private remove
bool AVLTree::remove(AVLNode*& current, KeyType key) {

    // Base case
    // Tree is empty or we reached a dead end
    if (!current) {
        return false;
    }

    // Search left subtree for the node to remove (key-matching)
    if (key < current->key) {
        bool found = remove(current->left, key);

        if (!found) {
            return false;
        }

        // ---POST NODE DELETION--- //
        // Node was removed->update height and rebalance
        size_t leftHeight = 0, rightHeight = 0;
        if (current->left != nullptr) {
            leftHeight = current->left->getHeight();
        }

        if (current->right != nullptr) {
            rightHeight = current->right->getHeight();
        }

        current->height = 1 + std::max(leftHeight, rightHeight);

        balanceNode(current);

        // Continue up the tree
        return true;
    }
    // Search right subtree
    else if (key > current->key) {
        bool found = remove(current->right, key);
        if (!found) {
            return false;
        }

        // Update height and rebalance
        size_t leftHeight = 0, rightHeight = 0;
        if (current->left != nullptr) {
            leftHeight = current->left->getHeight();
        }

        if (current->right != nullptr) {
            rightHeight = current->right->getHeight();
        }

        current->height = 1 + std::max(leftHeight, rightHeight);

        balanceNode(current);
        return true;
    }
    else {
        // Found the node (assuming key == current->key)
        return removeNode(current);
    }

}

// BALANCING AND ROTATIONS-----------------------------------------------------

void AVLTree::balanceNode(AVLNode *&node) {

    // No using recursion**

    if (node == nullptr) {
        return;
    }

    int balance = getNodeHeight(node->left) - getNodeHeight(node->right);

    // CASE 1: LEFT HEAVY
    if (balance > 1) {
        int llHeight = getNodeHeight(node->left->left);
        int lrHeight = getNodeHeight(node->left->right);

        // Left-left subcase
        if (llHeight >= lrHeight) {
            // Rotate node to the right
            rotateToRight(node);
        }
        // left-right
        else {
            // Double rotation: node->left
            // Single rotation: node
            rotateToLeft(node->left);
            rotateToRight(node);
        }
        return;
    }

    // CASE 2: RIGHT HEAVY
    if (balance < -1) {

        int rrHeight = getNodeHeight(node->right->right);
        int rlHeight = getNodeHeight(node->right->left);
        // right-right subcase
        if (rrHeight >= rlHeight) {
            // Rotate node to the left
            rotateToLeft(node);
        }
        // right-left
        else {
            // Double rotation: node->right
            // Single rotation: node
            rotateToRight(node->right);
            rotateToLeft(node);
        }
        return;
    }
}

// Rotate right helper
void AVLTree::rotateToRight(AVLNode*& node) {

    // Store values
    AVLNode* hook = node->left; // new root ( B is left of A )
    AVLNode *hookRight = hook->right;;

    // Rotate
    hook->right = node; //hook becomes the root: right of B is now A
    node->left = hookRight; // Right node from hook rotates left of prev root node (left of A is D)

    node->height = 1 + std::max(getNodeHeight(node->left), getNodeHeight(node->right));
    hook->height = 1 + std::max(getNodeHeight(hook->left), getNodeHeight(hook->right));

    node = hook; // hook becomes new root

}

// Rotate left helper
void AVLTree::rotateToLeft(AVLNode*& node) {

    // Store values
    AVLNode* hook = node->right; // new root ( B is right of A )
    AVLNode* hookLeft = hook->left; // To the left of the hook (D)

    // Rotate
    hook->left = node; //hook becomes the root: left of B is now A
    node->right = hookLeft; // Left node from hook rotates right of prev root node (right of A is D)

    node->height = 1 + std::max(getNodeHeight(node->left), getNodeHeight(node->right));
    hook->height = 1 + std::max(getNodeHeight(hook->left), getNodeHeight(hook->right));

    node = hook; // hook becomes new root

}

// find and get helpers----------------------------------------------------------

bool AVLTree::containsNode(AVLNode* node, const KeyType& key) const {
    // Check for node
    if (node == nullptr) {
        return false;
    }

    // if key is in the tree, return true
    if (key == node->key) {
        return true;
    }
    else if (key < node->key) {
        return containsNode(node->left, key); // left subtree
    }
    else {
        return containsNode(node->right, key); // right subtree
    }
}



optional<size_t> AVLTree::getNode(AVLNode* node, const KeyType& key) const {
    // key not found
    if (node == nullptr) {
        return nullopt;
    }

    // Key found return val
    if (key == node->key) {
        return node->value;
    }

    if (key < node->key) {
        return getNode(node->left, key);
    } else {
        return getNode(node->right, key);
    }
}



AVLTree::AVLNode* AVLTree::nodeOperator(AVLNode*& node, const KeyType& key) {
    // Base case: no node, so create a new one
    if (node == nullptr) {
        node = new AVLNode(key, 0);
        node->height = 1;
        return node;
    }

    // left
    if (key < node->key) {
        AVLNode* newNode = nodeOperator(node->left, key);
        balanceNode(node);
        return newNode;
    }

    // right
    if (key > node->key) {
        AVLNode* newNode = nodeOperator(node->right, key);
        balanceNode(node);
        return newNode;
    }

    return node; // key exists already, so return existing node
}

// Range and key helpers------------------------------------------------------

/**
 *(helper)
 *if node == nullptr: return
 *if node->key >= lowKey: return left
 *if lowKey <= node->key <= highKey: add node->value to vector
 *if node->key <= highKey: recurse to the right
 */
void AVLTree::findRangeHelper(AVLNode* node, const KeyType& lowKey, const KeyType& highKey, vector<size_t>& res) const {
    if (node == nullptr) {
        return;
    }

    // If node is greater, the left subtree might have valid keys?
    if (node->key >= lowKey) {
        findRangeHelper(node->left, lowKey, highKey, res);
    }

    // If this key is in the range, then add a value
    if (node->key >= lowKey && node->key <= highKey) {
        res.push_back(node->value);
    }

    // node key is smaller, right subtree might have keys
    if (node->key <= highKey) {
        findRangeHelper(node->right, lowKey, highKey, res);
    }
}



void AVLTree::getKeys(AVLNode* node, vector<KeyType>& vec) const {
    if (node == nullptr) {
        return;
    }

    getKeys(node->left, vec);
    vec.push_back(node->key);
    getKeys(node->right, vec);
}

// Deep copy and destroy-------------------------------------------------------
// Private helper for copy constructor
AVLTree::AVLNode* AVLTree::deepCopy(AVLNode* node) {
// This recursively copies nodes
    if (node == nullptr) {
        return nullptr;
    }

    // Create node with the same parameters
    AVLNode* newNode = new AVLNode(node->key, node->value, node->height, nullptr, nullptr);

    // Copy left subtree
    newNode->left = deepCopy(node->left);

    // Copy right subtree
    newNode->right = deepCopy(node->right);

    return newNode;
}

// Destructor helper
void AVLTree::searchAndDestroy(AVLNode* node) {
    // Base case if there is no more nodes (except root)
    if (node == nullptr) {
        return;
    }

    searchAndDestroy(node->left);
    searchAndDestroy(node->right);
    delete node;
}

// print-------------------------------------------------------------------
ostream& operator<<(ostream& os, const AVLTree& avlTree) {
    function<void(AVLTree::AVLNode*)> print;
    print = [&](AVLTree::AVLNode* node) {
        if (node == nullptr) {
            return;
        }
        print(node->left);
        os << node->key;
        print(node->right);
    };

    print(avlTree.root);

    return os;
}
