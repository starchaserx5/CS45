#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<vector<bool> > cart_product (const vector<vector<bool>>& v) {
    vector<vector<bool>> s = {{}};
    for (auto& u : v) {
        vector<vector<bool>> r;
        for (auto& x : s) {
            for (auto y : u) {
                r.push_back(x);
                r.back().push_back(y);
            }
        }
        s.swap(r);
    }
    return s;
}

int main () {

    vector<bool> binList = {true,false};
	vector<vector<bool>> test;
    for(int i =0; i < 4;++i)
        test.push_back(binList);
	vector<vector<bool> > res = cart_product(test);
	for(size_t i = 0; i < res.size();i++){
		for (size_t j = 0; j < res[i].size(); j++){
			cout << ((res[i][j] == true) ? ("True") : ("False")) << "\t";
		}
		cout << std::endl;
	}
    return 0;
}