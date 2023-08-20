#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <istream>
#include <climits>
#include <sstream>
#include <chrono>
#include <string>

using namespace std;

class Node{
    public:
    int val;
    int freq;
    Node* parent;
    // unordered_map<int, Node*> children;
    vector<Node*> childrens;
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
                // curr->children[t] = node;
                curr->childrens.push_back(node);
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
        // if(n->children.find(v) != n->children.end()){
        //     return n->children[v];
        // }
        for(Node* c:n->childrens){
            if(c->val == v){
                return c;
            }
        }
        return nullptr;
    }   
};

void minefrequentPattern(FPTree& tree, int minSupport, int minSize, vector<vector<int>>& patterns, map<int, int>& initialfrequencyMap, vector<int> pattern={}){
    vector<pair<int, vector<Node*>>> ht;
    for(auto& valuePair:tree.headerTable){
        ht.push_back(valuePair);
    }
    sort(ht.begin(), ht.end(), [&initialfrequencyMap](pair<int, vector<Node*>>& a, pair<int, vector<Node*>>& b){
            return initialfrequencyMap[a.first] < initialfrequencyMap[b.first];
    });
    for(auto& valuePairs:ht){
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
            map<int, int> frequencyMap;
            for(auto& node:nodes){
                vector<int> path;
                Node* curr = node;
                while(curr->parent != nullptr){
                    path.push_back(curr->val);
                    if(frequencyMap.find(curr->val) == frequencyMap.end()){
                        frequencyMap[curr->val] = 0;
                    }
                    frequencyMap[curr->val] += 1;
                    curr = curr->parent;
                }

                reverse(path.begin(), path.end());
                
                if(!path.empty()){
                    path.pop_back();
                    frequencyMap.erase(node->val);
                    nextTree.insert(path, node->freq);
                }
            }
            minefrequentPattern(nextTree, minSupport, minSize, patterns, frequencyMap, newPatterns);
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
    while(getline(file, line)){
        istringstream iss(line);
        int data;
        vector<int> dv;
        while(iss >> data){
            dv.push_back(data);
        }
        sort(dv.begin(), dv.end(), [&initialFrequencyMap](int a, int b){
            return initialFrequencyMap[a] > initialFrequencyMap[b];
        });
        vector<int> v(dv.begin(), min(dv.end(), dv.begin()+26));
        tree.insert(v);
    }
    file.close();
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




vector<int> compress(vector<int>& v, vector<vector<int>>& convertTo){
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
            t.push_back(-1*i-2);
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

void writeCompressedOutput(string inputFileName, string outputFileName, vector<vector<int>>& convertTo){
    ifstream file(inputFileName);
    ofstream outputFile(outputFileName);
    // long count = 0;
    if(!file.is_open()){
        cerr<<"Can't open file"<<endl;
        return;
    }
    string line;
    outputFile<<convertTo.size()<<endl;
    for(int i = 0; i < convertTo.size(); i++){
        // outputFile << maxVal + i << " ";
        outputFile << -1*i - 2<< " ";
        for(int j = 0;j < convertTo[i].size(); j++){
            outputFile << convertTo[i][j] << " ";
            // count++;
        }
        outputFile << endl;
    }
    while(getline(file, line)){
        istringstream iss(line);
        int data;
        vector<int> dv;
        while(iss >> data){
            dv.push_back(data);
        }
        vector<int> out = compress(dv, convertTo);
        for(int i: out){
            // count++;
            outputFile << i << " ";
        }
        outputFile << endl;
    }
    outputFile.close();
    file.close();
    // return count;
}

void writeExpandedOutput(string inputFileName, string outputFileName){
    ofstream outputFile(outputFileName);
    ifstream file(inputFileName);
    map<int, vector<int>> expand;
    if(!file.is_open()){
        cerr<<"error in opening file "<<endl;
        return;
    }
    int cnt = 0;
    string line;
    int mappingSize;
    while(getline(file, line)){
        istringstream iss(line);
        int data;
        vector<int> dv;
        if(cnt == 0){
            iss >> data;
            mappingSize = data;
            cnt++;
        }
        else if(cnt <= mappingSize){
            while(iss>>data){
                dv.push_back(data);
            }
            for(int i = 1; i < dv.size(); i++){
                expand[dv[0]].push_back(dv[i]);
            }
            cnt++;
        }
        
        else if(cnt > mappingSize && outputFile.is_open()){
            while(iss >> data){
                dv.push_back(data);
            }
            for(int i: dv){
                if(expand.find(i) == expand.end()){
                    outputFile << i << " ";
                }else{
                    for(int j: expand[i]){
                        outputFile << j << " ";
                    }
                }
            }
            outputFile << endl;
        }
    }
    file.close();
    outputFile.close();
}

int main(int argv, char* argc[]){
    FPTree fp;
    if(*argc[1] == 'C'){
        string fileName = argc[2];
        auto start = chrono::high_resolution_clock::now();
        map<int, int> initialFrequencyMap;
    
        int rows = readDataFirstPass(initialFrequencyMap, fileName);
        readDataSecondPass(fp, initialFrequencyMap, fileName);
    
        // long initialCount = 0;
        // for(auto& p:initialFrequencyMap){
        //     initialCount += p.second;
        // }
    
        vector<vector<int>> convertTo;
    
        int minSize = 2;
        int minSupport = rows/80;
        if(rows < 5000){
            // minSize = 3;
            minSupport = 1000;
        } 
        // cout<<"Initial Count: "<<initialCount<<endl;
        minefrequentPattern(fp, minSupport, minSize, convertTo, initialFrequencyMap);
        sort(convertTo.begin(), convertTo.end(), sizeCompare);

        map<int, vector<int>> expand;
        for(int i = 0; i < convertTo.size(); i++){
            expand[-1*i-2] = convertTo[i];
        }
        vector<vector<int>> convertto(convertTo.begin(), min(convertTo.end(), convertTo.begin() + 550));
        // int finalMappingSize = convertto.size();
        writeCompressedOutput(fileName, "compressed_dataset.dat", convertto);
        // cout<<"Final Count: "<<finalCount<<endl;
        // auto end = chrono::high_resolution_clock::now();
        // auto duration = chrono::duration_cast<chrono::milliseconds>(end-start);
        // cout<<"Run compression duration: "<<duration.count()/1000.0<<" sec"<<endl;
        
    }else{
        string fileName = argc[2];
        writeExpandedOutput(fileName, "reconstructed.dat");
    }
    
    
    // auto end = chrono::high_resolution_clock::now();
    // auto duration = chrono::duration_cast<chrono::milliseconds>(end-start);
    // cout<<"Run time duration: "<<duration.count()/1000.0<<" sec"<<endl;
    // 
    return 0;
}
