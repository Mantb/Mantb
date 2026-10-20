#include "args.hpp"
#include "loader.hpp"
#include "imgutils/search.hpp"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    try {
        // 1. Parse Arguments
        Args args = parse_args(argc, argv);

        cout << "--- Image Search Initialized ---" << endl;
        cout << "Dataset: " << args.hp << endl;
        cout << "Distractors: " << args.mir << " (Size: " << args.D << ")" << endl;
        cout << "Vocabulary Size: " << args.vocab << endl;

        // Output chosen method
        if (args.method == Method::Exact) {
            cout << "Search Method: Exact Search" << endl;
        } else if (args.method == Method::LSH) {
            cout << "Search Method: LSH (L=" << args.L << ", k=" << args.k << ", w=" << args.w << ")" << endl;
        } else {
            cout << "Search Method: Other (Hypercube/IVF)" << endl;
        }
        cout << "--------------------------------" << endl;

        // TODO: The Main Pipeline
        // 2. Load Datasets using `load_split` and `load_dataset`
        // 3. Extract SIFT for Training Set
        // 4. Run K-Means to build the Vocabulary
        // 5. Compute BoVW histograms for all images (Database + Queries)
        // 6. Initialize the appropriate SearchMethod based on `args.method`
        // 7. Loop through queries, call `search()`, and evaluate results!

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
