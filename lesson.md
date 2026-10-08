Similarity Search & Distances: The course covers Exact and Approximate Nearest Neighbors (ANN). Distances are measured using metrics like Euclidean (l 
2
​	
 ) and Manhattan (l 
1
​	
 ). To overcome the "Curse of Dimensionality" in high dimensions, ANN methods like LSH and Randomized Projections are used.   
PDF
+3
Locality Sensitive Hashing (LSH): LSH projects data so that nearby points hash to the same bucket with high probability. For Euclidean spaces, points are projected onto a random line defined by a normal distribution vector v∼N(0,1) 
d
 , shifted by a random t, and divided into windows of size w. Amplification is achieved by combining k functions (e.g., k=4 to 6) and using L hash tables.   
PDF
+4
Randomized Projections (Hypercube): Points are mapped to the vertices of a hypercube {0,1} 
d 
′
 
 . The search checks the query's vertex and expands to nearby vertices based on Hamming distance until a threshold of points or vertices is reached.   
PDF
+2
Clustering (k-means): The objective is to partition n objects into k clusters to minimize the distance to cluster centroids. The standard approach is Lloyd's algorithm: Initialize k centers, assign each point to the nearest center (Expectation), and update centers to the mean of the assigned points (Maximization). K-means++ initialization is recommended to spread out initial centroids, improving convergence.   
PDF
+3
Inverted File Index (IVF): Partitions the dataset into k clusters. During a query, it finds the b (or nprobes) nearest centroids and exhaustively searches only within those clusters.   
PDF
+1
IVF with Product Quantization (IVFPQ): Refines IVF by computing the residual (displacement) of a vector from its centroid, splitting this residual into M sub-vectors, and quantizing each part using sub-codebooks. Querying uses an Asymmetric Distance Computation (ADC) via a Look-Up Table (LUT) to approximate distances rapidly.   
PDF
+1
Image Processing (SIFT & BoVW): SIFT extracts local features invariant to scale and rotation, describing each keypoint with a 128-dimensional vector. Images are converted to a "Bag of Visual Words" by clustering SIFT descriptors from a training set into k centers (visual vocabulary). Each image is then represented as a normalized histogram of length k, counting how many of its descriptors fall into each visual word.