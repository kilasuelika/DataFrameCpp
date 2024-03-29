#pragma once

// @kilasuelika
// May 19, 2022

#include <filesystem>

#include "DataFrame.hpp"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/lexical_cast.hpp>
//#include <numeric_limits>

namespace dfc {

// Data type and colnames.
inline std::pair<std::vector<std::string>, std::vector<DType>>
guess_column_types(const std::string &filename, const CSVIOOptions &options = CSVIOOptions()) {
    std::ifstream file(filename);

    if (file.is_open()) {

        std::vector<std::string> columns;
        std::vector<std::array<long long, 3>> type_counts; // [double, string, int].
        std::string separator1(""); // dont let quoted arguments escape themselves
        std::string separator2(","); // �ָ�
        std::string separator3("\"\'"); // let it have quoted arguments
        boost::escaped_list_separator<char> els(separator1, separator2, separator3);

        size_t C = 0;
        std::string line;
        long long line_cnt = 0;

        // Get first valid row and count columns.
        do {
            std::getline(file, line);
        } while (line.empty());

        //header
        //Remove BOM.
        const std::array<unsigned char, 3> boms{0xef, 0xbb, 0xbf};
        std::string_view sv{(char *)boms.data(), boms.size()};
        if (line.starts_with(sv)) {
            line = line.substr(3, line.size() - 3);
        }
        boost::tokenizer<boost::escaped_list_separator<char>> tok(line, els);
        for (auto beg = tok.begin(); beg != tok.end(); ++beg) {
            if (options._header == 0) {
                columns.emplace_back(std::move(boost::algorithm::trim_copy(*beg)));
            }
            ++C;
        }
        ++line_cnt;

        if (options._header < 0) {
            // If no header, reopen file.
            file.close();
            file.open(filename);
            line_cnt = 0;
        }
        // Now read one by one.
        // Assuming all columns are double.
        type_counts.resize(C);
        std::vector<bool> contain_double(C, false);
        while (std::getline(file, line)) {
            if (line_cnt > options._header) {
                boost::tokenizer<boost::escaped_list_separator<char>> tok(line, els);
                int i = 0;
                for (auto beg = tok.begin(); beg != tok.end(); ++beg) {
                    if (i > C) {
                        std::cerr << "[guess_row]: Line " << line_cnt << " has " << i
                            << " columns which is not equal to previous one." << std::endl;
                    }

                    std::string token = *beg;
                    boost::algorithm::trim(token);
                    // If contain double, then the column can't be int.
                    if (contain_double[i]) {
                        try {
                            // try cast to double
                            boost::lexical_cast<double>(token);
                            ++type_counts[i][0];
                            contain_double[i] = true;
                        } catch (...) {
                            if (token.empty() || (token == ".") || (token == "-")) {
                                contain_double[i] = true;
                                ++type_counts[i][0];
                            } else {
                                ++type_counts[i][1];
                            }
                        }
                    } else {
                        try {
                            // try if can cast to int.
                            boost::lexical_cast<int>(token);
                            ++type_counts[i][2];
                        } catch (...) {
                            try {
                                // try cast to double
                                boost::lexical_cast<double>(token);
                                ++type_counts[i][0];
                                contain_double[i] = true;
                            } catch (...) {
                                if (token.empty() || (token == ".") || (token == "-")) {
                                    contain_double[i] = true;
                                    ++type_counts[i][0];
                                } else {
                                    ++type_counts[i][1];
                                }
                            }
                        }
                    }
                    ++i;
                }
            }
            ++line_cnt;
            if (line_cnt > options.guess_rows) {
                break;
            }
        }
        std::vector<DType> types(C);
        std::transform(type_counts.begin(), type_counts.end(), types.begin(),
                       [double_threshold(options.double_threshold)](const auto &ele) -> DType {
                           if (ele[0] > 0) {
                               //No int, no string, then double.
                               if (ele[1] == 0 && ele[2] == 0) {
                                   return DType::DOUBLE;
                               } else {
                                   return DType::STRING;
                               }
                           } else {
                               //No double, no string, then int.
                               if (ele[1] == 0) {
                                   return DType::INT;
                               } else {
                                   return DType::STRING;
                               }
                           }
                       });
        /*for (int i = 0; i < columns.size(); ++i) {
            std::cout << columns[i] << " " << DTypeName[types[i]] << std::endl;
        }*/

        return std::make_pair(columns, types);
        // file.close();
    } else {
        std::cerr << "[read_csv]: Failed to open file " << filename << "." << std::endl;
    }
}

inline DataFrame read_csv(const std::string &filename,
                          const CSVIOOptions &options = CSVIOOptions()) {
    if (!std::filesystem::exists(std::filesystem::path(filename))) {
        std::cerr << "[read_csv]: File " << filename << " doesn't exists." << std::endl;
        return DataFrame();
    }
    auto [columns, types] = guess_column_types(filename, options);
    std::ifstream file(filename);

    std::string line;

    std::string separator1("");     // dont let quoted arguments escape themselves
    std::string separator2(",");    // �ָ�
    std::string separator3("\"\'"); // let it have quoted arguments
    boost::escaped_list_separator<char> els(separator1, separator2, separator3);

    DataFrame df(columns, types);
    //std::cout << df << std::endl;
    int r = 0;
    while (std::getline(file, line)) {
        // std::cout << r << std::endl;
        if (r > options._header) {
            df.append_row();
            boost::tokenizer<boost::escaped_list_separator<char>> tok(line, els);
            int c = 0;
            for (auto beg = tok.begin(); beg != tok.end(); ++beg) {
                std::string token = *beg;
                boost::algorithm::trim(token);
                switch (types[c]) {
                case DType::STRING:
                    std::get<std::vector<std::string>>(df._values[c]->_values).back() =
                        std::move(token);
                    break;
                case DType::DOUBLE:
                    try {
                        std::get<std::vector<double>>(df._values[c]->_values).back() =
                            boost::lexical_cast<double>(token);
                    } catch (...) {
                        std::get<std::vector<double>>(df._values[c]->_values).back() =
                            std::numeric_limits<double>::quiet_NaN();
                    }
                    break;
                case DType::INT:
                    std::get<std::vector<int>>(df._values[c]->_values).back() =
                        boost::lexical_cast<int>(token);
                    break;
                default:
                    break;
                }
                ++c;
            }
        }
        ++r;
    }

    return df;
}
}; // namespace dfc
