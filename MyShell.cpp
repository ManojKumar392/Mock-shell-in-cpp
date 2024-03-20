#include<cstdlib>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<unordered_map>
#include<unistd.h>
#include<fstream>
#include<filesystem>

using namespace std;
namespace fs = std::filesystem;

string lsh_read_line() {
    string line;
    if (getline(cin, line)) {
        return line;
    } else {
        throw std::runtime_error("Error reading line from std::cin");
    }
}

vector<string> lsh_split_line(string line) {
    stringstream ss(line);
    string word;
    vector<string> words;
    while(getline(ss,word,' ')) {
        words.push_back(word);
    }
    return words;
}

int cdFunc(vector<string> args) {
    if(args.size()==0) return 0;
    else {
        if(chdir(args[0].c_str())!=0) {
            perror("lsh");
        }
    }
    char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
                std::printf("Current working directory: %s\n", cwd);
        }
    return 1;
}

int lsFunc(vector<string> args) {
    fs::path currentDir = fs::current_path();

    for (const auto& entry : fs::directory_iterator(currentDir)) {
        
        if (entry.is_directory()) {
            std::cout << entry.path().filename() << std::endl;
        }
    }

    return 1;
}

int touchFunc(vector<string> args) {
    string filepath = args[0];
    try {
        if(!fs::exists(filepath)) {
            ofstream tempfile(filepath);
            tempfile.close();
            cout<<"File created: "<< filepath<<endl;
        }
        auto currentTime = filesystem::file_time_type::clock::now();

        fs::last_write_time(filepath,currentTime);
        fs::last_write_time(filepath,currentTime);

        cout<<"Timestamps updated for files: "<<filepath<<endl;
    } catch (const exception& ex) {
        cerr<<"Error:"<<ex.what()<<endl;
    }
    return 1;
}

int cpFunc(vector<string> args) {
    if (args.size() != 2) {
        cerr << "Usage: cp <source> <destination>" << endl;
        return 1;
    }

    fs::path sourcePath = args[0];
    fs::path destinationPath = args[1];

    try {
        fs::copy(sourcePath, destinationPath, fs::copy_options::overwrite_existing);
        cout << "File copied successfully from " << sourcePath << " to " << destinationPath << endl;
        return 0;
    } catch (const exception& ex) {
        cerr << "Error copying file: " << ex.what() << endl;
        return 1;
    }
}

int clrFunc(vector<string> args) {
    cout << "\033[2J\033[1;1H"; 
    return 1;
}

vector<string> commandHistory;

int historyFunc(vector<string> args) {
    for (size_t i = 0; i < commandHistory.size(); ++i) {
        cout << i + 1 << ": " << commandHistory[i] << endl;
    }
    return 0;
}

int lsh_execute(vector<string> args)
{
    unordered_map<string,int(*)(vector<string>)> commands; 
    commands["cd"] = cdFunc;
    commands["ls"] = lsFunc;
    commands["touch"] = touchFunc;
    commands["cp"] = cpFunc;
    commands["clr"] = clrFunc;
    commands["history"] = historyFunc;
    if(args.size()==0)
    {
        return 1;
    }
    else {
        if(commands.find(args[0])==commands.end()) return 0;
        else {
            int(*function)(vector<string>) = commands[args[0]];
            vector<string> newargs(args.begin()+1,args.end());
            return function(newargs);
        }
    }
}

void lsh_loop() {
    string line;
    vector<string> args;
    int status;

    do {
        printf("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);
        if (!args.empty()) {
            commandHistory.push_back(line);
        }
    } while(status);
}


int main(int argc, char **argv)
{
    lsh_loop();
    return EXIT_SUCCESS;
}