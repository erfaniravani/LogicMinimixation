#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>



using namespace std;


vector<string> get_lines_of_file(vector<string> lines , string file_name);
vector<string> string_to_word(string str);
vector<string> string_to_word_2(string str);
vector<string> seperate_variabels(string str);
vector<string> seq_inputs(string first_line , vector<string> input_list);
vector<string> seq_outputs(string first_line , vector<string> output_list);
vector<vector<string> > generate_cube_0(string assign_lines , vector<string> inputs , vector<string> outputs , vector<string> wires , string& element_orders , vector<string>& assign_orders);
bool compare(vector<string> str1 , vector<string> str2);
vector<string> combine(vector<string> str1 , vector<string> str2);
void generate_cubes(vector<vector<string> >& cube_0 , vector<vector<string> >& cube_next , vector<vector<string> >& cube_left);
void generate_cubes2(vector<vector<string> >& cube_0 , vector<vector<string> >& cube_next , vector<vector<string> >& cube_left);
vector<vector<int> > generate_proper_groups(vector<vector<int> > can_make , vector<vector<string> > cube_0 , vector<vector<string> > cube_left);
bool is_proper_group(vector<string> cube_0 , vector<string> cube_left);
vector<vector<string> > unique_vector(vector<vector<string> > cube);
void delete_rows(vector<vector<int> >& can_make , int index);
vector<int> add_essentials(vector<int> final_indexes , vector<vector<int> >& can_make);
void number_counts(vector<vector<int> >& can_make , vector<int>& quantity , int index);
vector<int> add_others(vector<int> final_indexes , vector<vector<int> >& can_make , int size);
bool minimization_done(vector<vector<int> > can_make);
void do_minimization(vector<int>& final_indexes , vector<vector<int> >& can_make);
vector<vector<string> > generate_final_expressions(vector<vector<string> > final_expressions , vector<int>& final_indexes , vector<vector<string> > cube_left);
void generate_primes(vector<vector<string> >& cube_0 , vector<vector<string> >& cube_next , vector<vector<string> >& cube_left , vector<string> inputs ,
                     vector<string> outputs , vector<string> wires , string assign_lines , string& element_orders , vector<string>& assign_orders);
int get_index(string str , vector<string> names);
vector<string> figure_assign_outputs(vector<string> assign_lines);

void do_QM(vector<string> assign_lines , vector<string> words , vector<string> inputs , vector<string> outputs , vector<string> wires , 
		   vector<vector<vector<string> > >& all_minimized , vector<vector<string> >& all_seqenced_assigns);





/*
vector<wire> figure_inputs_wires(vector<string> inputs);
vector<wire> figure_outputs_wires(vector<string> outputs);
vector<wire> figure_wires_wires(vector<string> wires);
vector<wire> initial_inputs(vector<wire> sequenced_inputs_wire , string tb_line);
vector<string> find_assign_input_values(vector<string> seq_assign , vector<wire*> all_wires_address);
int where_in_vector_pointer(string wire_name , vector<wire*> wire_vect_adr);
void simulate_assign(string assign_output , vector<string> assign_input_values , vector<vector<string> > final_expressions , vector<wire*> all_wires_address);
bool check_same(vector<string> expression , vector<string> assign_input_values);
*/
int main(){
	vector<string> file_lines;
	file_lines = get_lines_of_file(file_lines , "ooTest1.txt");
	vector<string> assign_lines;
	vector<string> mux_lines;
	vector<string> words;
	vector<string> inputs;
	vector<string> ins;
	vector<string> outputs;
	vector<string> outs;
	vector<string> wires;
	vector<string> assign_outputs;

	for(int i = 0 ; i < file_lines.size() ; i++){
		words = string_to_word(file_lines[i]);
		if(words[0] == "assign"){
			if(find(words.begin(), words.end(), "?") != words.end())
				mux_lines.push_back(file_lines[i]);
			else
				assign_lines.push_back(file_lines[i]);
		}
		if(words[0] == "wire"){
			wires.push_back(words[1]);
		}
		if(words[0] == "output"){
			outs.push_back(words[1]);
		}
		if(words[0] == "input"){
			ins.push_back(words[1]);
		}
	}

	inputs = seq_inputs(file_lines[0] , ins);
	outputs = seq_outputs(file_lines[0] , outs);
	assign_outputs = figure_assign_outputs(assign_lines);

	vector<vector<vector<string> > > all_minimized;
	vector<vector<string> > all_seqenced_assigns;

	do_QM(assign_lines , words , inputs , outputs , wires , all_minimized , all_seqenced_assigns);

	//generate verilog file after minimization

	ofstream new_file("ca3verilog.txt");
	new_file << "module main(";
	for(int i = 0 ; i < inputs.size() ; i++){
		if(i == inputs.size() - 1){
			new_file << inputs[i] << ",";
			for(int j = 0 ; j < outputs.size() ; j++){
				if(j == outputs.size() - 1)
					new_file << outputs[j] << ");";
				else
					new_file << outputs[j];
			}
		}
		else
			new_file << inputs[i] << ",";
	}
	new_file << endl;
	for(int i = 0 ; i < inputs.size() ; i++){
		new_file << "input " << inputs[i] << ";" << endl;
	}
	for(int i = 0 ; i < outputs.size() ; i++){
		new_file << "output " << outputs[i] << ";" << endl;
	}
	for(int i = 0 ; i < wires.size() ; i++){
		new_file << "wire " << wires[i] << ";" << endl;
	}

	for(int i = 0 ; i < assign_lines.size() ; i++){
		words.clear();
		words = string_to_word(assign_lines[i]);
		new_file << "assign #" << words[1] << " " << words[2] << " = ";
		for(int j = 0 ; j < all_minimized[i].size() ; j++){
			//find last x if there is any
			int index_of_last_x;
			for(int u = all_minimized[i][j].size()-1 ; u > -1 ; u--){
				if(all_minimized[i][j][u] == "1" or all_minimized[i][j][u] == "0"){
					index_of_last_x = u;
					break;
				}
			}
			//////////////////
			for(int k = 0 ; k < all_minimized[i][j].size() ; k++){

				if(all_minimized[i][j][k] == "1"){
					if(k == all_minimized[i][j].size() - 1){						
						new_file << all_seqenced_assigns[i][k];
					}
					else{
						if(k == index_of_last_x)
							new_file << all_seqenced_assigns[i][k];
						else
							new_file << all_seqenced_assigns[i][k] << "&";
					}
				}
				else if(all_minimized[i][j][k] == "0"){
					if(k == all_minimized[i][j].size() - 1)
						new_file << "~" <<all_seqenced_assigns[i][k];
					else{
						if(k == index_of_last_x)
							new_file << "~" << all_seqenced_assigns[i][k];
						else
							new_file << "~" << all_seqenced_assigns[i][k] << "&";
					}
				}

			}
			if(j != all_minimized[i].size() - 1)
				new_file << " | ";
		}
		new_file << ";";
		new_file << endl;
	}
	for(int i = 0 ; i < mux_lines.size() ; i++){
		new_file << mux_lines[i] << endl;
	}
	new_file << "endmodule" << endl;

	/////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////


	int counter = 0;
	while(counter < assign_lines.size()){
		for(int i = 0 ; i < all_minimized[counter].size() ; i++){
			for(int j = 0 ; j < all_minimized[counter][i].size() ; j++){
				cout << all_minimized[counter][i][j] << "  " ;
			}
			cout << endl;
		}
		for(int i = 0 ; i < all_seqenced_assigns[counter].size() ; i++){
			cout << all_seqenced_assigns[counter][i] << "   ";
		}
		cout << endl;
		cout << endl;
		counter++;
	}

	/*cout << "*********************" << endl;
	for(int i = 0 ; i < all_wires_address.size() ; i++){
		cout << "name:  " << all_wires_address[i]->name << "     " << "value:  " << all_wires_address[i]->value << endl;
	}*/
}



void do_QM(vector<string> assign_lines , vector<string> words , vector<string> inputs , vector<string> outputs , vector<string> wires , 
		   vector<vector<vector<string> > >& all_minimized , vector<vector<string> >& all_seqenced_assigns){
	for(int loop = 0 ; loop < assign_lines.size() ; loop++){
		words.clear();
		words = string_to_word(assign_lines[loop]);
		if(find(words.begin(), words.end(), "?") != words.end()){
			cout << "should be assigned to mux" << endl;
		}
		else{
			vector<vector<string> > cube_0;
			vector<vector<string> > cube_next;
			vector<vector<string> > cube_left;
			string element_orders;
			vector<string> assign_orders;
			generate_primes(cube_0 , cube_next , cube_left , inputs , wires , outputs , assign_lines[loop] , element_orders , assign_orders);
		/////////////make ready to pre end
			vector<vector<int> > can_make(cube_0.size()-1);
			can_make = generate_proper_groups(can_make , cube_0 , cube_left);
		////////////pre end
			vector<int> final_indexes;
			final_indexes = add_essentials(final_indexes , can_make);
		//////////end
			do_minimization(final_indexes , can_make);
			vector<vector<string> > final_expressions(final_indexes.size());
			final_expressions = generate_final_expressions(final_expressions , final_indexes , cube_left);

			all_seqenced_assigns.push_back(assign_orders);
			all_minimized.push_back(final_expressions);
		}
	}
}



void generate_primes(vector<vector<string> >& cube_0 , vector<vector<string> >& cube_next , vector<vector<string> >& cube_left , vector<string> inputs ,
                     vector<string> outputs , vector<string> wires , string assign_lines , string& element_orders , vector<string>& assign_orders){
	cube_0 = generate_cube_0(assign_lines , inputs , outputs , wires , element_orders , assign_orders);
	
	generate_cubes(cube_0 , cube_next , cube_left);
	cube_next = unique_vector(cube_next);
//////////////
	int count = 0;
	while(cube_next.size() != 0){
		vector<vector<string> > cube_temp;
		cube_temp = cube_next;
		cube_next.clear();
		generate_cubes2(cube_temp , cube_next , cube_left);
		cube_next = unique_vector(cube_next);
		count++;
	}
}

vector<vector<string> > generate_final_expressions(vector<vector<string> > final_expressions , vector<int>& final_indexes , vector<vector<string> > cube_left){
	for(int i = 0 ; i < final_indexes.size() ; i++){
		final_expressions[i] = cube_left[final_indexes[i]];
	}
	return final_expressions;
}

void do_minimization(vector<int>& final_indexes , vector<vector<int> >& can_make){
	int go = 2;
	while(minimization_done(can_make) != 1){

		final_indexes = add_others(final_indexes , can_make , go);
		go++;
	}
}

bool minimization_done(vector<vector<int> > can_make){
	int done = 1;
	for(int i = 0 ; i < can_make.size() ; i++){
		if(can_make[i].size() == 0)
			continue;
		else if(can_make[i].size() > 0)
			done = 0;
	}
	if(done == 1)
		return 1;
	else 
		return 0;
}

vector<int> add_others(vector<int> final_indexes , vector<vector<int> >& can_make , int size){
	for(int i = 0 ; i < can_make.size() ; i++){
		if(can_make[i].size() == size){
			vector<int> quantity;
			for(int j = 0 ; j < size ; j++){
				number_counts(can_make , quantity , can_make[i][j]);
			}
			int maxElementIndex = std::max_element(quantity.begin(),quantity.end()) - quantity.begin();
			final_indexes.push_back(can_make[i][maxElementIndex]);
			delete_rows(can_make , can_make[i][maxElementIndex]);
		}
	}
	return final_indexes;
}

void number_counts(vector<vector<int> >& can_make , vector<int>& quantity , int index){
	int cnt = 0;
	for(int i = 0 ; i < can_make.size() ; i++){
		if(std::find(can_make[i].begin(), can_make[i].end(), index) != can_make[i].end())
			cnt++;
	}
	quantity.push_back(cnt);
}

vector<int> add_essentials(vector<int> final_indexes , vector<vector<int> >& can_make){
	for(int i = 0 ; i < can_make.size() ; i++){
		if(can_make[i].size() == 1){
			final_indexes.push_back(can_make[i][0]);
			delete_rows(can_make , can_make[i][0]);
		}
	}
	return final_indexes;
}
void delete_rows(vector<vector<int> >& can_make , int index){
	for(int i = 0 ; i < can_make.size() ; i++){
		if(std::find(can_make[i].begin(), can_make[i].end(), index) != can_make[i].end())
			can_make[i].clear();
	}
}

vector<vector<int> > generate_proper_groups(vector<vector<int> > can_make , vector<vector<string> > cube_0 , vector<vector<string> > cube_left){
	for(int i = 0 ; i < cube_0.size()-1 ; i++){
		for(int j = 0 ; j < cube_left.size() ; j++){
			if(is_proper_group(cube_0[i] , cube_left[j]) == 1){
				can_make[i].push_back(j);
			}
		}
	}

	return can_make;
}
bool is_proper_group(vector<string> cube_0 , vector<string> cube_left){
	int diff = 0;
	for(int i = 0 ; i < cube_0.size() ; i++){
		if(cube_0[i] == cube_left[i])
			continue;
		if(cube_0[i] == "1" && cube_left[i] == "x")
			continue;
		if(cube_0[i] == "0" && cube_left[i] == "x")
			continue;
		if(cube_0[i] == "1" && cube_left[i] == "0")
			diff++;
		if(cube_0[i] == "0" && cube_left[i] == "1")
			diff++;
	}
	if(diff == 0)
		return 1;
	else
		return 0;
}

vector<vector<string> > unique_vector(vector<vector<string> > cube){
	vector<vector<string> > new_cube;
	for(int i = 0 ; i < cube.size() ; i++){
		new_cube.push_back(cube[i]);
		for(int j = i+1 ; j < cube.size() ; j++){
			if(cube[i] == cube[j])
				new_cube.pop_back();
		}
	}
	return new_cube;
}

void generate_cubes(vector<vector<string> >& cube_0 , vector<vector<string> >& cube_next , vector<vector<string> >& cube_left){
	vector<int> status(cube_0.size()-1 , 0);
	for(int i = 0 ; i < cube_0.size()-1 ; i++){
		for(int j = i+1 ; j < cube_0.size()-1 ; j++){
			bool adjcnt = compare(cube_0[i] , cube_0[j]);
			if(adjcnt == 1){
				vector<string> combined = combine(cube_0[i] , cube_0[j]);
				cube_next.push_back(combined);
				status[i] = 1;
				status[j] = 1;
			}
		}
	}
	for(int i = 0 ; i < status.size() ; i++){
		if(status[i] == 0){
			cube_left.push_back(cube_0[i]);
		}
	}
}
void generate_cubes2(vector<vector<string> >& cube_0 , vector<vector<string> >& cube_next , vector<vector<string> >& cube_left){
	vector<int> status(cube_0.size() , 0);
	for(int i = 0 ; i < cube_0.size() ; i++){
		for(int j = i+1 ; j < cube_0.size() ; j++){
			bool adjcnt = compare(cube_0[i] , cube_0[j]);
			if(adjcnt == 1){
				vector<string> combined = combine(cube_0[i] , cube_0[j]);
				cube_next.push_back(combined);
				status[i] = 1;
				status[j] = 1;
			}
		}
	}
	for(int i = 0 ; i < status.size() ; i++){
		if(status[i] == 0){
			cube_left.push_back(cube_0[i]);
		}
	}
}

vector<string> combine(vector<string> str1 , vector<string> str2){
	vector<string> combined_vector;
	for(int i = 0 ; i < str1.size() ; i++){
		if(str1[i] != str2[i]){
			combined_vector.push_back("x");
		}
		else if(str1[i] == str2[i]){
			if(str2[i] == "x")
				combined_vector.push_back("x");
			if(str2[i] == "0")
				combined_vector.push_back("0");
			if(str2[i] == "1")
				combined_vector.push_back("1");
		}
	}

	return combined_vector;
}


bool compare(vector<string> str1 , vector<string> str2){
	int diff = 0;
	for(int i = 0 ; i < str1.size() ; i++){
		if(str1[i] != str2[i]){
			diff++;
		}
	}
	if(diff == 1)
		return 1;
	else
		return 0;
}

vector<string> seq_inputs(string first_line , vector<string> input_list){
  vector<string> line_to_word;
  vector<string> sequenced;
  line_to_word = string_to_word(first_line);
  for(int i = 2 ; i < line_to_word.size() ; i++){
    if (find(input_list.begin(), input_list.end(), line_to_word[i]) != input_list.end())
      sequenced.push_back(line_to_word[i]);
  } 
  return sequenced;
}
vector<string> seq_outputs(string first_line , vector<string> output_list){
  vector<string> line_to_word;
  vector<string> sequenced;
  line_to_word = string_to_word(first_line);
  for(int i = 2 ; i < line_to_word.size() ; i++){
    if (find(output_list.begin(), output_list.end(), line_to_word[i]) != output_list.end())
      sequenced.push_back(line_to_word[i]);
  } 
  return sequenced;
}

vector<string> figure_assign_outputs(vector<string> assign_lines){
	vector<string> outs;
	vector<string> words;
	for(int i = 0 ; i < assign_lines.size() ; i++){
		words = string_to_word(assign_lines[i]);
		outs.push_back(words[2]);
	}
	return outs;
}


int get_index(string str , vector<string> names){
	auto it = find(names.begin(), names.end(), str);
    if (it != names.end())
    {

        int index = it - names.begin();
        return index;
    }
    else {
        return -1;
    }
}

vector<vector<string> > generate_cube_0(string assign_lines , vector<string> inputs , vector<string> outputs , vector<string> wires , string& element_orders , vector<string>& assign_orders){
	vector<string> words;
	vector<string> cube_var;
	//assign_lines.erase(remove(assign_lines.begin(), assign_lines.end(), ' '), assign_lines.end());
	words = string_to_word(assign_lines);
	for(int i = 3 ; i < words.size() ; i++){
		cube_var.push_back(words[i]);
	}
	assign_orders = string_to_word_2(cube_var[0]);
	for(int i = 0 ; i < cube_var.size() ; i++){
		cube_var[i].erase(remove(cube_var[i].begin(), cube_var[i].end(), '&'), cube_var[i].end());
	}
	//orders
	element_orders = cube_var[0];
	element_orders.erase(remove(element_orders.begin(), element_orders.end(), '~'), element_orders.end());
	//generate number
	vector<vector<string> > cube_num;

	for(int i = 0 ; i < cube_var.size() ; i++){
		vector<string> each_num;
		for(int j = 0 ; j < element_orders.size() ; j++){
			//if in inputs
			if(find(inputs.begin(), inputs.end(), string(1,element_orders[j])) != inputs.end()){
				int pos = find(inputs.begin(), inputs.end(), string(1,element_orders[j])) - inputs.begin();
				int index = cube_var[i].find(inputs[pos]);
				if(index == 0){
					each_num.push_back("1");
				}
				else if( index > 0){
					if(cube_var[i][index-1] == '~'){
						each_num.push_back("0");
					}
					else{
						each_num.push_back("1");
					}
				}
			}
			//if in wires
			if(find(wires.begin(), wires.end(), string(1,element_orders[j])) != wires.end()){
				int pos = find(wires.begin(), wires.end(), string(1,element_orders[j])) - wires.begin();
				int index = cube_var[i].find(wires[pos]);
				if(index == 0){
					each_num.push_back("1");
				}
				else if( index > 0){
					if(cube_var[i][index-1] == '~'){
						each_num.push_back("0");
					}
					else{
						each_num.push_back("1");
					}
				}
			}
			//if in outputs
			if(find(outputs.begin(), outputs.end(), string(1,element_orders[j])) != outputs.end()){
				int pos = find(outputs.begin(), outputs.end(), string(1,element_orders[j])) - outputs.begin();
				int index = cube_var[i].find(outputs[pos]);
				if(index == 0){
					each_num.push_back("1");
				}
				else if( index > 0){
					if(cube_var[i][index-1] == '~'){
						each_num.push_back("0");
					}
					else{
						each_num.push_back("1");
					}
				}
			}

		}
		cube_num.push_back(each_num);
	}
	
	return cube_num;
}

vector<string> seperate_variabels(string str){
	vector<string> words; 
    string word = ""; 
    for (auto x : str)  { 
      	if (x == '&'){ 
        	if(word != "")  
            	words.push_back(word);
            word = ""; 
      	} 
      	else{ 
        	word = word + x; 
      	} 
    }    
  	words.push_back(word);
    return words; 
}

vector<string> string_to_word(string str){
  	vector<string> words; 
    string word = ""; 
    for (auto x : str)  { 
      	if (x == '=' || x == '(' || x == ')' || x == ',' || x == ';' || x == '#' || x == '|' || x == ' '){ 
        	if(word != "")  
            	words.push_back(word);
            word = ""; 
      	} 
      	else{ 
        	word = word + x; 
      	} 
    }    
  	words.push_back(word);
    return words; 
} 

vector<string> string_to_word_2(string str){
    vector<string> words; 
    string word = ""; 
    for (auto x : str)  { 
        if (x == '&' || x == '~'){ 
          if(word != "")  
              words.push_back(word);
            word = ""; 
        } 
        else{ 
          word = word + x; 
        } 
    }    
    words.push_back(word);
    return words; 
} 


vector<string> get_lines_of_file(vector<string> lines , string file_name){

    ifstream in(file_name);

    if(!in) {
        cout << "Cannot open input file.\n";
        return lines;
    }

    char str[255];

    while(in) {
        in.getline(str, 255); 
        if(in) 
          lines.push_back(str);
    }

    in.close();
    return lines;
}