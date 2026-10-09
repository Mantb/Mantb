# Part 1: Project Overview and Architecture

## The Big Picture
Your objective is to build an **Image Search Engine**. Given a query image, the system must find the most similar images from a large database. Instead of comparing pixels directly (which is slow and inaccurate), we transform images into high-dimensional vectors and perform **Nearest Neighbor Search**.

### The Pipeline
1. **Feature Extraction:** Read images and extract SIFT descriptors (128-dimensional vectors).
2. **Visual Vocabulary (Training Phase):** Take a subset of SIFT descriptors from the training dataset. Run **K-Means clustering** to group them into $K$ clusters. The centers of these clusters are our "Visual Words".
3. **Bag of Visual Words (BoVW):** For every image in the dataset, represent it as a histogram (a $K$-dimensional vector) counting how many of its SIFT descriptors fall into each "Visual Word". Normalize this vector.
4. **Indexing & Searching:** Store all these $K$-dimensional vectors. Given a query image (converted to the same $K$-dimensional format), find the closest vectors in the database using Exact Search, LSH, Hypercube, or IVF variants.
5. **Evaluation:** Output the top 10 results. Evaluate performance using Recall and Mean Average Precision (mAP).

## Software Architecture
Your code should follow strict Object-Oriented principles and RAII.
* **`include/config.hpp`**: Define structs to hold CLI parameters (e.g., `-D`, `-vocab`, `-k`). Write a parsing function here.
* **Encapsulation:** Keep data members private. Expose only what's necessary through public methods.
* **Memory Management:** Use `std::vector`, `std::unique_ptr`, or `std::shared_ptr`. Avoid raw pointers (`new`/`delete`).

## Evaluation Metrics (Math & Logic)
For each query, the ground truth defines exactly 5 "relevant" images (from HPatches). You return 10 results.
* **Recall@X:** Out of the 5 relevant images, how many were in your top $X$ results?
  * If your top 5 results contain 3 relevant images, Recall@5 = 3/5 = 0.6.
* **Average Precision (AP@10):** Measures ranking quality. If you find relevant images at ranks 1, 3, and 6, your AP is the average of the precisions at those ranks.
* **mAP@10:** The mean of the AP@10 across all queries.
