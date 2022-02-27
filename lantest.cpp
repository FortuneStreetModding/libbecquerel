#include "brlan.h"
#include <fstream>

using namespace std;
using namespace bq::brlan;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "usage: lantest [filename] [outfilename]" << endl;
        return 1;
    }

    ifstream fs(argv[1], std::ios::binary | std::ios::in);
    ofstream fs1(argv[2], std::ios::binary | std::ios::out);

    Brlan brlan;
    brlan.read(fs);

    cout << boolalpha;
    cout << "bom: " << hex << brlan.bom << dec << endl;
    cout << "headersize: " << brlan.headerSize << endl;

    brlan.write(fs1);

    return 0;
}
