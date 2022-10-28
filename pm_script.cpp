#include <iostream>
#include <fstream>
#include <string.h>
#include <map>
#include <set>
#include <chrono>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

#define FOFILE "StreamTokenMapFO.config"
#define OTUPUTFILENAME "PriceMonitor.config"
#define FILEPATH "" // Keep blank for Current Directory

#define FIRSTCOLUMN 1
#define THIRDCOLUMN 0
#define FOURTHCOLUMN 100

#define FIRSTLINE "1"
#define SECONDLINE "0,PriceMonitor_Fut.log,"

// Default columns
int firstColumn = FIRSTCOLUMN;
int thirdColumn = THIRDCOLUMN;
int fourthColumn = FOURTHCOLUMN;

// FILE PATH
string ipfilename = FOFILE;
string opfilename = OTUPUTFILENAME;
string filepath = FILEPATH; // USE THIS FOR CUSTOM PATH OTHER THAN PWD

// DataStructures
map<int, int> token2Stream;
map<int, string> token2Symbol;
multiset<int> freqset;
set<int> stream_set;
int symcount = 0;

int token, stream;
string symbol;

int stream_arr[20];
int column_arr[3];
int pricemon = 0;

void printusage()
{
    cout << "-i: InputFileName\n";
    cout << "-o: OutputFileName\n";
    cout << "-s: Stream(s) Comma Seperated\n";
    cout << "-p: Path of Input and Output file\n";
    cout << "-c: Create\n";
    cout << "-v: View info\n";
    cout << "-w: Custom PriceMonitor Params\n";
}

// return total number of streams parsed
int parsestreaminfo(string str, int *p)
{
    int count = 0;
    char *ch = (char *)str.c_str();

    char *tok = strtok(ch, ",");
    int i = 0;
    while (tok)
    {
        p[i++] = atoi(tok);
        tok = strtok(NULL, ",");
        count++;
    }
    return count;
}

void writefile(int stream)
{

    ofstream fp;
    string outputfilename = filepath;
    outputfilename.append(opfilename);

    fp.open(outputfilename, ios::out | ios::app);
    if (!fp.is_open())
    {
        std::cout << "Cannot create file for writting" << endl;
        exit(0);
    }
    int count = 0;
    for (auto it = token2Stream.begin(); it != token2Stream.end(); it++)
    {
        if (it->second == stream)
        {
            fp << firstColumn << ',' << token2Symbol[it->first] << ',' << thirdColumn << ',' << fourthColumn << ',' << endl;
            count++;
        }
    }

    pricemon += count;

    std::cout << "Stream " << stream << " : " << count << " Entries" << endl;

    fp.close();
}

void createFile(string filename)
{

    ofstream fp;
    string outputfilename = filepath;
    outputfilename.append(filename);

    fp.open(outputfilename, ios::out | ios::trunc);

    if (!fp.is_open())
    {
        std::cout << "Cannot create file for writting";
        exit(0);
    }

    if (FIRSTLINE != "" && SECONDLINE != "")
    {
        fp << FIRSTLINE << endl;
        fp << SECONDLINE << endl;
    }

    fp.close();

    std::cout << filename << " File Created.\n"
              << endl;
}

void printinfo()
{

    std::cout << "\nNo. of Symbols found in each Stream: " << endl;

    std::cout << "\n";
    for (auto it = stream_set.begin(); it != stream_set.end(); it++)
    {
        std::cout << "Strm" << *it << "\t";
    }
    std::cout << endl;

    for (auto it = stream_set.begin(); it != stream_set.end(); it++)
    {
        std::cout << freqset.count(*it) << "\t";
    }
    std::cout << endl
              << endl;

    // Parsing info
    std::cout << stream_set.size() << " Unique Streams" << endl;
    std::cout << symcount << " Unique Symbols\n"
              << endl;
}

void fileParser()
{
    string inputFileName;
    if (filepath != "")
    {
        inputFileName = filepath;
        if (inputFileName[inputFileName.length() - 1] != '/')
            inputFileName.append("/");
    }
    // one time update of the file path
    filepath = inputFileName;

    inputFileName.append(ipfilename);

    ifstream fip;
    fip.open(inputFileName);

    if (!fip.is_open())
    {
        std::cout << "Error Opening Input File:  " << inputFileName << endl;
        exit(0);
    }
    std::cout << "Opening Input File : " << inputFileName << endl;

    // Parsing FILE
    string line = "DEAFULT";
    symcount = 0;
    // while(fp >> temp )
    while (!fip.eof())
    {

        getline(fip, line);

        if (line == "")
            break;

        symcount++;

        char *ch = (char *)line.c_str();
        stream = atoi(strtok(ch, ","));
        // std::cout<<stream<<" ";
        symbol = (string)strtok(NULL, ",");
        // std::cout<<symbol<<" ";
        token = atoi(strtok(NULL, ","));
        // std::cout<<token<<endl;

        token2Stream[token] = stream;
        token2Symbol[token] = symbol;
        stream_set.insert(stream);
        freqset.insert(stream);
    }
    fip.close();
}

void addStream(int stream)
{
    auto pos = stream_set.find(stream);
    if (pos != stream_set.end())
    {
        writefile(stream);
    }
    else
        std::cout << "Invalid Stream or Stream not found!" << endl;
}

void printTime(std::chrono::_V2::system_clock::time_point *stime)
{
    std::chrono::_V2::system_clock::time_point start = *stime;
    std::chrono::_V2::system_clock::time_point stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);
    duration<double> time_span = duration_cast<duration<double>>(stop - start);
    std::cout << "\nTime taken : " << time_span.count() << " Seconds\n"
              << endl;
}

int main(int argc, char *argv[])
{
    // timer
    std::chrono::_V2::system_clock::time_point start = high_resolution_clock::now();

    // new changes //options parser
    bool view, create;
    view = create = false;

    int opt, stream_count = 0;
    while ((opt = getopt(argc, argv, "i:o:s:vcp:w:h")) != -1)
    {
        switch (opt)
        {
        case 'i':
            printf("Input File name: %s\n", optarg);
            ipfilename = optarg;
            break;
        case 'o':
            printf("Output File name: %s\n", optarg);
            opfilename = optarg;
            break;
        case 's':
            printf("Stream number %s will be added\n", optarg);
            stream_count = parsestreaminfo(optarg, stream_arr);
            break;
        case 'p':
            printf("Path: %s\n", optarg);
            filepath = optarg;
            break;
        case 'c':
            printf("File will be created\n");
            create = true;
            break;
        case 'w':
            printf("Columns will be : %s\n", optarg);
            parsestreaminfo(optarg, column_arr);
            firstColumn = column_arr[0];
            thirdColumn = column_arr[1];
            fourthColumn = column_arr[2];
            break;
        case 'v':
            view = true;
            break;
        case 'h':
            printusage();
            break;
        case '?':
            printusage();
            break;
        default:
            printf("Unknown option: %c\n", optopt);
            printusage();
            break;
        }
    }
    // DRIVER CODE
    if (view)
    {
        fileParser();
        printinfo();
    }
    if (create)
    {
        if (!view)
            fileParser();

        createFile(opfilename);
        for (int i = 0; i < stream_count; i++)
            addStream(stream_arr[i]);

        std::cout << endl
                  << opfilename << " : " << pricemon << " Total entires added " << endl;

        // Total TIME
        printTime(&start);
    }
    // end changes

    // std::cout << pricemon << " Entries added to PriceMonitor.config" << endl;

    return 0;
}