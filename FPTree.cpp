#include <bits/stdc++.h>

using namespace std;

class Node{
    public:
    int val;
    int freq;
    Node* parent;
    unordered_map<int, Node*> children;
};

class FPTree{
    Node* root;
    public: 
    map<int, vector<Node*>> headerTable;
    FPTree(){
        root = new Node();
        root->val = -1;
    };

    void insert(vector<int>& transactions, int count=1){
        Node* curr = root;
        for(auto t: transactions){
            Node* next = findChildren(curr, t);
            if(next == nullptr){
                Node* node = new Node();
                node->val = t;
                node->freq = count;
                node->parent = curr;
                curr->children[t] = node;
                curr = node;
                if(headerTable.find(t) == headerTable.end()){
                    headerTable[t] = {node};
                }else{
                    headerTable[t].push_back(node);
                }
            }else{
                next->freq += count;
                curr = next;
            }
        }
    }

    Node* findChildren(Node* n, int v){
        if(n->children.find(v) != n->children.end()){
            return n->children[v];
        }
        return nullptr;
    }   

    void dfs(){
        unordered_set<Node*> vis;
        dfs(this->root, vis);
        cout<<endl;
    }

    void dfs(Node* r, unordered_set<Node*>& visited){
        if(visited.find(r) == visited.end()){
            cout<<r->val<<"--"<<r->freq<<"  ";
            visited.insert(r);
            for(auto& child:r->children){
                dfs(child.second, visited);
            }
        }

    }

    void printTable(){
        for(auto& itemPair:headerTable){
            cout<<itemPair.first<<" ";
            for(auto& node:itemPair.second){
                cout<<node->val<<" ";
            }
            cout<<endl;
        }
    }
};

void minefrequentPattern(FPTree& tree, int minSupport, vector<vector<int>>& patterns, vector<int> pattern={}){
    for(auto& valuePairs:tree.headerTable){
        int item = valuePairs.first;
        vector<Node*>& nodes = valuePairs.second;

        int support = 0;
        for(auto& node: nodes){
            support += node->freq;
        }
        if(support >= minSupport){
            vector<int> newPatterns = pattern;
            newPatterns.push_back(item);
            patterns.push_back(newPatterns);
            cout<<"New pattern: ";
            for(auto v:newPatterns){
                cout<<v<<" ";
            }
            cout<<" Support is: "<<support<<endl;

            FPTree nextTree;

            for(auto& node:nodes){
                vector<int> path;
                Node* curr = node;
                while(curr->parent != nullptr){
                    path.push_back(curr->val);
                    curr = curr->parent;
                }
                reverse(path.begin(), path.end());
                
                if(!path.empty()){
                    path.pop_back();
                    nextTree.insert(path, node->freq);
                }
            }
            minefrequentPattern(nextTree, minSupport, patterns, newPatterns);
        }
    }
}

int main(){
    vector<vector<int>> v = {
        {1, 2, 3},
        {2, 3, 4},
        {1, 3, 4},
        {2, 3, 4, 5},
        {2, 3},
        {1, 2, 4},
        {1, 3}
    };
    FPTree fp;
    for(auto& vec:v){
        fp.insert(vec);
    }
    // fp.dfs();
    map<vector<int>, int> patterns;
    vector<vector<int>> ptrns;
    fp.printTable();
    minefrequentPattern(fp, 2, ptrns);

    for(auto& pattern:ptrns){
        for(int v:pattern){
            cout<<v<<" ";
        }
        cout<<endl;
    }
    return 0;
}