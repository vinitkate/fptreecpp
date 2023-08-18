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

void minefrequentPattern(FPTree& tree, int minSupport, int minSize, vector<vector<int>>& patterns, vector<int> pattern={}){
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
            if(newPatterns.size() >= minSize){
                patterns.push_back(newPatterns);
                // cout<<"New pattern: ";
                // for(auto v:newPatterns){
                //     cout<<v<<" ";
                // }
                // cout<<" Support is: "<<support<<endl;
            }
            
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
            minefrequentPattern(nextTree, minSupport, minSize, patterns, newPatterns);
        }
    }
}

int readDataFirstPass(map<int, int>& initialFreqMap, string fileName){
    ifstream file(fileName);
    int rows = 0;
    if(!file.is_open()){
        cerr<<"Error in opening"<<endl;
        return -1;
    }
    string line;
    while(getline(file, line)){
        istringstream iss(line);
        int data;
        rows++;
        while(iss >> data){
            if(initialFreqMap.find(data) == initialFreqMap.end()){
                initialFreqMap[data] = 1;
            }else{
                initialFreqMap[data] += 1;
            }
        }
    }
    return rows;
}

void readDataSecondPass(FPTree& tree, map<int, int>& initialFrequencyMap, string fileName){
    ifstream file(fileName);

    if(!file.is_open()){
        cerr << "Failed to open" <<endl;
        return;
    }
    string line;
    vector<vector<int>> dataVectors;
    while(getline(file, line)){
        istringstream iss(line);
        int data;
        vector<int> dv;
        while(iss >> data){
            dv.push_back(data);
        }
        dataVectors.push_back(dv);
    }
    file.close();

    for(vector<int> dv: dataVectors){
        sort(dv.begin(), dv.end(), [&initialFrequencyMap](int a, int b){
            return initialFrequencyMap[a] > initialFrequencyMap[b];
        });
        tree.insert(dv);
    }
}

int binarySearch(vector<int>& vec, int ele, int start){
    int l = start, r = vec.size()-1;
    while(l <= r){
        int m = l + (r - l)/2;
        if(vec[m] == ele){
            return m;
        }else if(vec[m] > ele){
            r = m-1;
        }else{
            l = m+1;
        }
    }
    return -1;
}




vector<int> compress(vector<int>& v, vector<vector<int>>& convertTo, int maxVal){
    int range = 100;
    vector<int>& ans = v;
    int vecSize = v.size();
    int sz = convertTo.size();
    for(int i = 0; i < sz; i++){
        vector<int> temp = convertTo[i];
        if(temp.size() > ans.size()){
            continue;
        }
        vector<int> t;
        sort(temp.begin(), temp.end());
        int l = 0, r = 0, f = 1;
        while(l < temp.size() && r < ans.size()){
            // if(temp[l] == ans[r]){
            //     l++;
            //     r++;
            //     f++;
            // }else if(ans[r] < temp[l]){
            //     t.push_back(ans[r]);
            //     r++;
            // }else{
            //     l++;
            // }

            int i = binarySearch(ans, temp[l], r);
            if(i == -1){
                f = 0;
                break;
            }else{
                int m = r;
                while(m < i){
                    t.push_back(ans[m]);
                    m++;
                }
                r = i+1;
            }
            l++;
        }

        if(f && l == temp.size()){
            while(r < ans.size()){
                t.push_back(ans[r++]);
            }
            t.push_back(maxVal+i);
            // cout<<"Compressed: "<<ans.size()<<" "<<t.size()<<endl;
            ans = t;
            sort(ans.begin(), ans.end());
        }
    }
    return ans;
}

bool sizeCompare(vector<int>& a, vector<int>& b){
    return a.size() > b.size();
}

int writeCompressedOutput(string inputFileName, string outputFileName, vector<vector<int>>& convertTo, int maxVal){
    ifstream file(inputFileName);
    ofstream outputFile(outputFileName);
    int count = 0;
    if(!file.is_open()){
        cerr<<"Can't open file"<<endl;
        return 0;
    }
    string line;
    vector<vector<int>> dataVectors;
    while(getline(file, line)){
        istringstream iss(line);
        int data;
        vector<int> dv;
        while(iss >> data){
            dv.push_back(data);
        }
        dataVectors.push_back(dv);
    }
    for(int i = 0; i < convertTo.size(); i++){
        outputFile << maxVal + i << " ";
        for(int j = 0;j < convertTo[i].size(); j++){
            outputFile << convertTo[i][j] << " ";
            count++;
        }
    }
    for(vector<int> dv: dataVectors){
        if(outputFile.is_open()){
            vector<int> out = compress(dv, convertTo, maxVal);
            for(int i: out){
                count++;
                outputFile << i << " ";
            }
            outputFile << endl;
        }
    }
    outputFile.close();
    file.close();
    return count;
}

void writeExpandedOutput(map<long, vector<int>>& expand, string inputFileName, string outputFileName){
    ofstream outputFile1(outputFileName);
    ifstream file1(inputFileName);
    if(!file1.is_open()){
        cerr<<"Unable to open "<<endl;
        return;
    }
    string line1;
    while(getline(file1, line1)){
        istringstream iss(line1);
        int data;
        vector<int> dv;
        while(iss >> data){
            dv.push_back(data);
        }
        if(outputFile1.is_open()){
            for(int i: dv){
                if(expand.find(i) == expand.end()){
                    outputFile1 << i << " ";
                }else{
                    for(int j: expand[i]){
                        outputFile1 << j << " ";
                    }
                }
            }
            outputFile1 << endl;
        }
    }
    file1.close();
    outputFile1.close();
}

int main(){
    FPTree fp;

    string fileName = "D_medium.dat";
    auto start = chrono::high_resolution_clock::now();
    map<int, int> initialFrequencyMap;

    int rows = readDataFirstPass(initialFrequencyMap, fileName);
    readDataSecondPass(fp, initialFrequencyMap, fileName);

    int totalFreq = 0, minFreq = INT_MAX, maxFreq = 0;
    int maxval = 0;
    for(auto& p:initialFrequencyMap){
        totalFreq += p.second;
        minFreq = min(minFreq, p.second);
        maxFreq = max(maxFreq, p.second);
        maxval = max(maxval, p.first);
    }
    maxval++;
    long maxVal = maxval;

    vector<vector<int>> convertTo;

    int minSize = 2;
    int minSupport = rows/100;
    if(rows < 5000){
        minSupport = 1700;
    } 
    cout<<"Initial Count: "<<totalFreq<<endl;
    minefrequentPattern(fp, minSupport, minSize, convertTo);
    sort(convertTo.begin(), convertTo.end(), sizeCompare);

    map<long, vector<int>> expand;
    for(long i = 0; i < convertTo.size(); i++){
        expand[maxVal+i] = convertTo[i];
    }
    cout<<convertTo.size()<<endl;
    vector<vector<int>> convertto(convertTo.begin(), min(convertTo.end(), convertTo.begin() + 1500));

    int finalCount = writeCompressedOutput(fileName, "output.dat", convertto, maxVal);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end-start);
    cout<<"Run time duration: "<<duration.count()/1000.0<<" "<<endl;
    cout<<"Final Count: "<<finalCount<<endl;
    return 0;
}
