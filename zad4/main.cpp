#include "pch.h"

struct berResults
{
    double tot; //total number of bits
    double err; //error bits (different bits)
    float ber;  //ber calc result
    clock_t t1; //calc start time
    clock_t t2; //calc stop time
};

uint8_t hammingDistance(uint8_t n1, uint8_t n2);
void createFile1(const std::string name, const int count, const char value);
berResults calculateBer(std::string fpath1, std::string fpath2);
void printResult(berResults results);

int main(int argc, char* argv[])
{
    std::string fpath1; 
    std::string fpath2; 
    berResults results;

    openLog("log.log"); 
    if (argc != 3) 
    {
        saveLog("Tworzenie testowych plikow");
        createFile1("t1file1.bin", 100, 0x55);
        createFile1("t1file2.bin", 100, 0x55);

        createFile1("t2file1.bin", 100, 0x55);
        createFile1("t2file2.bin", 90, 0x55);

        createFile1("t3file1.bin", 400000000, 0x50);
        createFile1("t3file2.bin", 400000000, 0x50);
        saveLog("Pliki testowe zostaly stworzone");
    }
    else //arguments are passed correctly
    {
        fpath1 = argv[1];
        fpath2 = argv[2];

        saveLog("Processing test files");
        results = calculateBer(fpath1, fpath2);
        printResult(results);
    }

    closeLog();
    return 0;
}

uint8_t hammingDistance(uint8_t n1, uint8_t n2)
{
    uint8_t x = n1 ^ n2; // XOR
    uint8_t setBits = 0;
    while (x > 0)
    {
        setBits += x & 1;
        x >>= 1;
    }
    return setBits;
}

void createFile1(const std::string name, const int count, const char value)
{
    std::fstream f;
    f.open(name.c_str(), std::ios::binary | std::ios::out);
    for (int i = 0; i < count; i++)
    {
        f.write((char*)&value, 1);
    }
    f.close();
}

berResults calculateBer(std::string fpath1, std::string fpath2)
{
    std::fstream f1, f2; 
    berResults results;
    results.t1 = 0;
    results.t2 = 0;
    results.ber = 0;
    results.err = 0;
    results.tot = 0;

    saveLog("Calculating BER...");
    f1.open(fpath1.c_str(), std::ios::binary | std::ios::in);
    f2.open(fpath2.c_str(), std::ios::binary | std::ios::in);
    char a = 0x00;
    char b = 0x00;
    results.t1 = clock();

    while (!f1.eof())
    {
        f1 >> a; 
        f2 >> b; 
        if (!f1.eof()) 
        {
            results.err += hammingDistance(a, b); 
            results.tot += 8; 
        }
    }

    results.ber = (float)results.err / results.tot; 
    results.t2 = clock();
    saveLog("BER calculations are done");
    return results; 
}

void printResult(berResults results)
{
    std::stringstream message;
    message << "Results are: " << std::endl;
    message << "BER: " << results.ber << std::endl;
    message << "Tot: " << results.tot << std::endl;
    message << "Err: " << results.err << std::endl;
    message << "Calc time: " << ((float)results.t2 - results.t1) / CLOCKS_PER_SEC << " sec " << std::endl;
    saveLog(message.str());
}