#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <unordered_set>
#include <iomanip>



using namespace std;

/* a user_defined data type for all different itemsets of size k with their counter */
typedef struct lk{
	map<set<int>,int> counter; /* the count of an itemset */
	int k; /* the size of the itemset */
}lk;

// the global variables:
double min_support_count; /* min count for an itemset to be included in the frequent itemset */
int num_items=0;
int num_transactions=0;
int max_itemset_size=0;
double min_support;
double min_confidence;
int number_of_strong_rules=0;
vector<set<int>> transactions;
clock_t t1,t2;

// input argument variables
string program_name; // the main file
string data_file; //the dataset 
string display_option;


/* a vector of lks containing all frequent itemsets of different sizes */
vector<lk> all_lk;


// header of the functions:
// functions are included in "utility.cpp" file
void create_c1(); // to read to dataset
void generate_candidates(lk &new_c, lk &previous_l);
int select_counting_algorithm(int d, int k);
void subset_finder(set<int> &sset, int size, int left_size, set<int>::iterator index, lk &ck, set<int> &temp_i);
void keep_frequent_candidates(lk &c);
void display();
void display_frequent_itemsets();
void find_strong_rules();
void find_strong_rules_subset(set<int> &sset, int count_i, int size, int left_size, set<int>::iterator index, set<int> &v, ofstream &fh);
void display_number_of_association_rules();
void display_number_of_frequent_itemset();
void display_run_time();
string double_to_string_with_precision(double value);


#include "utility.cpp"

int main(int argc, char **argv){

	// to get the keyword arguments from the command line.
    if(argc==4){
		
        program_name = argv[0];
        data_file = argv[1];
        min_support = atof(argv[2]);
        min_confidence = atof(argv[3]);
		display_option = "absent";
    }

    else if(argc==5){

        program_name = argv[0];
        data_file = argv[1];
        min_support = atof(argv[2]);
        min_confidence = atof(argv[3]);
        display_option = argv[4];
    }

	// cout << "min_support: " << min_support << endl;
	// cout << "min_confidence: " << min_confidence << endl;
	// cout << "display_option: " << display_option << endl;

    t1=clock();
    
    create_c1();
    lk ck;
    lk previous_l=all_lk[0];

    while(previous_l.counter.size()!=0){
    	generate_candidates(ck,previous_l); //void generate_candidates(lk &new_c, lk &previous_l)
    	keep_frequent_candidates(ck);
    	previous_l=all_lk.back();	
    }
	all_lk.pop_back();
	
	display();

    t2=clock();
	
	display_run_time();

    return 0;
}
