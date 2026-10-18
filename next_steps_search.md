# Next Steps: Search and Evaluation

Now that you have the ability to extract Bag of Visual Words (BoVW) vectors representing your images, the next critical step is to find the nearest neighbors for a query image and evaluate the quality of those results.

## 1. Exact Search (Brute Force)

Before moving to complex indexing like LSH or IVF, we must establish a baseline using Exact Search. This is an exhaustive $O(N)$ operation where we compare the query to every single image in the database.

### Mathematical Logic

Given a query vector $q \in \mathbb{R}^K$ and a database of $N$ image vectors $X = \{x_1, x_2, \dots, x_N\}$, the objective is to find the set of $k$ vectors in $X$ that minimize the distance to $q$.

The assignment specifies the **Euclidean metric**:
$$ d(q, x_i) = \sqrt{\sum_{j=1}^{K} (q_j - x_{i,j})^2} $$

*Optimization Tip:* Because the square root function is monotonic, sorting by $d(q, x_i)$ is mathematically equivalent to sorting by $d(q, x_i)^2$. To save computational cycles, you can omit the `std::sqrt` during the distance calculation and simply sort based on the squared distances.

### Implementation Steps (`src/search.cpp`)
1. **`build_index`**: For Exact Search, "building the index" is trivial. Simply copy (or store references/pointers to) the `dataset` vectors and `image_ids` into the class member variables.
2. **`search`**:
   - Initialize a container (like `std::vector<SearchResult>`) to hold pairs of (distance, image_id).
   - Loop through every vector in `dataset_`.
   - Compute the squared Euclidean distance between `query` and the current dataset vector.
   - Store the result.
   - Once the loop is complete, sort the results in ascending order based on distance.
   - Return a sub-vector (or slice) containing the first `top_k` results. (The assignment specifies queries return the top 10 ranked images).

## 2. Evaluation Metrics

Once we retrieve the top $K$ results for a query sequence (in the HPatches dataset, a sequence has 1 query image and 5 relevant images), we need to measure how good our search was.

Let:
* $R_{ret}$ be the sequence of retrieved image IDs, ordered by rank.
* $G$ be the set of ground-truth relevant image IDs for the given query.

### Recall@K
Recall measures the fraction of the relevant images that were successfully retrieved within the top $K$ results.

$$ \text{Recall}@K = \frac{|R_{ret} \cap G|}{|G|} $$

Since each HPatches sequence has 5 relevant images, $|G| = 5$. If your top 10 results contain 3 of the 5 relevant images, your Recall@10 is $3/5 = 0.6$.

**Implementation (`calculate_recall`):**
Simply count how many of the `image_id`s in your `results` vector exist in the `ground_truth` vector, and divide by the size of `ground_truth`.

### Average Precision (AP@K)
Recall doesn't care *where* in the top 10 the relevant images appear. Finding a relevant image at rank 1 should be rewarded more than finding it at rank 10. Average Precision accounts for this.

Precision at rank $i$ (denoted $P@i$) is the proportion of retrieved images up to rank $i$ that are relevant.

$$ \text{AP}@K = \frac{1}{|G|} \sum_{i=1}^{K} P@i \cdot \text{rel}(i) $$
Where $\text{rel}(i)$ is an indicator function equal to $1$ if the item at rank $i$ is relevant, and $0$ otherwise.

**Implementation (`calculate_ap`):**
1. Initialize `relevant_found_so_far = 0` and `sum_precisions = 0.0`.
2. Loop through your `results` vector with index `i` (representing rank, where rank starts at 1, so rank is `i + 1`).
3. If `results[i].image_id` exists in `ground_truth`:
   - Increment `relevant_found_so_far`.
   - Calculate precision at this rank: `precision = relevant_found_so_far / (i + 1.0)`.
   - Add `precision` to `sum_precisions`.
4. Return `sum_precisions / ground_truth.size()`.

---

## Action Plan
I have created `include/imgutils/search.hpp` and `src/search.cpp` with an object-oriented skeleton for the search methods and the evaluation functions.
Your next task is to fill out the implementations marked with `TODO` in `src/search.cpp`!
