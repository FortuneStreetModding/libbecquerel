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
    cout << boolalpha;
    cout << "bom: " << hex << brlyt.header.bom << dec << endl;
    cout << "headersize: " << brlyt.header.headerSize << endl;
    auto &lyt1 = brlyt.header.lyt1;
    cout << "lyt_name: " << lyt1.name << endl;
    cout << "lyt_drawfromcenter: " << lyt1.drawFromCenter << endl; 
    cout << "lyt_width: " << lyt1.width << endl;
    cout << "lyt_height: " << lyt1.height << endl;
    for (auto &entry: brlyt.header.paneTable) {
        cout << "pane_name: " << entry.first << endl;
        auto txt1 = dynamic_pointer_cast<Txt1>(entry.second);
        if (txt1) {
            cout << "text: " << txt1->text << endl;
        }
    }
}