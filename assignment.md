Assignment Objective: Develop a C++ application to search for images using vector representations. Images are processed into 128-dimensional SIFT descriptors and clustered into K visual words using k-means. Each image is represented as a normalized frequency vector x 
I
​	
 =( 
n 
I
​	
 
h 
1
​	
 
​	
 ,..., 
n 
I
​	
 
h 
K
​	
 
​	
 ).   
PDF
+2
Search Methods: Implement and compare Exact Search, LSH, Hypercube, IVFFlat, and IVFPQ using the Euclidean metric.   
PDF
Datasets:
HPatches: 116 sequences of 6 images (1 query, 5 relevant) divided into illumination and viewpoint changes. Split into Training (70), Validation (23), and Test (23).   
PDF
+1
MIRFlickr-25K: Used purely as distractors to scale the database size D∈{1000,5000,10000,25000}.   
PDF
Implementation Rules:
Use OpenCV strictly for reading images and extracting SIFT descriptors.   
PDF
+1
K-means training samples a maximum of S (default 500) descriptors per training image.   
PDF
Queries return the top 10 ranked images.   
PDF
CLI Parameters: Must support specific arguments like -hp, -mir, -vocab (K centers), -S, -D, and method-specific flags (e.g., -lsh -k 4 -L 5 -w 0.05, -ivfpq -kclusters 100 -M 16 -nbits 8).   
PDF
Evaluation Metrics: Output must include Recall@5, Recall@10, Average Precision (AP@10), Mean AP (mAP@10), and average query time in milliseconds. Recall measures the fraction of the 5 relevant images found, while AP accounts for their rank position.