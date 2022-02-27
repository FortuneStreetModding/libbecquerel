#include "brlyt.h"
#include <codecvt>
#include <fstream>

using namespace std;
using namespace bq::brlyt;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "usage: bqtest [filename] [outfilename]" << endl;
        return 1;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
    ifstream fs(argv[1], std::ios::binary | std::ios::in);
    ofstream fs1(argv[2], std::ios::binary | std::ios::out);
    Brlyt brlyt;
    brlyt.read(fs);
    cout << boolalpha;
    cout << "bom: " << hex << brlyt.bom << dec << endl;
    cout << "headersize: " << brlyt.headerSize << endl;
    auto &lyt1 = brlyt.lyt1;
    cout << "lyt_name: " << lyt1.name << endl;
    cout << "lyt_drawfromcenter: " << lyt1.drawFromCenter << endl; 
    cout << "lyt_width: " << lyt1.width << endl;
    cout << "lyt_height: " << lyt1.height << endl;
    auto &mat1 = brlyt.mat1;
    for (int i=0; i<mat1.materials.size(); ++i) {
        cout << "material at " << i << ": " << mat1.materials[i].name << " (flag = " << hex << mat1.materials[i].flags << ")" << endl;
    }
    #if 0
    for (auto &entry: brlyt.paneTable) {
        cout << "pane_name: " << entry.first << endl;
        auto txt1 = dynamic_pointer_cast<Txt1>(entry.second);
        if (txt1) {
            cout << "text: " << conv.to_bytes(txt1->text) << endl;
        }
    }
    #endif
    brlyt.write(fs1);
}