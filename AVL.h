#ifndef AVL_H
#define AVL_H

// AVL Tree: a BST that keeps itself balanced using rotations.
// Key = int (tracking ID), value = pointer.
// NOTE: the tree does NOT delete the records it points to.
template <typename V>
class AVL {
private:
    struct TreeNode {
        int key;
        V* value;
        TreeNode* left;
        TreeNode* right;
        int height;      // height of this node (a leaf has height 1)
        TreeNode(int k, V* v) {
            key = k;
            value = v;
            left = nullptr;
            right = nullptr;
            height = 1;
        }
    };

    TreeNode* root;
    int count;
    int lastSteps;     // nodes visited by the last search
    int rotations;     // how many rotations were done so far

    int heightOf(TreeNode* n) const {
        return (n == nullptr) ? 0 : n->height;
    }

    void updateHeight(TreeNode* n) {
        int l = heightOf(n->left);
        int r = heightOf(n->right);
        n->height = 1 + (l > r ? l : r);
    }

    // balance factor = left height - right height (must stay -1, 0 or 1)
    int balanceFactor(TreeNode* n) const {
        return heightOf(n->left) - heightOf(n->right);
    }

    // Right rotation: the left child x moves up, y becomes its right child
    TreeNode* rotateRight(TreeNode* y) {
        TreeNode* x = y->left;
        TreeNode* B = x->right;
        x->right = y;
        y->left = B;
        updateHeight(y);
        updateHeight(x);
        rotations++;
        return x;
    }

    // Left rotation: the right child y moves up, x becomes its left child
    TreeNode* rotateLeft(TreeNode* x) {
        TreeNode* y = x->right;
        TreeNode* B = y->left;
        y->left = x;
        x->right = B;
        updateHeight(x);
        updateHeight(y);
        rotations++;
        return y;
    }

    TreeNode* insertRec(TreeNode* node, int key, V* value, bool& inserted) {
        if (node == nullptr) {
            inserted = true;
            return new TreeNode(key, value);
        }

        if (key < node->key)
            node->left = insertRec(node->left, key, value, inserted);
        else if (key > node->key)
            node->right = insertRec(node->right, key, value, inserted);
        else {
            inserted = false;   // duplicate
            return node;
        }

        updateHeight(node);
        int bf = balanceFactor(node);

        // Left-Left case
        if (bf > 1 && key < node->left->key)
            return rotateRight(node);

        // Right-Right case
        if (bf < -1 && key > node->right->key)
            return rotateLeft(node);

        // Left-Right case
        if (bf > 1 && key > node->left->key) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // Right-Left case
        if (bf < -1 && key < node->right->key) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    void inorderRec(TreeNode* node, void (*visit)(int, V*)) const {
        if (node == nullptr) return;
        inorderRec(node->left, visit);
        visit(node->key, node->value);
        inorderRec(node->right, visit);
    }

    void preorderRec(TreeNode* node, void (*visit)(int, V*)) const {
        if (node == nullptr) return;
        visit(node->key, node->value);
        preorderRec(node->left, visit);
        preorderRec(node->right, visit);
    }

    void postorderRec(TreeNode* node, void (*visit)(int, V*)) const {
        if (node == nullptr) return;
        postorderRec(node->left, visit);
        postorderRec(node->right, visit);
        visit(node->key, node->value);
    }

    void destroy(TreeNode* node) {
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    AVL() {
        root = nullptr;
        count = 0;
        lastSteps = 0;
        rotations = 0;
    }

    ~AVL() {
        destroy(root);
    }

    AVL(const AVL&) = delete;
    AVL& operator=(const AVL&) = delete;

    int size() const { return count; }
    int getHeight() const { return heightOf(root); }
    int getLastSteps() const { return lastSteps; }
    int getRotations() const { return rotations; }

    bool insert(int key, V* value) {
        bool inserted = false;
        root = insertRec(root, key, value, inserted);
        if (inserted) count++;
        return inserted;
    }

    V* search(int key) {
        lastSteps = 0;
        TreeNode* cur = root;
        while (cur != nullptr) {
            lastSteps++;
            if (key == cur->key) return cur->value;
            if (key < cur->key) cur = cur->left;
            else cur = cur->right;
        }
        return nullptr;
    }

    void inorder(void (*visit)(int, V*)) const { inorderRec(root, visit); }
    void preorder(void (*visit)(int, V*)) const { preorderRec(root, visit); }
    void postorder(void (*visit)(int, V*)) const { postorderRec(root, visit); }
};

#endif