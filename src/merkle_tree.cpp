#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <boost/array.hpp>
#include <boost/algorithm/hex.hpp>
#include <sodium.h>

#include "merkle_tree.hpp"


std::string calc_top_hash(const std::vector<std::string>& hashes) {
    /* Convert vector to queue for pop_pront funcs */
    std::queue<std::string> q(std::deque<std::string>(hashes.begin(), hashes.end()));
    
    std::size_t even_size;
    while ((even_size=q.size()-(q.size()%2)) > 1) {
        std::queue<std::string> tmp_q;
        for (int i = 0; i < even_size; i+=2) {
            std::string a, b;
            a = q.front();
            q.pop();
            b = q.front();
            q.pop();

            std::string comb;
            boost::array<uint8_t, crypto_hash_sha256_BYTES> out;
            /* Comb is in hex, 1 byte = 2 char representation in hex */
            comb.reserve(crypto_hash_sha256_BYTES * 2);

            /* Do multipart sha256 hashing */
            crypto_hash_sha256_state sha256_state;
            crypto_hash_sha256_init(&sha256_state);

            crypto_hash_sha256_update(&sha256_state, (const uint8_t*)a.data(), a.size());
            crypto_hash_sha256_update(&sha256_state, (const uint8_t*)b.data(), b.size());

            crypto_hash_sha256_final(&sha256_state, &out[0]);

            /* Convert to hex */
            boost::algorithm::hex(out.begin(), out.end(), std::back_inserter(comb));

            tmp_q.push(comb);
        }
        if ((q.size() % 2) == 1) {tmp_q.push(q.back());}
        /* Overwrite */
        q = tmp_q;
    }

    return q.front();
    
}