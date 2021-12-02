// priorityqueue.h
// Author: Hamza Sheikh
// project 5:  priorityqueue
// This class is a priority queue implementation using
// a custom binary search tree.
//
#pragma once

#include <iostream>
#include <sstream>
#include <set>

using namespace std;

template<typename T>
class priorityqueue {
private:
    struct NODE {
        int priority;  // used to build BST
        T value;  // stored data for the p-queue
        bool dup;  // marked true when there are duplicate prioritiesne
        NODE* parent;  // links back to parent
        NODE* link;  // links to linked list of NODES with duplicate priorities
        NODE* left;  // links to left child
        NODE* right;  // links to right child
    };
    NODE* root;  // pointer to root node of the BST
    int size;  // # of elements in the pqueue
    NODE* curr;  // pointer to next item in pqueue (see begin and next)
    // This function takes in the value and priority, then
    // allocates memory for a new node in the tree and returns it.
    NODE* makeNode (T value, int priority) {
        NODE* newNode = new NODE;
        newNode->priority = priority;
        newNode->value = value;
        newNode->left = newNode->right = nullptr;
        newNode->link = nullptr;
        newNode->dup = false;
        return newNode;
    }
    
    // This function takes the root of the tree and a string.
    // It then traverses the tree using the inorder traversal
    // and concatenates each value to the string as it goes.
    void inorder (NODE* root, string& str) {
        if (root == nullptr)
            return;
        inorder(root->left, str);
        stringstream ss;
        if (root->dup) {
            NODE* tmp = root;
            while (tmp != nullptr) {
                ss << tmp->priority << " value: "
                << tmp->value << endl;
                tmp = tmp->link;
            }
            str += ss.str();
        } else {
            ss << root->priority << " value: "
            << root->value << endl;
            str += ss.str();
        }
        inorder(root->right, str);
    }
    
    // This function traverses through the tree and for each
    // node deletes it and frees the memory.
    void clearTree(NODE* root) {
        if (root == nullptr)
            return;
        clearTree(root->left);
        clearTree(root->right);
        if (root->dup) {
            NODE* head = root->link;
            NODE* tmp = nullptr;
            while (head != nullptr) {
                tmp = head->link;
                delete head;
                head = tmp;
            }
        }
        delete root;
    }
    
    // This funciton takes in a NODE pointer to the root of a tre 
    // It then makes a new tree that is the exact copy of the
    // tree that was passed in and returns the pointer to the
    // root of the newly created tree
    NODE* deepCopy(NODE* otherRoot) {
        if (otherRoot == nullptr) {
            return nullptr;
        }
        NODE* newNode = new NODE;
        newNode->value = otherRoot->value;
        newNode->priority = otherRoot->priority;
        newNode->parent = otherRoot->parent;
        newNode->dup = otherRoot->dup;
        if (newNode->dup) {
             NODE* otherTmp = otherRoot->link;
             NODE* tmp = newNode;
             NODE* add = nullptr;
             while (otherTmp != nullptr) {
                add = new NODE;
                add->value = otherTmp->value;
                add->priority = otherTmp->priority;
                add->parent = otherTmp->parent;
                add->link = otherTmp->link;
                tmp->link = add;
                tmp = add;
                otherTmp = otherTmp->link;
            }
        }
        newNode->left = deepCopy(otherRoot->left);
        newNode->right = deepCopy(otherRoot->right);
        return newNode;
    }
    
    // This function checks for equality between two nodes.
    // returns true if they are the same and false otherwise.
    bool equality(NODE* root, NODE* otherRoot) const {
        if (root == nullptr && otherRoot == nullptr)
            return true;
        else if (root == nullptr || otherRoot == nullptr)
            return false;
        else {
            return (root->value == otherRoot->value &&
            root->priority == otherRoot->priority &&
            equality(root->left, otherRoot->left) && 
            equality(root->right, otherRoot->right) &&
            equality(root->link, otherRoot->link));
        }
    }

public:
    //
    // default constructor:
    //
    // Creates an empty priority queue.
    // O(1)
    //
    priorityqueue() {
        size = 0;
        root = nullptr;
        curr = nullptr;
    }
    
    //
    // operator=
    //s
    // Clears "this" tree and then makes a copy of the "other" tree.
    // Sets all member variables appropriately.
    // O(n), where n is total number of nodes in custom BST
    //
    priorityqueue& operator=(const priorityqueue& other) {
        // // if a priorityqueue is being set to itself
        // if (this == &other)
        //     return *this;
        clear();
        this->size = other.size;
        this->curr = other.curr;
        this->root = deepCopy(other.root);
        return *this;
    }
    
    //
    // clear:
    //
    // Frees the memory associated with the priority queue but is public.
    // O(n), where n is total number of nodes in custom BST
    //
    void clear() {
        clearTree(root);
        root = nullptr;
        size = 0;
        curr = nullptr;
    }
    
    //
    // destructor:
    //
    // Frees the memory associated with the priority queue.
    // O(n), where n is total number of nodes in custom BST
    //
    ~priorityqueue() {
        clear();
    }
    
    //
    // enqueue:
    //
    // Inserts the value into the custom BST in the correct location based on
    // priority.
    // O(logn + m), where n is number of unique nodes in tree and m is number of
    // duplicate priorities
    //
    void enqueue(T value, int priority) {
        // if tree is empty
        if (root == nullptr) {
            root = makeNode(value, priority);
            root->parent = nullptr;
            size++;
            return;  // ends the function here
        }
        // if root != nullptr, traverses the tree and finds
        // the node where it should insert the new NODE
        NODE* tmp = root;
        NODE* tmpParent = nullptr;
        while (tmp != nullptr) {
            tmpParent = tmp;
            if (priority == tmp->priority)
                break;  // dup found
            else if (priority < tmp->priority)
                tmp = tmp->left;
            else 
                tmp = tmp->right;
        }
        if (priority == tmpParent->priority) {
            tmpParent->dup = true;
            NODE* newNode = tmpParent;
            NODE* prev = nullptr;
            // go to the end of linked list
            while (newNode != nullptr) {
                prev = newNode;
                newNode = newNode->link;
            }
            newNode = makeNode(value, priority);
            newNode->dup = true;
            newNode->parent = tmpParent;
            prev->link = newNode;
        }
        // insert to left
        else if (priority < tmpParent->priority) {
            tmpParent->left = makeNode(value, priority);
            tmpParent->left->parent = tmpParent;
        } 
        // insert to right
        else {
            tmpParent->right = makeNode(value, priority);
            tmpParent->right->parent = tmpParent;
        }
        size++;
    }
    //
    // dequeue:
    //
    // returns the value of the next element in the priority queue and removes
    // the element from the priority queue.
    // O(logn + m), where n is number of unique nodes in tree and m is number of
    // duplicate priorities
    //
    T dequeue() {
        T valueOut;
        NODE* originalCurr = curr;
        begin();
        NODE* tmp = curr;
        curr = originalCurr;
        // if leaf node
        if (tmp->right == nullptr && tmp->left == nullptr){
            if (tmp == root) {
                if (tmp->link != nullptr) {
                    root = tmp->link;
                    root->parent = nullptr;
                }
                else
                    root = nullptr;
            }
            else {
                if (tmp->link != nullptr) {
                    tmp->parent->left = tmp->link;
                    tmp->link->parent = tmp->parent;
                } else
                    tmp->parent->left = nullptr;
            }
            valueOut = tmp->value;
            size--;
            delete tmp;
        }
        // if node with one child
        else if (tmp->right == nullptr || tmp->left == nullptr) {
            if (tmp == root) {
                if (tmp->link != nullptr) {
                    root = tmp->link;
                    root->parent = nullptr;
                    root->right = tmp->right;
                }
                else {
                    root = tmp->right;
                    root->parent = nullptr;
                }    
            }
            else if (tmp->parent->left == tmp && tmp->right != nullptr) {
                if (tmp->link != nullptr) {
                    tmp->parent->left = tmp->link;
                    tmp->link->parent = tmp->parent;
                    tmp->link->right = tmp->right;
                }
                else {
                    tmp->parent->left = tmp->right;
                    tmp->right->parent = tmp->parent;
                }
            }
            valueOut = tmp->value;
            delete tmp;
            size--;
        }
        return valueOut;
    }
    
    //
    // Size:
    //
    // Returns the # of elements in the priority queue, 0 if empty.
    // O(1)
    //
    int Size() {
        return size;     
    }
    
    //
    // begin
    //
    // Resets internal state for an inorder traversal.  After the
    // call to begin(), the internal state denotes the first inorder
    // node; this ensure that first call to next() function returns
    // the first inorder node value.
    //
    // O(logn), where n is number of unique nodes in tree and m is number of
    // duplicate priorities
    //
    // Example usage:
    //    pq.begin();
    //    while (tree.next(value, priority)) {
    //      cout << priority << " value: " << value << endl;
    //    }
    //    cout << priority << " value: " << value << endl;
    void begin() {
        NODE* tmp = root;
        if (tmp == nullptr) {
            this->curr = nullptr;
            return;
        }
        // leftmost node = node with smalles priority
        while (tmp->left != nullptr) {
            tmp = tmp->left;
        }
        this->curr = tmp;
    }
    
    //
    // next
    //
    // Uses the internal state to return the next inorder priority, and
    // then advances the internal state in anticipation of future
    // calls.  If a value/priority are in fact returned (via the reference
    // parameter), true is also returned.
    //
    // False is returned when the internal state has reached null,
    // meaning no more values/priorities are available.  This is the end of the
    // inorder traversal.
    //
    // O(?) - hard to say.  But approximately O(logn + m).  Definitely not O(n).
    //
    // Example usage:
    //    pq.begin();
    //    while (tree.next(value, priority)) {
    //      cout << priority << " value: " << value << endl;
    //    }
    //    cout << priority << " value: " << value << endl;
    //
    bool next(T& value, int &priority) {
        // if tree empty
        if (curr == nullptr)
            return false;
        NODE* tmp = curr;
        value = curr->value;
        priority = curr->priority;
        // if curr has dups
        if (tmp->link != nullptr) {
            value = curr->value;
            priority = curr->priority;
            curr = curr->link;
            return true;
        } else if (tmp->dup == true && tmp->link == nullptr) {
            // last node in the linked list goes back to the
            // original first node
            if (tmp->right != nullptr)
                tmp = curr;
            else
                tmp = curr->parent;
            if (tmp == nullptr)
                return false;
        }
        // if curr has no right child
        if (tmp->right == nullptr) {
            // goes up until it finds a node that is left child
            // of its parent
            while (tmp->parent != nullptr && tmp->parent->left != tmp) {
                tmp = tmp->parent;
            }
            curr = tmp->parent;
        } else {
            // if curr has a right child, goes to the smallest
            // priority in the right subtree
            tmp = tmp->right;
            while (tmp->left != nullptr) {
                tmp = tmp->left;
            }
            curr = tmp;
        }
        if (curr == nullptr)
            return false;
        return true;
    }
    
    //
    // toString:
    //
    // Returns a string of the entire priority queue, in order.  Format:
    // "1 value: Ben
    //  2 value: Jen
    //  2 value: Sven
    //  3 value: Gwen"
    //
    string toString() {
        string str = "";
        inorder(root, str);
        return str;
    }
    
    //
    // peek:
    //
    // returns the value of the next element in the priority queue but does not
    // remove the item from the priority queue.
    // O(logn + m), where n is number of unique nodes in tree and m is number of
    // duplicate priorities
    //
    T peek() {
        T valueOut;
        NODE* tmp = curr;
        begin();
        valueOut = curr->value;
        curr = tmp;
        return valueOut;
    }
    
    //
    // ==operator
    //
    // Returns true if this priority queue as the priority queue passed in as
    // other.  Otherwise returns false.
    // O(n), where n is total number of nodes in custom BST
    //
    bool operator==(const priorityqueue& other) const {
        return equality(root, other.root) && (other.size == size);
    }
    
    //
    // getRoot - Do not edit/change!
    //
    // Used for testing the BST.
    // return the root node for testing.
    //
    void* getRoot() {
        return root;
    }
};
