#include "args.hpp"
#include "loader.hpp"
#include "imgutils/pipeline.hpp"
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
        cout << "Search Method Enum: " << static_cast<int>(args.method) << endl;
        cout << "--------------------------------" << endl;

        // TODO: Tie the Pipeline Together!

        // 2. Load Datasets using `load_split` and `load_dataset` (from loader.hpp/split.hpp)

        // 3. Build Vocabulary
        // vector<vector<float>> vocab = build_vocabulary(train_images, args.vocab, args.S);

        // 4. Compute BoVW
        // vector<vector<float>> database_bovw = compute_dataset_bovw(database_images, vocab, 500);
        // vector<vector<float>> query_bovw = compute_dataset_bovw(query_images, vocab, 10000); // queries shouldn't be capped!

        // 5. Initialize Search Method
        // SearchMethod* searcher = initialize_search_method(args);

        // 6. Build the Index (LSH, Hypercube, IVF, or Exact)
        // searcher->build_index(database_bovw, database_image_ids);

        // 7. Evaluate
        // evaluate_queries(searcher, query_images, query_bovw, ground_truth);

        // delete searcher;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
