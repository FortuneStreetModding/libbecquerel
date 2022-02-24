#include "brlyt.h"
#include <fstream>

using namespace std;
using namespace bq::brlyt;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "usage: bqtest [filename]" << endl;
        return 1;
    }
    fstream fs(argv[1]);
    Brlyt brlyt;
    brlyt.read(fs);
    cout << "bom: " << hex << brlyt.header.bom << endl;
}