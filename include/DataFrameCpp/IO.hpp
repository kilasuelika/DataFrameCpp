#pragma once

// @kilasuelika
// May 19, 2022

#include "DataFrame.hpp"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/lexical_cast.hpp>
//#include <numeric_limits>

namespace dfc {

// Data type and colnames.
std::pair<std::vector<std::string>, std::vector<DType>>
guess_column_types(const std::string &filename, const CSVIOOptions &options = CSVIOOptions()) {
    std::ifstream file(filename);

    if (file.is_open()) {

        std::vector<std::string> columns;
        std::vector<std::array<long long, 2>>
            type_counts;                // Counts valid double elements in each column.
        std::string separator1("");     // dont let quoted arguments escape themselves
        std::string separator2(",");    // ·Ö¸î
        std::string separator3("\"\'"); // let it have quoted arguments
        boost::escaped_list_separator<char> els(separator1, separator2, separator3);

        size_t C = 0;
        std::string line;
        long long line_cnt = 0;

        // Get first valid row and count columns.
        do {
            std::getline(file, line);
        } while (line.empty());

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
        while (std::getline(file, line)) {
            if (line_cnt > options._header) {
                boost::tokenizer<boost::escaped_list_separator<char>> tok(line, els);
                int i = 0;
                for (auto beg = tok.begin(); beg != tok.end(); ++beg) {
                    if (i > C) {
                        std::cerr << "[guess_row]: Line " << line_cnt << " has " << i
                                  << " columns which is not equal to previous one." << std::endl;
                    }

                    try {
                        // try if can cast to double.
                        std::string token = *beg;
                        boost::algorithm::trim(token);
                        boost::lexical_cast<double>(token);
                        ++type_counts[i][0];
                    } catch (...) {
                        ++type_counts[i][1];
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
                           if ((static_cast<double>(ele[0]) / (ele[0] + ele[1])) >
                               double_threshold) {
                               return DType::DOUBLE;
                           } else {
                               return DType::STRING;
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
DataFrame<int, Series> read_csv(const std::string &filename,
                                const CSVIOOptions &options = CSVIOOptions()) {
    auto [columns, types] = guess_column_types(filename, options);
    std::ifstream file(filename);

    std::string line;

    std::string separator1("");     // dont let quoted arguments escape themselves
    std::string separator2(",");    // ·Ö¸î
    std::string separator3("\"\'"); // let it have quoted arguments
    boost::escaped_list_separator<char> els(separator1, separator2, separator3);

    DataFrame<int, Series> df(columns, types);
    // std::cout << df << std::endl;
    int r = 0;
    while (std::getline(file, line)) {
        if (r > options._header) {
            df.push_back();
            boost::tokenizer<boost::escaped_list_separator<char>> tok(line, els);
            int c = 0;
            for (auto beg = tok.begin(); beg != tok.end(); ++beg) {
                std::string token = *beg;
                boost::algorithm::trim(token);
                switch (types[c]) {
                case DType::STRING:
                    std::get<std::vector<std::string>>(df._values[c]._values).back() =
                        std::move(token);
                    break;
                case DType::DOUBLE:
                    try {
                        std::get<std::vector<double>>(df._values[c]._values).back() =
                            boost::lexical_cast<double>(token);
                    } catch (...) {
                        std::get<std::vector<double>>(df._values[c]._values).back() =
                            std::numeric_limits<double>::quiet_NaN();
                    }
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