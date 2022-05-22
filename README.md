# Apriori implementation in c++

This repo contains the c++ implementation of apriori algorithm for association rule mining in transactional databases (`dataT10K500D12L.data.txt`).

# Code summary

The code is consisted of 2 modules: `main.cpp` and `utility.cpp`. The functions that are used in `main.cpp` are defined in `utility.cpp`.

# To run the implementation

- Keep the executable file and the dataset in the same directory


- To compile: `c++ ./main.cpp -o main`


- To Run: `<program_name> <data_file> <min_support> <min_confidence> <display_option> `  
Ex: `./main dataT10K500D12L.data.txt 0.001 0.8 a`

# Algorithm summary
Apriori consists of two stages: frequent itemset generation and strong rule generation.

## Frequent itemset generation
- The `create_c1` function creates candidate itemset of size 1 based on the items in the transactions.
- Every time, the `generate_candidates` function creates candidates of size k+1 from frequent itemsets of size k.
Based on the Apriori principle, there is a pruning step. The powerful and scalable pruning technique is that if a subset of an itemset is infrequent, then the itemset cannot be frequent; consequently, it shouldn’t be added to the candidates.
- For every candidate itemset, the `keep_frequent_candidates` function checks the support of the itemset. If the support of the itemset is higher than the user-defined min_support threshold, then itemset is added to frequent itemsets.

## Strong rule generation
- The `find_strong_rules` function generates all the non-empty subsets from thefrequent itemsets. If the confidence of the rule is higher than the user-defined min_confidence threshold, the rule is considered strong.


# Experiment
## Setup

I used my laptop to run the code with following specification:


Processor: 2.3 GHz Intel Core i7  
Memory: 4 GigaBytes  
Hard disk: 1 TeraBytes HDD  
Operating system: Linux Ubuntu  

## Results
#### For 10000 transactions:
***Dataset***: `T10K500D12L.data.txt`  
min_support = 0.001  
min_confidence = 0.8  
**To run**: `./main dataT10K500D12L.data.txt 0.001 0.8`  
Number of frequent 1_itemsets: 472  
Number of frequent 2_itemsets: 24085  
Number of frequent 3_itemsets: 8049  
Number of frequent 4_itemsets: 3786  
Number of frequent 5_itemsets: 2329  
Number of frequent 6_itemsets: 1115  
Number of frequent 7_itemsets: 407  
Number of frequent 8_itemsets: 111  
Number of frequent 9_itemsets: 21  
Number of frequent 10_itemsets: 2  
Number of association rules: 161856  
**Run time**: 277.387 seconds  


#### For 1000 transactions:
***Dataset***: `dataT10K500D12L.data_1000.txt` (the first top 1000 transactions)  
min_support = 0.01  
min_confidence = 0.8  
**To run**: `./main dataT10K500D12L.data_1000.txt 0.01 0.8`  
Number of frequent 1_itemsets: 334  
Number of frequent 2_itemsets: 174  
Number of association rules: 0  
**Run time**: 1.695 seconds    

#### For 100 transactions:
***Dataset***: `dataT10K500D12L.data_100.txt` (the first top 100 transactions)  
min_support = 0.1  
min_confidence = 0.8  
**To run**: `./main dataT10K500D12L.data_100.txt 0.1 0.8`  
Number of frequent 1_itemsets: 10  
Number of association rules: 0  
**Run time**: 0.014 seconds  
