/**
    Copyright 2016-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#pragma once


#include <fostgres/matcher.hpp>
#include <fostgres/response.hpp>
#include <fostgres/sql.hpp>


namespace fostgres {


    /// Internal class used for PATCH and PUT
    class updater {
      public:
        const fostlib::string relation;

        updater(fostlib::json method_config,
                fostlib::pg::connection &,
                const fostgres::match &,
                fostlib::http::server::request &);

        /// Returning data
        const std::vector<fostlib::string> returning() const {
            return returning_cols;
        }

        /// ## Tracking of `UPDATE` versus `INSERT ON CONFLICT UPDATE`
        enum class action { do_default, insertable, updateable };
        /// The default action is required to support the difference in
        /// default behaviour between CSJ and object `PATCH` requests.
        /// CSJ has always defaulted to performing the `INSERT` statement,
        /// and the object requests have done the `UPDATE`. We need to
        /// keep the default for the object requests so that we don't break
        /// old `PATCH` configurations.
        ///
        /// The new behaviour for object `PATCH` will be supported if
        /// the configuration contains the new columns configuration:
        ///     "insert": "required"

        /// Break apart the data into the key and value parts
        using intermediate_data = std::pair<fostlib::json, fostlib::json>;
        intermediate_data data(const fostlib::json &data);

        /// Perform an INSERT and potentially return a response
        [[nodiscard]] std::pair<boost::shared_ptr<fostlib::mime>, int>
                insert(intermediate_data, std::optional<std::size_t> row = {});
        /// Perform an update
        [[nodiscard]] std::pair<boost::shared_ptr<fostlib::mime>, int>
                update(intermediate_data, std::optional<std::size_t> row = {});

        action perform() const { return deduced_action; }

        /// The old APIs which combine the UPDATE/INSERT with the
        /// data processing.
        [[nodiscard]] std::pair<
                std::pair<boost::shared_ptr<fostlib::mime>, int>,
                std::pair<fostlib::json, fostlib::json>>
                upsert(const fostlib::json &body_row,
                       std::optional<std::size_t> row = {});
        std::tuple<
                fostlib::json,
                fostlib::json,
                boost::shared_ptr<fostlib::mime>,
                int>
                update(const fostlib::json &body_row);

      private:
        action deduced_action;
        fostlib::json config, col_config;
        std::vector<fostlib::string> returning_cols;

        fostlib::pg::connection &cnx;
        const fostgres::match &m;
        fostlib::http::server::request &req;
    };


    std::pair<boost::shared_ptr<fostlib::mime>, int> schema_check(
            fostlib::pg::connection &cnx,
            const fostlib::json &config,
            const fostgres::match &m,
            fostlib::http::server::request &req,
            const fostlib::json &schema_config,
            const fostlib::json &instance,
            fostlib::jcursor dpos);


    /**
     * ## PUT handling
     *
     * Handle PUT requests for array sequences. Used by CSJ bodies and by
     * JSON arrays in the JSON object requests.
     */

    template<typename T>
    using ordered_keys = std::vector<T>;

    /**
     *  Create a SELECT statement to collect all the associated keys
     *  in the database. We need to SELECT across the keys not in
     *  the body data and store the keys that are in the body data
     *
     *  The bool is set to true when the key has been seen. Those still
     *  false by the end of the PUT need to be deleted.
     */
    struct put_records_seen {
        using storage_type =
                std::vector<std::pair<std::vector<fostlib::json>, bool>>;
        storage_type records;

        put_records_seen(
                fostlib::pg::connection &,
                f5::u8view select_sql,
                const match &,
                fostlib::http::server::request &);

        /// The number of records in the database before processing the
        /// PUT request.
        auto size() const noexcept { return records.size(); }

        /// Look to see if we had this data in the database before
        /// and if so mark it as seen in the PUT body
        bool record(fostlib::json const &inserted);

        /// Look through the initial keys to find any that weren't in the
        /// incoming data in the PUT body so the rows can be deleted
        std::size_t delete_left_over_records(f5::u8view delete_sql);

      private:
        fostlib::pg::connection &cnx;
        match const &m;
        std::vector<fostlib::json> key_match;
        ordered_keys<fostlib::string> key_names;
    };


}
