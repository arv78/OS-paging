#include <iostream>
#include <fstream>
#include <vector> 
#include <math.h>
using namespace std;

struct process {
	int name;
	int start;
	int finish;
	vector <int> memory_need;
};

struct pages {
	process process;
	bool used = false;
	int size;
};

int max_element(vector<int> array) {
	int max = array[0];
	for (int i = 1; i < array.size(); i++) {
		if (array[i] > max) {
			max = array[i];
		}
	}
	return max;
}

void print_input_queue(vector <process>proc_list, ofstream& outfile) {
	outfile << "input queue: [";
	for (int i = 0; i < proc_list.size(); i++) {
		outfile << proc_list[i].name << " ,";
	}
	outfile << " ]" << endl;
}

vector<process> sort(vector<process> proc) {
	for (int i = 0; i < proc.size(); i++) {
		for (int j = i; j < proc.size(); j++) {
			if (proc[j].start < proc[i].start) {
				process temp = proc[j];
				proc[j] = proc[i];
				proc[i] = temp;
			}
		}
	}
	return proc;
}

pages* find_space(pages* memory_pages, int need_pages, int num_pages, process current_process, vector<int> fifo_list) {

	while (need_pages > 0) {
		int count_empty = 0;
		//find empty space
		for (int i = 0; i < num_pages; i++) {
			if (!memory_pages[i].used) {
				count_empty++;
			}
		}
		//empty pages are enough
		if (count_empty >= need_pages) {
			for (int i = 0; i < num_pages; i++) {
				if (!memory_pages[i].used) {
					memory_pages[i].process = current_process;
					memory_pages[i].used = true;
					need_pages--;
				}
				if (need_pages <= 0) {
					return memory_pages;
				}
			}
		}
		//not enough pages, delete unsing fifo
		//delete first in
		int delete_page = fifo_list.front();
		fifo_list.erase(fifo_list.begin());
		for (int i = 0; i < num_pages; i++) {
			if (memory_pages[i].process.name == delete_page) {
				memory_pages[i].used = false;
			}
		}
	}
}

void print_memory(pages* memory_pages, int page_size, int num_pages, ofstream& outfile) {
	outfile << "Memory Map: ";
	int count = 0;
	int count_pages = 1;
	for (int i = 0; i < num_pages; i++) {
		outfile << count << "-" << count + page_size - 1 << ": ";
		count += page_size;
		if (memory_pages[i].used) {
			outfile << "Process " << memory_pages[i].process.name << ", Page ";
			if (i != 0 && memory_pages[i].process.name != memory_pages[i - 1].process.name) {
				count_pages = 1;
			}
			outfile << count_pages << endl;
			count_pages++;
		}
		else {
			outfile << "Free frame" << endl;
		}
	}
}

int main() {
	ofstream outfile;
	outfile.open("output.txt");
	int memory_size;
	cout << "enter memory size: ";
	cin >> memory_size; cout << endl;

	int page_size;
	cout << "enter page size: ";
	cin >> page_size; cout << endl;

	int max_run_time;
	cout << "enter maximum run time: ";
	cin >> max_run_time; cout << endl;

	// read file
	process proc;
	int count_memory;
	int temp1;
	ifstream infile("sample1.txt");
	if (infile.fail()) {
		cerr << "error" << std::endl;
	}
	int num_proccess;
	int proccess_name;
	int start_time;
	int finish_time;
	infile >> num_proccess;
	vector <process>proc_list;
	for (int i = 0; i < num_proccess; i++) {
		//cout << "yes";
		int name;
		infile >> name;
		int start;
		infile >> start;
		int finish;
		infile >> finish;
		infile >> count_memory;
		process temp;
		temp.name = name;
		temp.start = start;
		temp.finish = finish;
		for (int j = 0; j < count_memory; j++) {
			infile >> temp1;
			temp.memory_need.push_back(temp1);
		}
		proc_list.push_back(temp);

	}

	proc_list = sort(proc_list);


	//define memory and pages
	int num_pages = memory_size / page_size;
	pages* memory_pages = new pages[num_pages];

	//processes arrival
	for (int i = 0; i < proc_list.size(); i++) {
		outfile << "t = " << proc_list[i].start << endl;
		outfile << "process " << proc_list[i].name << " arrives" << endl;
		outfile << "input queue: [";

		for (int j = 0; j <= i; j++) {
			outfile << proc_list[j].name << " ,";
		}

		outfile << " ]" << endl;
	}
	//start
	vector<int> fifo_list;
	while (!proc_list.empty()) {
		//select process
		process current_process = proc_list.front();
		proc_list.erase(proc_list.begin());
		int total_memory_need = 0;
		bool enough_space = true;
		for (int i = 0; i < current_process.memory_need.size(); i++) {
			total_memory_need += current_process.memory_need[i];
		}
		if (total_memory_need > memory_size) {
			outfile << "Not enough space for process " << current_process.name << endl;
			enough_space = false;
		}
		if (enough_space) {
			if ((current_process.finish - current_process.start) <= max_run_time) {
				//find empty place for process
				for (int i = 0; i < current_process.memory_need.size(); i++) {
					int current_space_need = current_process.memory_need[i];
					memory_pages = find_space(memory_pages, ceil((float)current_space_need / page_size), num_pages, current_process, fifo_list);
					fifo_list.push_back(current_process.name);
				}
				outfile << "MM moves Process " << current_process.name << "to memory" << endl;
				print_input_queue(proc_list,outfile);
				print_memory(memory_pages, page_size, num_pages,outfile);
				max_run_time -= (current_process.finish - current_process.start);
			}
			else {
				outfile << "time out!" << endl;
				outfile.close();
				return 0;
			}
		}
	}
	outfile.close();
	return 0;
}