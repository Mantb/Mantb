#include "args.hpp"
#include "loader.hpp"
#include "imgutils/split.hpp"
#include "imgutils/pipeline.hpp"
#include "imgutils/search.hpp"
#include "imgutils/split.hpp"
#include <iostream>
#define DATASETPATH "dataset/MIRFLICKR-1M"
#define SPLITPATH "dataset/splits.txt"#include <filesystem>

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

        // 2. Load Datasets using `load_split` and `load_dataset` (from loader.hpp/split.hpp)
        Split split = load_split(args.split);

        vector<Image> train_images;
        for (const auto& seq : split.train) {
            vector<Image> seq_images = load_dataset(args.hp, seq);
            train_images.insert(train_images.end(), seq_images.begin(), seq_images.end());
        }

        vector<Image> database_images;
        vector<Image> query_images;
        vector<vector<string>> ground_truth;
        vector<string> database_image_ids;

        const vector<string>& eval_set = (args.set == "test") ? split.test : split.validation;

        for (const auto& seq : eval_set) {
            vector<Image> seq_images = load_dataset(args.hp, seq);
            if (!seq_images.empty()) {
                query_images.push_back(seq_images[0]);
                vector<string> gt_ids;
                for (size_t i = 1; i < seq_images.size(); ++i) {
                    database_images.push_back(seq_images[i]);
                    database_image_ids.push_back(seq_images[i].id);
                    gt_ids.push_back(seq_images[i].id);
                }
                ground_truth.push_back(gt_ids);
            }
        }

        // Load Distractors (MIRFlickr)
        int distractor_count = 0;
        if (!args.mir.empty() && args.D > 0 && std::filesystem::exists(args.mir)) {
            for (const auto& entry : std::filesystem::directory_iterator(args.mir)) {
                if (entry.is_regular_file() && distractor_count < args.D) {
                    string path = entry.path().string();
                    string id = entry.path().filename().string();
                    database_images.push_back({id, path});
                    database_image_ids.push_back(id);
                    distractor_count++;
                }
            }
        }
        cout << "Loaded " << train_images.size() << " train images, "
             << database_images.size() << " database images (incl. " << distractor_count << " distractors), and "
             << query_images.size() << " queries." << endl;

        // 3. Build Vocabulary
        vector<vector<float>> vocab = build_vocabulary(train_images, args.vocab, args.S);

        // 4. Compute BoVW
        cout << "Computing BoVW for Database..." << endl;
        vector<vector<float>> database_bovw = compute_dataset_bovw(database_images, vocab, 500);
        cout << "Computing BoVW for Queries..." << endl;
        vector<vector<float>> query_bovw = compute_dataset_bovw(query_images, vocab, 10000); // queries shouldn't be capped!

        // 5. Initialize Search Method
        SearchMethod* searcher = initialize_search_method(args);

        // 6. Build the Index (LSH, Hypercube, IVF, or Exact)
        cout << "Building Index..." << endl;
        searcher->build_index(database_bovw, database_image_ids);

        // 7. Evaluate
        cout << "Evaluating Queries..." << endl;
        evaluate_queries(searcher, query_images, query_bovw, ground_truth, args.output);

        delete searcher;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
