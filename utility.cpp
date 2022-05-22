// This file includes all the functions used in main file

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

void display_run_time(){
	double seconds = ((double)t2-(double)t1) / CLOCKS_PER_SEC;
	cout << endl << "-------------------------------------------" << endl;
    cout<<"Run time: " << double_to_string_with_precision(seconds) <<  " seconds" <<endl;;

}

/*
	- it Reads the transactions from data_file
	- finds the number of transactions (num_transactions)
	- gets maximum number of items in one transaction (max_itemset_size)
	- creates candidate items of size 1 with the counter (ck)
	- create frequent itemsets of size 1 (l1)
*/
void create_c1(){

	/* fh: file handler to read the data_file */
    ifstream fh(data_file);
    string line;

    int n; /* each item of a transaction */
    int m; /* number of items in one transaction */

    /* ck is a of user-defined data type (lk) for candidates of size k+1 */
    lk ck;
    ck.k=1;

	// reading the data_file line by line
    while (getline(fh,line)){
        istringstream iss(line);

        set<int> transaction;
        
        set<int> n_set; 

        m=0;
        while( iss >> n ){

            m +=1;
            
            n_set.insert(n);

			transaction.insert(n);

            if(ck.counter.count(n_set)>0){
            	
                ck.counter[n_set] +=1;
            }
            else{
                ck.counter.insert(pair<set<int>,int>(n_set,1));
                num_items +=1;
            }

            n_set.erase(n);
        }

        // to find max_itemset_size.
        if (m > max_itemset_size){
            max_itemset_size = m;
        }

        num_transactions +=1;
		
		transactions.push_back(transaction);

    }

    min_support_count = min_support * num_transactions; 

	// cout << "num_transactions: " << num_transactions << endl;
	// cout << "min_support_count: " << min_support_count << endl;

    lk l1;
    l1.k=1;

	set<int> set_i;
    int count_i;
    for (auto iter = ck.counter.begin(); iter != ck.counter.end(); ++iter){
        set_i = iter->first;
        count_i = iter->second;
        if (count_i >= min_support_count){
            l1.counter.insert({set_i,count_i});
        }
    }

    ck.counter.clear();
    all_lk.push_back(l1);
    fh.close();

    return;
}


/* This function generates new candidate itemsets of size k+1 from  frequent itemsets of size k. */
void generate_candidates(lk &new_c, lk &previous_l){

    new_c.k = previous_l.k +1; 
    
    int num;
    int item;
    bool to_generate;

	set<int> temp;
	set<int> temp_i;
	set<int> temp_ii;
	set<int> items_all; /* it contains a set of all  items of previous_l */
	
	for (auto iter = previous_l.counter.begin(); iter!= previous_l.counter.end(); iter++){
        temp_i = iter->first; 
	    items_all.insert(temp_i.begin(),temp_i.end());
    }

	for (auto iter = previous_l.counter.begin(); iter != previous_l.counter.end(); ++iter++){

        temp_i = iter->first;

        for (auto iter_i =items_all.begin(); iter_i!=items_all.end(); iter_i++){
            item = *iter_i; // dereference the pointer to elements

			// it check if new_c has the newly generated candidate (temp_i) or not. It is added to the new_c if new_c doesn't have the temp_i.
            if(temp_i.count(item)==0){ 

                temp_i.insert(item);
                
                if(new_c.counter.count(temp_i)==0){ 

					temp_ii = temp_i; /* subset of an frequent itemset for pruning step */
                    to_generate=true;

					// pruning step: a new candidate itemset is not generated if a subset is infrequent.   
                    for (auto iter=temp_i.begin(); iter!= temp_i.end(); iter++){  

	                    temp_ii.erase(*iter);

	                    if(previous_l.counter.count(temp_ii)==0){

	                        to_generate=false;
	                        temp_ii.insert(*iter);
	                            
	                        break;
	                    }

	                    temp_ii.insert(*iter);
	                }
                	
					// to add a newly generated candidate (temp_i) to the candidate (new_c).
	                if(to_generate){
	                    new_c.counter.insert({temp_i,0});
	                }
                	
                }

                temp_i.erase(item);
            }
        }
	}
	
	items_all.clear();
	temp.clear();
	temp_i.clear();
	
	return;
}


/*
	it is used to select which algorithm should be used for counting transactions for each itemset
*/
int select_counting_algorithm(int d, int k){ // 

	if(k==1){
		return d;
	}
	if(k==d){
		return 1;
	}

	if(k > d){
		return 0;
	}
	return select_counting_algorithm(d-1,k) + select_counting_algorithm(d-1,k-1);
}

/*
Go to each tracsaction and find subsets of size k and increment their counter in c. This algorithm is implemented in function subset().
To form a subset of one transaction, the function starts from index, goes forward by left_size (initially k). then it checks if any candidate matches the subset. 
Every time, it tries to find if anyone of our candidates exists in one transaction. if it exists (c.counter.count(v)>0), then increase the counter of that candidate (c.counter[v] +=1)
You try this function on all the transactions one by one, at the end you know how many times the candidate exists in the transactions.
*/
void subset_finder(set<int> &sset, int size, int left_size, set<int>::iterator index, lk &ck, set<int> &temp_i){
    if(left_size==0){
        if(ck.counter.count(temp_i)>0){
            ck.counter[temp_i] +=1;
        }
        return;
    }

    for(set<int>::iterator it=index; it!=sset.end(); ++it){
        temp_i.insert(*it);
        subset_finder(sset,size,left_size-1,++index, ck, temp_i);
        temp_i.erase(*it);
    }
    return;}


/*
	The function only keeps the candidates which the count is higher than min_support_count.
	It creates frequent itemset (lk+1) from the candidates (ck+1).
*/
void keep_frequent_candidates(lk &ck){

	int temp_count;
    bool to_count;

	set<int> candidate_temp_set;
    set<int> transaction_i;
    set<int> temp_i;
     
	for (int iter=0; iter<transactions.size(); iter++){ // vector<set<int>> transactions;
	transaction_i=transactions[iter];
	
	if(transaction_i.size() >= ck.k){
		if(ck.counter.size() < select_counting_algorithm(transaction_i.size(),ck.k)){

			
			for (auto iteri = ck.counter.begin(); iteri!= ck.counter.end(); iteri++){
				candidate_temp_set=iteri->first; 
				to_count=true;

				
				for (auto iterii = candidate_temp_set.begin(); iterii != candidate_temp_set.end(); iterii++){
					if(transaction_i.count(*iterii)==0){
						to_count=false;
						break; // break if the candidate never appear in the transaction.
					}
				}

				if(to_count){
					ck.counter[candidate_temp_set] +=1;
				}
			}
			
		}
		else{
			subset_finder(transaction_i, transaction_i.size(), ck.k, transaction_i.begin(), ck, temp_i);
		}
	}
	}

	lk l_new;
    l_new.k = ck.k;

    int count_i;
    for (auto iter = ck.counter.begin(); iter != ck.counter.end(); iter++){
        temp_i=iter->first;
        count_i=iter->second;
        if (count_i >= min_support_count){
            l_new.counter.insert({temp_i,count_i});
        }
    }

    ck.counter.clear(); //it is clear and ready for the next candidate generaion step
    all_lk.push_back(l_new);

    return;
}


/*
	it displays the frequent itemsets or strong rules depending on display_option
*/
void display(){

	if(display_option == "f"){
		// "f": then all frequent itemsets are displayed

		cout << endl << "-------------------------------------------" << endl;
		cout << "frequent itemsets:"<< endl;
		display_frequent_itemsets();
	}

	if(display_option == "r"){
		// "r": all strong association rules are displayed

		cout << endl << "-------------------------------------------" << endl;
		cout << "strong rules: (s,c)" << endl;
		find_strong_rules();
	}

	if(display_option == "a"){
		// "a": all frequent itemsets and all strong association rules are displayed

		cout << "-------------------------------------------" << endl;
		cout << "frequent itemsets:" << endl;
		display_frequent_itemsets();

		cout << endl << "-------------------------------------------" << endl;
		cout << "strong rules: (s,c)" << endl;
		find_strong_rules();
	}

	if(display_option == "absent"){
		// absent: only the number of frequent itemsets of different sizes and the number of strong rules are displayed.

		cout << endl << "-------------------------------------------" << endl;
		cout << "Number of frequent itemsets: " << endl;
		display_number_of_frequent_itemset();

		cout << endl << "-------------------------------------------" << endl;
		find_strong_rules();
		display_number_of_association_rules();
	}

	return;}


void display_number_of_association_rules(){

	string text;
	text="Number of association rules: " + to_string(number_of_strong_rules);
	cout<<text<<endl;
	cout<<endl;

	return;
}

void display_number_of_frequent_itemset(){

	// to print no of frequent itemsets 
	string text;
	lk l_temp;
	
	
	for(int i=0;i<all_lk.size();i++){
		l_temp=all_lk[i];
		text="Number of frequent " + to_string(i+1) + "_itemsets: " + to_string(l_temp.counter.size());
		cout<<text<<endl;
	}

	return;
}

void display_frequent_itemsets(){

	int count;
	string text;
	string textD; /* text that is displayed */
	set<int> temp;
	lk l_temp;
	ofstream outfile;

	// i: size of itemset
	for(int i=0;i<all_lk.size();i++){ // vector<lk> all_lk;
		l_temp=all_lk[i];

		textD = "\nsize k = " + to_string(i+1) + "\nitemset (count)";
		cout << textD << endl;
		
		// to iterate through the itemsets of size k = i.
    	for (auto it = l_temp.counter.begin(); it != l_temp.counter.end(); ++it){

			string text_temp;

		    temp=it->first; /* one set of frequent items */
		    count=it->second; /* count of that frequent itemset */

		    for(auto itt=temp.begin(); itt!=temp.end();){

				text_temp += (to_string(*itt));

		       	if(++itt!=temp.end()){
		        	text_temp += ", ";
		        }
		        else{
		        	text_temp += " ";
		        }
		    }
		    
			text_temp += ("(" + to_string(count) + ")");
			cout << text_temp << endl;

		}
	}
}


/*
	This function finds strong rules based on frequent itemsets of size k.
	It first generates all the subsets out of the frequent itemsets.
	If the confidence of the rule is higher than min_confidence, the rule is added to the association_rules.txt and it is displayed.
*/
void find_strong_rules(){

	set<int> itemset_i; /* the ith frequent itemset */
	int count_i; /* count of ith frequent itemset */
	lk l_temp; /* all itemsets of size k */
	ofstream fh; /* file handler to create and write the "association_rules.txt" */

	fh.open("association_rules.txt");

	set<int> subset;

	for(int i=1; i<all_lk.size(); i++){
		l_temp = all_lk[i];
    	for (auto iter = l_temp.counter.begin(); iter != l_temp.counter.end(); ++iter){
		    itemset_i = iter->first;
		    count_i = iter->second;
		    for(int subset_size=1; subset_size<itemset_i.size();subset_size++){ // to produce subsets of different sizes out of one frequent itemset
		    	
		    	find_strong_rules_subset(itemset_i, count_i, itemset_i.size(), subset_size, itemset_i.begin(), subset, fh);
		    }
		}	
	}

	fh.close();
}


/*
	This function finds strong rules based on one itemset of size k.
	It first generates all the subsets out of that itemset for rule generation, i.e, output rule: subset -> (itemset-subset).
	If the confidence of the rule is higher than min_confidence, the rule is added to the association_rules.txt and it is displayed.
*/
void find_strong_rules_subset(set<int> &itemset, int itemset_count, int size, int left_size, set<int>::iterator index, set<int> &subset, ofstream &fh){
    
	if(left_size==0){
    	
        double count_subset = all_lk[subset.size()-1].counter[subset];
        double confidence_ = (double)itemset_count/count_subset;
		double support_ = (double)itemset_count/num_transactions;
        
        string text = "";
        if(confidence_ >= min_confidence){
        	vector<int> temp_set(itemset.size());
        	vector<int>::iterator it=set_difference(itemset.begin(),itemset.end(),subset.begin(),subset.end(),temp_set.begin());
        	temp_set.resize(it-temp_set.begin());


			for(auto itt=subset.begin(); itt!=subset.end();){
				text += to_string(*itt);
				if(++itt!=subset.end()){
					text += ", ";
				}
				else{
					text += " ";
				}
			}
			text += "-> ";

			for(auto itt=temp_set.begin(); itt!=temp_set.end();){
				text += to_string(*itt);
				if(++itt!=temp_set.end()){
					text += ", ";
				}
				else{
					text += " ";
				}
			}
			
			text += "(" + double_to_string_with_precision(support_) + ", " + double_to_string_with_precision(confidence_) + ")";

			if(display_option == "r" || display_option == "a"){
				cout << text << endl;
			}

			fh << text << endl;

		    number_of_strong_rules +=1;
        }
    	
        return;
    }


    for(set<int>::iterator it=index; it!=itemset.end(); ++it){
        subset.insert(*it);
        find_strong_rules_subset(itemset, itemset_count, size,left_size-1,++index,subset,fh);
        subset.erase(*it);
    }
	
    return;

}

string double_to_string_with_precision(double value){
    ostringstream out;
    out << fixed << setprecision(3) << value;
    return out.str();

}