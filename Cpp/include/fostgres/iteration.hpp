/*
    Copyright 2016-2017, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#pragma once


#include <fost/postgres>


namespace fostgres {

    /// Add in the column names to a recordset
    std::pair<std::vector<fostlib::string>, fostlib::pg::recordset>
        column_names(fostlib::pg::recordset &&rs);


    /// A range that allows the recordset to be iterated to produce JSON
    /// objects, one per row.
    class json_recordset {
        std::vector<fostlib::string> names;
        fostlib::pg::recordset rs;
    public:
        json_recordset(std::pair<std::vector<fostlib::string>, fostlib::pg::recordset> rs);

        class const_iterator :
            public std::iterator<std::input_iterator_tag, fostlib::json::object_t>
        {
            friend class json_recordset;

            const std::vector<fostlib::string> *names;
            fostlib::pg::recordset::const_iterator pos;
            fostlib::json::object_t object;

            const_iterator(
                const std::vector<fostlib::string> &names,
                fostlib::pg::recordset::const_iterator p);
        public:
            const_iterator()
            : names(nullptr) {
            }
            const_iterator(const const_iterator &) = default;

            const_iterator &operator = (const const_iterator &) = default;

            bool operator == (const const_iterator &other) const {
                return names == other.names && pos == other.pos;
            }
            bool operator != (const const_iterator &other) const {
                return not (*this == other);
            }

            const_iterator &operator ++ ();
            const_iterator operator ++ (int);

            fostlib::json::object_t &operator * ();
        };

        const_iterator begin() const;
        const_iterator end() const;
    };


}

