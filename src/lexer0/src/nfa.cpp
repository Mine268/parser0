#include "nfa.hpp"

namespace lexer0 {

    void nfa::reset() {
        curr_status.clear();
        curr_status.push_back(0);
        trans_empty();
        is_trapped = std::accumulate(curr_status.begin(),
                                 curr_status.end(),
                                 true,
                                 [&](auto ix1, auto ix2) {
                                     return trap_status.at(ix1) && trap_status.at(ix2);
                                 });
    }

    nfa::nfa(size_type n) : size{n}, curr_status{0},
                            is_trapped{true}, trans(n),
                            inv_trans(n), accept_status(n, false),
                            trap_status(n, true)
    {
        trans_empty();
    }

    void nfa::add_accept(status_type acc_status) {
        accept_status.at(acc_status) = true;
        untrap(acc_status);
        is_trapped = std::accumulate(curr_status.begin(),
                                 curr_status.end(),
                                 true,
                                 [&](auto ix1, auto ix2) {
                                     return trap_status.at(ix1) && trap_status.at(ix2);
                                 });
    }

    void nfa::add_trans(status_type from, status_type to, std::tuple<bool, input_type> v) {
        if (std::get<0>(v)) {
            std::get<1>(v) = input_type{};
        }
        auto& trans_it = trans.at(from);
        auto tmp_edge_it = trans_it.find(v);
        if (tmp_edge_it == trans_it.end() || tmp_edge_it->second != to) {
            trans_it.insert(std::make_pair(v, to));
            std::reduce(inv_trans.at(to).begin(),
                        inv_trans.at(to).end(),
                        false,
                        [&](auto res, auto inv_ix) {
                return res || (inv_ix == from);
            }) ? 0 : (inv_trans.at(to).push_back(from), 0);
            trans_empty();

            if (!std::get<0>(v)) {
                registered_input.insert(std::get<1>(v));
            }
        }
    }

    void nfa::untrap(status_type acc_status) {
        std::queue<status_type> q;
        q.push(acc_status);
        while (!q.empty()) {
            auto top = q.front();
            q.pop();
            for (auto prev : inv_trans.at(top)) {
                if (trap_status.at(prev)) {
                    trap_status.at(prev) = false;
                    q.push(prev);
                }
            }
        }
    }

    void nfa::trans_empty() {
        trans_empty_on(curr_status);
    }

    bit_flagger nfa::status_code() const {
        bit_flagger ret{this->size, false};
        for (auto ix : curr_status) {
            ret.set(ix, true);
        }
        return ret;
    }

    std::tuple<bool, bool> nfa::trans_on(input_type v) {
        if (is_trapped) {
            curr_status.clear();
            return std::make_tuple(false, is_trapped);
        }
        curr_status = trans_on_on(curr_status, v);
        is_trapped = curr_status.empty() ||
                std::accumulate(curr_status.begin(),
                            curr_status.end(),
                            true,
                            [&](auto ix1, auto ix2) {
                    return trap_status.at(ix1) && trap_status.at(ix2);
                });
        return std::make_tuple(
                std::accumulate(curr_status.begin(),
                            curr_status.end(),
                            false,
                            [&](auto ix1, auto ix2) {
                    return accept_status.at(ix1) || accept_status.at(ix2);
                }),
                is_trapped
                );
    }

    void nfa::add_trans(status_type from, status_type to, input_type v) {
        this->add_trans(from, to, std::make_tuple(false, v));
    }

    void nfa::add_trans(status_type from, status_type to) {
        this->add_trans(from, to, std::make_tuple(true, input_type{}));
    }

    std::string nfa::to_string() const {
        std::string ret;
        for (std::size_t status = 0; status < size; ++status) {
            auto& edges = trans.at(status);
            ret += "status " + std::to_string(status) + ": ";
            for (auto &[input, next] : edges) {
                if (std::get<0>(input)) {
                    ret += "none=>" + std::to_string(next) + ' ';
                } else {
                    ret += "[" + std::to_string(std::get<1>(input)) + "]=>"
                            + std::to_string(next) + ' ';
                }
            }
            ret += '\n';
        }
        ret += "accept:";
        for (std::size_t ix = 0; ix < accept_status.size(); ++ix) {
            if (accept_status.at(ix)) {
                ret += ' ' + std::to_string(ix);
            }
        }
        ret += '\n';
        return ret;
    }

    void nfa::trans_empty_on(std::vector<status_type> &status) {
        std::queue<status_type> q;
        bit_flagger flagger{size, false};
        for (auto c: status) {
            q.push(c);
            flagger.set(c, true);
        }
        while (!q.empty()) {
            auto top = q.front();
            q.pop();
            for (auto &[key, val]: trans.at(top)) {
                if (std::get<0>(key) && !flagger.get(val)) {
                    q.push(val);
                    flagger.set(val, true);
                    status.push_back(val);
                }
            }
        }
    }

    std::vector<status_type> nfa::trans_on_on(const std::vector<status_type> &status, input_type v) {
        std::vector<status_type> next_status;
        bit_flagger flag{size, false};
        for (auto curr: status) {
            for (auto &[input, ns] : trans.at(curr)) {
                if (!std::get<0>(input) && std::get<1>(input) == v && !flag.get(ns)) {
                    next_status.push_back(ns);
                    flag.set(ns, true);
                }
            }
        }
        trans_empty_on(next_status);
        return next_status;
    }

    dfa nfa::get_dfa() {
        std::vector<std::map<input_type, status_type>> dfa_trans(1);

        std::queue<std::vector<status_type>> queue;
        std::map<std::vector<status_type>, status_type> compound_ix_map;
        status_type top_status_ix{0};

        decltype(curr_status) compound_initial_status{0}; // status that begin with 0
        trans_empty_on(compound_initial_status); // expand the transition

        queue.push(compound_initial_status); // initial the bfs queue
        compound_ix_map.insert(std::make_pair(compound_initial_status, top_status_ix++)); // status code 0

        while (!queue.empty()) {
            auto top = std::move(queue.front());
            queue.pop();
            auto top_dfa_ix = compound_ix_map.find(top)->second;
            for (auto input : registered_input) {
                auto next_compound = trans_on_on(top, input);
                auto next_compound_it = compound_ix_map.find(next_compound);
                status_type next_dfa_ix;
                if (next_compound_it == compound_ix_map.end()) {
                    queue.push(next_compound);
                    compound_ix_map.insert(std::make_pair(next_compound, top_status_ix++));
                    next_dfa_ix = top_status_ix - 1;
                    dfa_trans.emplace_back();
                } else {
                    next_dfa_ix = next_compound_it->second;
                }
                dfa_trans.at(top_dfa_ix).insert(std::make_pair(input, next_dfa_ix));
            }
        }

        dfa ret_dfa{dfa_trans.size(), 0};
        for (std::size_t ix = 0; ix < dfa_trans.size(); ++ix) {
            for (auto [input, ns] : dfa_trans.at(ix)) {
                ret_dfa.add_trans(ix, ns, input);
            }
        }
        for (auto &[compound_status, dfa_status] : compound_ix_map) {
            if (std::any_of(compound_status.begin(), compound_status.end(),
                            [&](const auto &item) {
                                return accept_status.at(item);
                            })) {
                ret_dfa.add_accept(dfa_status);
            }
        }

        return ret_dfa;
    }

}