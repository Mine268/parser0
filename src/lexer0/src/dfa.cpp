#include "dfa.hpp"

namespace lexer0 {
    dfa::dfa(size_type st, status_type ini) : trans(st), inv_trans(st),
                             accept_status(st, false), trap_status(st, true),
                             curr_status{ini}, is_trapped{true}, size{st}, ini_status{ini} {}

    void dfa::add_trans(status_type from, status_type to, input_type v) {
        auto trans_it = trans.at(from).find(v);
        if (trans_it == trans.at(from).end()) {
            trans.at(from).insert(std::make_pair(v, to));
            inv_trans.at(to).push_back(from);

            registered_input.insert(v);
        }
    }

    void dfa::add_accept(status_type acc_status) {
        accept_status.at(acc_status) = true;
        untrap(acc_status);
        is_trapped = trap_status[curr_status];
    }

    std::tuple<bool, bool> dfa::trans_on(input_type input) {
        if (is_trapped) {
            return std::make_tuple(false, true);
        }
        auto &trans_map = trans.at(curr_status);
        auto next_trans_it = trans_map.find(input);
        bool is_accept = false;
        if (next_trans_it == trans_map.end()) {
            is_trapped = true;
        } else {
            curr_status = next_trans_it->second;
            is_accept = accept_status.at(curr_status);
            is_trapped = trap_status.at(curr_status);
        }
        return std::make_tuple(is_accept, is_trapped);
    }

    void dfa::reset() {
        curr_status = ini_status;
        is_trapped = false;
    }

    void dfa::untrap(status_type acc_status) {
        std::queue<status_type> q;
        q.push(acc_status);
        while (!q.empty()) {
            auto top = q.front();
            q.pop();
            for (auto prev: inv_trans.at(top)) {
                if (trap_status.at(prev)) {
                    trap_status.at(prev) = false;
                    q.push(prev);
                }
            }
        }
    }

    status_type dfa::status_code() const {
        return curr_status;
    }

    std::string dfa::to_string() const {
        std::string ret;
        for (std::size_t status = 0; status < size; ++status) {
            auto &edges = trans.at(status);
            ret += "status " + std::to_string(status) + ": ";
            for (auto &[input, next]: edges) {
                ret += "[" + std::to_string(input) + "]=>"
                       + std::to_string(next) + ' ';
            }
            ret += '\n';
        }
        ret += "from: " + std::to_string(ini_status) + '\n';
        ret += "accept:";
        for (std::size_t ix = 0; ix < accept_status.size(); ++ix) {
            if (accept_status.at(ix)) {
                ret += ' ' + std::to_string(ix);
            }
        }
        ret += '\n';
        return ret;
    }

    dfa dfa::get_optimize() {
        // bit_flagger stands for compound status
        using compound_status = bit_flagger;
        // status status_queue for optimization
        std::deque<compound_status> status_queue;
        // initial status: accept and others
        compound_status ini_all_acc{size, false}, ex_ini_all_acc{size, true};
        // flag if all the statuses are accept status
        bool is_all_acc = true;
        // configure the two initial statuses
        for (std::size_t i = 0; i < accept_status.size(); ++i) {
            is_all_acc &= accept_status.at(i);
            if (accept_status.at(i)) {
                ini_all_acc.set(i, true);
                ex_ini_all_acc.set(i, false);
            }
        }
        if (is_all_acc) {
            dfa easy_dfa {1};
            for (auto input : registered_input) {
                easy_dfa.add_trans(0, 0, input);
            }
            easy_dfa.add_accept(0);
            return easy_dfa;
        }
        // trans on compound_status
        auto c_trans_on = [&](compound_status &c_status, input_type v) -> compound_status {
            compound_status ret{size, false};
            for (std::size_t s = 0; s < size; ++s) {
                if (c_status.get(s)) {
                    ret.set(trans.at(s).find(v)->second, true);
                }
            }
            return ret;
        };
        // given the status code, return the ix of status_queue which the element contains the code
        auto locate = [&](status_type status) -> std::size_t {
            // elements in status_queue are a full partition of entire status space
            // without overlapping, so I stop the iteration on finding the one
            // containing the status
            for (std::size_t i = 0; i < status_queue.size(); ++i) {
                if (status_queue.at(i).get(status)) {
                    return i;
                }
            }
            // return -1(std::size_t) on login error occurred
            return -1;
        };
        // check if the compound is inside any compound status in the queue
        auto in_one = [&](compound_status &c_status) -> bool {
            bool on_start = true;
            std::size_t c_ix = 0;
            for (std::size_t s = 0; s < size; ++s) {
                if (c_status.get(s)) {
                    if (on_start) {
                        on_start = false;
                        c_ix = locate(s);
                    } else {
                        auto tmp_ix = locate(s);
                        if (tmp_ix != c_ix) {
                            return false;
                        }
                    }
                }
            }
            return true;
        };
        // get iterator points at ix
        auto get_it = [&](std::size_t ix) {
            auto ret = status_queue.begin();
            for (std::size_t i = 0; i < ix && ret != status_queue.end(); ++i) {
                ++ret;
            }
            return ret;
        };
        // get first non-zero index in bit_flagger/compound_status
        auto first_nonzero_ix = [&](compound_status &cs) -> std::size_t {
            for (std::size_t i = 0; i < size; ++i) {
                if (cs.get(i)) {
                    return i;
                }
            }
            return -1;
        };

        // push the initial statuses into the status_queue
        status_queue.push_back(ex_ini_all_acc);
        status_queue.push_back(ini_all_acc);
        // iterate through the status_queue,
        for (auto q_ix = 0; q_ix != status_queue.size(); ++q_ix) {
            // transit on every input
            for (auto input: registered_input) {
                auto tmp_trans_status = c_trans_on(status_queue.at(q_ix), input);
                if (in_one(tmp_trans_status)) {
                    continue;
                } else {
                    std::map<std::size_t, compound_status> divide_map;
                    for (std::size_t s_ix = 0; s_ix < size; ++s_ix) {
                        if (status_queue.at(q_ix).get(s_ix)) {
                            auto d_q_ix = locate(trans.at(s_ix).find(input)->second);
                            auto d_q_map_item = divide_map.find(d_q_ix);
                            if (d_q_map_item == divide_map.end()) {
                                auto to_be_inserted = compound_status{size, false};
                                to_be_inserted.set(s_ix, true);
                                divide_map.insert(std::make_pair(d_q_ix, std::move(to_be_inserted)));
                            } else {
                                d_q_map_item->second.set(s_ix, true);
                            }
                        }
                    }
                    status_queue.erase(get_it(q_ix--));
                    for (auto &map_pair: divide_map) {
                        status_queue.push_back(std::move(map_pair.second));
                    }
                    goto jump_out;
                }
            }
            jump_out:;
        }

        // construct the DFA
        dfa ret{status_queue.size(), locate(0)};
        for (std::size_t new_status_ix = 0; new_status_ix < status_queue.size(); ++new_status_ix) {
            for (auto input: registered_input) {
                auto tmp = first_nonzero_ix(status_queue.at(new_status_ix));
                auto new_next_status_ix = locate(
                        trans.at(tmp).find(input)->second);
                ret.add_trans(new_status_ix, new_next_status_ix, input);
            }
        }
        for (std::size_t old_acc_ix = 0; old_acc_ix < size; ++old_acc_ix) {
            if (accept_status.at(old_acc_ix)) {
                ret.add_accept(locate(old_acc_ix));
            }
        }

        return ret;
    }
}