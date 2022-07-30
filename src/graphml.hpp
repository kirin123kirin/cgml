/* csv-parser | MIT Liscence | https://github.com/kirin123kirin/cgml/blob/master/LICENSE */

#ifndef GRPAPHML_HPP
#define GRPAPHML_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <fstream>

const char BEFORE_TEXT[] =
    "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
    "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" "
    "xmlns:java=\"http://www.yworks.com/xml/yfiles-common/1.0/java\" "
    "xmlns:sys=\"http://www.yworks.com/xml/yfiles-common/markup/primitives/2.0\" "
    "xmlns:x=\"http://www.yworks.com/xml/yfiles-common/markup/2.0\" "
    "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:y=\"http://www.yworks.com/xml/graphml\" "
    "xmlns:yed=\"http://www.yworks.com/xml/yed/3\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns "
    "http://www.yworks.com/xml/schema/graphml/1.1/ygraphml.xsd\">\n"
    "  <key attr.name=\"Description\" attr.type=\"string\" for=\"graph\" id=\"d0\"/>\n"
    "  <key for=\"port\" id=\"d1\" yfiles.type=\"portgraphics\"/>\n"
    "  <key for=\"port\" id=\"d2\" yfiles.type=\"portgeometry\"/>\n"
    "  <key for=\"port\" id=\"d3\" yfiles.type=\"portuserdata\"/>\n"
    "  <key attr.name=\"url\" attr.type=\"string\" for=\"node\" id=\"d4\"/>\n"
    "  <key attr.name=\"description\" attr.type=\"string\" for=\"node\" id=\"d5\"/>\n"
    "  <key for=\"node\" id=\"d6\" yfiles.type=\"nodegraphics\"/>\n"
    "  <key for=\"graphml\" id=\"d7\" yfiles.type=\"resources\"/>\n"
    "  <key attr.name=\"url\" attr.type=\"string\" for=\"edge\" id=\"d8\"/>\n"
    "  <key attr.name=\"description\" attr.type=\"string\" for=\"edge\" id=\"d9\"/>\n"
    "  <key for=\"edge\" id=\"d10\" yfiles.type=\"edgegraphics\"/>\n"
    "<graph edgedefault=\"directed\" id=\"G\">\n";

const char AFTER_TEXT[] = "</graph>\n\n</graphml>";

template <typename CharT, typename Container = std::vector<std::vector<std::basic_string<CharT>>>>
class GraphMLData {
   public:
    using row_type = typename Container::value_type;

    Container data;
    std::unordered_map<std::basic_string<CharT>, int> nodes;
    std::unordered_map<std::basic_string<CharT>, int> links;
    CharT delimiter = TYPED_LITERAL(CharT, ',');
    CharT quote = TYPED_LITERAL(CharT, '"');

   private:
    bool parsed;
    int n_cnt;
    int l_cnt;

    const char* errmsg = "argument is 2d list or tuple object?";

   public:
    // GraphMLData() : data(), nodes({}), links({}), parsed(false) {}
    // GraphMLData(std::nullptr_t) : data(), nodes({}), links({}), parsed(false) {}
    GraphMLData(Container _data) : data(_data), nodes({}), links({}), parsed(false), n_cnt(0), l_cnt(0) {}
    ~GraphMLData() {}

   private:
    void _table_parse(bool header, std::size_t needcolsize, std::size_t srcidx, std::size_t taridx) {
        int i = 0;
        for(auto&& row : data) {
            if(i++ == 0 && header)
                continue;

            if(row.empty())
                throw std::runtime_error("UnExpected Error. Empty row Found.");

            if(row.size() != needcolsize)
                throw std::runtime_error("UnExpected column size Error. expected columns num.\n");

            mapper(row[srcidx], row[taridx]);
        }
    }

    void mapper(const std::basic_string<CharT>& src,
                const std::basic_string<CharT>& tar) {  //_append_data
        if(src.empty() && tar.empty()) {
            throw std::runtime_error("Parse string Error.");
        } else if(src.empty()) {
            if(nodes.find(tar) == nodes.end())
                nodes[tar] = n_cnt++;
        } else if(tar.empty()) {
            if(nodes.find(src) == nodes.end())
                nodes[src] = n_cnt++;
        } else {
            if(nodes.find(src) == nodes.end())
                nodes[src] = n_cnt++;
            if(nodes.find(tar) == nodes.end())
                nodes[tar] = n_cnt++;

            std::basic_string<CharT> res;
            res += TYPED_LITERAL(CharT*, "\" source=\"n");
            res += std::_Integral_to_string<CharT>(nodes[src]);
            res += TYPED_LITERAL(CharT*, "\" target=\"n");
            res += std::_Integral_to_string<CharT>(nodes[tar]);

            if(links.find(res) == links.end())
                links[res] = l_cnt++;
        }
    }

   public:
    std::basic_string<CharT> to_xml() {
        if(parsed == false)
            throw std::runtime_error("yet parse data....");

        std::basic_string<CharT> res;

        for(auto&& node : nodes) {
            res += TYPED_LITERAL(CharT*, "    <node id=\"n");
            res += std::_Integral_to_string<CharT>(node.second);
            res += TYPED_LITERAL(CharT*, "\"><data key=\"d6\"><y:ShapeNode><y:NodeLabel>") + std::get<0>(node);
            res += TYPED_LITERAL(CharT*, "</y:NodeLabel></y:ShapeNode></data></node>\n");
        }

        for(auto&& link : links) {
            res += TYPED_LITERAL(CharT*, "    <edge id=\"e");
            res += std::_Integral_to_string<CharT>(link.second);
            res += link.first;
            res += TYPED_LITERAL(CharT*, "\"></edge>\n");
        }
        return res;
    }

    void parse() {
        std::size_t i, j, _nlen, c = 0;
        parsed = true;

        for(auto&& row : data) {
            _nlen = row.size();

            for(i = 0; i < _nlen; ++i) {
                j = i;
                std::basic_string<CharT> src, tar, vs;

                if((_nlen = row.size()) == 0)
                    continue;

                while(j < _nlen) {
                    vs = row[j];
                    if(!vs.empty())
                        src.swap(vs);

                    if(++j == _nlen)
                        break;

                    tar = row[j];
                    if(!tar.empty()) {
                        ++j;
                        break;
                    }
                }
                if(!src.empty()) {
                    mapper(src, tar);
                    ++c;
                }
            }
        }

        if (c == 0)
            throw std::runtime_error("Cannot Parse Empty Data.\n");
    }

    void parse(bool header) {
        std::size_t nlen = std::size_t(-1);
        parsed = true;

        if((nlen = (*data.begin()).size()) == 0)
            return;

        if(nlen == 2 || nlen == 3) {
            return _table_parse(header, nlen, 0, 1);
        } else if(nlen == 4) {
            return _table_parse(header, nlen, 1, 2);
        } else {
            throw std::runtime_error("If you want to use this feature, at least 2 - 4 columns are needed.\n");
        }
    }

    void _locale(const char* codepage = "") {
        std::ios_base::sync_with_stdio(false);
        std::locale default_loc(codepage);
        std::locale::global(default_loc);
        std::locale ctype_default(std::locale::classic(), default_loc, std::locale::ctype);
        std::cout.imbue(ctype_default);
        std::wcout.imbue(ctype_default);
        std::cin.imbue(ctype_default);
        std::wcin.imbue(ctype_default);
    }

    int to_gml(const std::string& filename, const char* codepage = "Japanese_Japan.65001") {
        _locale(codepage);
        std::basic_ofstream<CharT> wf(filename, std::basic_ios<CharT>::out);
        if(!wf.good())
            throw std::runtime_error("Fail Writing File Ready.");

        wf << BEFORE_TEXT;
        wf << this->to_xml();
        wf << AFTER_TEXT << std::endl;

        wf.close();
        return 0;
    }

    int to_gml(const char* codepage = "Japanese_Japan.932") {
        _locale(codepage);
        std::wcout << BEFORE_TEXT;
        std::wcout << this->to_xml();
        std::wcout << AFTER_TEXT << std::endl;
        return 0;
    }
};

#endif /* GRPAPHML_HPP */
