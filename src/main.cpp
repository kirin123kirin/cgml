/* csv-parser | MIT Liscence | https://github.com/kirin123kirin/cgml/blob/master/LICENSE */

#include "cxxopts.hpp"
#include "parser.hpp"
#include "graphml.hpp"
#include <Windows.h>
#include <io.h>

std::string gettmpdir() {
    std::string temp_dir;
    char charPath[MAX_PATH];
    if(GetTempPathA(MAX_PATH, charPath)) {
        temp_dir = charPath;
        temp_dir.erase(temp_dir.size() - 1);  // remove last separater.
    }
    return temp_dir;
}

// thanks for http://stackoverflow.com/questions/478898
void exec(const std::string cmd) {
    FILE* pipe = _popen(cmd.data(), "r");
    if(!pipe)
        throw std::runtime_error("popen() failed!");
    _pclose(pipe);
}

template <typename Container, typename T>
int from_data(Container data, const std::basic_string<T>& outpath, int header = -1) {
    GraphMLData<wchar_t> snk(data);
    if(header == -1)
        snk.parse();
    else
        snk.parse((bool)header);

    return snk.to_gml(outpath);
}

template <typename T>
int from_clipboard(const std::basic_string<T>& outpath, int header = -1) {
    int ret = 1;
    if(!OpenClipboard(nullptr))
        throw std::runtime_error("Failed Read Clipboard Data.");

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if(hData == nullptr)
        throw std::runtime_error("Failed Read Clipboard Data.");

    wchar_t* buf = static_cast<wchar_t*>(GlobalLock(hData));
    if(buf == nullptr)
        throw std::runtime_error("Failed Read Clipboard Data.");

    auto data = csv::CsvVec(buf, L'\t', L'"');
    ret = from_data(data, outpath, header);

    GlobalUnlock(hData);
    CloseClipboard();

    return ret;
}

int main(int argc, char** argv) {
    std::string outpath, default_outfile = "tmp_GraphML.graphml";

    const char sep = '\\';

    cxxopts::Options op("GraphML", "Build GraphML yEd diagram from csv or tsv data.");

    try {
        op.add_options()("o,outpath", "output GraphMLFile File Path.\noutdirectory $TMPDIR/" + default_outfile,
                         cxxopts::value<std::string>());
        op.add_options()("n,no_open", "Is Open GraphMLFile auto by Default Browser? (default auto open)",
                         cxxopts::value<bool>()->default_value("false"));
        op.add_options()("H,header", "is header? (default no header)", cxxopts::value<bool>()->default_value("false"));
        op.add_options()("w,well_formed", "is input csv data is well-formed table? (default nonwell-formed)",
                         cxxopts::value<bool>()->default_value("false"));
        op.add_options()("s,sep", "separator(delimiter) of csv data. (default `,`)",
                         cxxopts::value<std::string>()->default_value(std::string(",")));
        op.add_options()("quote", "quote charactor of csv data. (default `\"`)",
                         cxxopts::value<std::string>()->default_value(std::string("\"")));
        op.add_options()("h,help", "Print usage");

        auto opts = op.parse(argc, argv);

        if(opts.count("help")) {
            std::cerr << op.help() << std::endl;
            exit(0);
        }

        if(opts.count("outpath"))
            outpath = opts["outpath"].as<std::string>();
        else
            outpath = gettmpdir() + sep + default_outfile;

        int ret;
        bool header = opts["header"].as<bool>();
        const wchar_t delimiter = (wchar_t)opts["sep"].as<std::string>()[0];
        const wchar_t quote = (wchar_t)opts["quote"].as<std::string>()[0];

        if(opts.unmatched().empty()) {
            int nopipe = _isatty(_fileno(stdin));
            if(nopipe) {
                if(header)
                    ret = from_clipboard(outpath, header);
                else
                    ret = from_clipboard(outpath);
            } else {
                auto data = csv::CsvstdinVec(delimiter, quote);
                ret = from_data(data, outpath, header);
            }
            if(ret) {
                std::cerr << "\nFailed Build Output File ->\n\t" << outpath << std::endl;
                exit(ret);
            }

        } else {
            int i = 1;
            for(auto&& inp : opts.unmatched()) {
                auto data = csv::CsvfileVec(inp, delimiter, quote);
                std::string file = outpath;
                std::size_t pos = outpath.find_last_of(".htm");
                if(pos != std::string::npos)
                    file.insert(pos, "_arg_" + std::to_string(i));

                ret = from_data(data, file, header);
                if(ret) {
                    std::cerr << "\nFailed Build Output File ->\n\t" << file << std::endl;
                    exit(ret);
                }
            }
        }

        bool auto_open = !opts["no_open"].as<bool>();

        if(auto_open) {
            exec("start /MIN yEd.exe " + outpath);
            std::cerr << "Opening Build GraphMLFile..." << std::endl;
            std::cerr << outpath << std::endl;
        }
        std::cerr << "Success!" << std::endl;
    } catch(std::exception& e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }
    return 0;
}
