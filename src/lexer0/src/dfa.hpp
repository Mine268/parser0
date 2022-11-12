#pragma once

#include <utility>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <type_traits>
#include <string>

#include "bit_flagger.hpp"

namespace lexer0 {

    using index_type = std::size_t;
    using size_type = index_type;
    using status_type = std::size_t;
    using input_type = int;

    class dfa {
        friend class nfa;
    private:
        // status size
        size_type size;
        // initial status
        status_type ini_status;
        // current status of dfa
        status_type curr_status;
        /* flag, indicating whether there is possible path from
            current state to accepting status */
        bool is_trapped;

        // input code seen
        std::set<input_type> registered_input;

        // transition edge for every status on designated input
        std::vector<std::map<input_type, status_type>> trans;
        /* the possible status that might transimit to current
            status in 1 step */
        std::vector<std::vector<status_type>> inv_trans;
        // accepting status
        std::vector<bool> accept_status;
        /* indicating whether there is possible path from the 
            current status to accepting status*/
        std::vector<bool> trap_status;

        // updating the *trap_status* given the accepting status
        void untrap(status_type);

    public:
        explicit dfa(size_type, status_type = 0);
        /**
         * @brief Create a transition edge from status *from*
         *  to status *to* on input <code>v</code>.
        */
        void add_trans(status_type, status_type, input_type);
        /**
         * @brief Make status <code>acc_status</code> the accepting status.
        */
        void add_accept(status_type);
        /**
         * @brief Feed a input character to the dfa, the return-
         * ing value is
         * <il>
         *  <li>Is the dfa at the accepting status</li>
         *  <li>Is there no possible path to the accepting status</li>
         * </il>
        */
        std::tuple<bool, bool> trans_on(input_type);
        /**
         * @brief Reset the dfa.
        */
        void reset();
        /**
         * @brief Get the current status code
        */
        [[nodiscard]] status_type status_code() const;
        /**
         * Get the description
         * @return description
         */
        [[nodiscard]] std::string to_string() const;

        /**
         * Get the optimized DFA from current DFA, this method should
         * be invoked on the DFA where the results for every input on
         * every status is given, for example <code>nfa::get_nfa</code>.
         * @return Optimized DFA
         */
        [[nodiscard]] dfa get_optimize();
    };

}
