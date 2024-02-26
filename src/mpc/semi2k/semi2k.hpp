#pragma once

#include <cstdlib>

#include "mpc/protocol.hpp"
#include "mpc/preprocessing.hpp"

#include "../../ndarray/array_ref.hpp"
#include "../../ndarray/ndarray_ref.hpp"
#include "../../ndarray/operations.hpp"
#include "../../network/multi_party_player.hpp"
#include "../../network/playerid.h"
#include "../../network/multi_party_player.hpp"
#include "../../datatypes/Z2k.hpp"
#include "../../ndarray/tools.hpp"
#include "../../serialization/stl.h"
#include <map>

namespace mpc{

constexpr int MAX_K = 66;

using core::ArrayRef, core::NDArrayRef;

/// @class Semi2kTripl
/// @brief Multiplication triplet used in Semi2k protocol.
class Semi2kTriple: public mpc::Preprocessing{
public:
    Semi2kTriple() = default;
    ~Semi2kTriple() = default;

    /// @brief Get n triples for Semi2k protocol.
    /// @param n How many triples we need
    template<size_t K, bool Signed>
    std::tuple<
        ArrayRef<Z2<K, Signed>>,
        ArrayRef<Z2<K, Signed>>,
        ArrayRef<Z2<K, Signed>>
    > get_n_triple(size_t n)
    {
        return std::make_tuple(
            core::make_array(Z2<K, Signed>{0}, n, true),
            core::make_array(Z2<K, Signed>{0}, n, true),
            core::make_array(Z2<K, Signed>{0}, n, true)
        );
    }

    /// @brief Get matrix bever triple for Semi2k protocol.
    /// @param M The first parameter
    /// @param N The second parameter
    /// @param KK The third parameter
    /// @return Matrix bever triple
    template<size_t K, bool Signed>
    std::tuple<
        ArrayRef<Z2<K, Signed>>,
        ArrayRef<Z2<K, Signed>>,
        ArrayRef<Z2<K, Signed>>
    > get_matrix_triple(int64_t M, int64_t N, int64_t KK)
    {
        return std::make_tuple(
            core::make_array(Z2<K, Signed>{0}, M * N, true),
            core::make_array(Z2<K, Signed>{0}, N * KK, true),
            core::make_array(Z2<K, Signed>{0}, M * KK , true)
        );
    }

    /// @brief Try to get n randbits. Return true if successful, false otherwise
    /// @param n How many randbits we need
    /// @return n randbits
    template<size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> get_n_randbit(size_t n)
    {
        return core::make_array(Z2<K, Signed>{0}, n, true);
    }

    /// @brief Get a pair of randbits.
    /// @param num How many randbits we need
    /// @return num pair of randbits
    template<size_t K, bool Signed>
    auto get_r_and_rr(int64_t num)
    {
        struct r_and_rr{
            ArrayRef<Z2<K, Signed>> r;
            ArrayRef<Z2<K, Signed>> rr;
        };
        return r_and_rr{core::flatten(core::zeros<Z2<K, Signed>>(std::vector<int64_t>{num})), core::flatten(core::zeros<Z2<K, Signed>>(std::vector<int64_t>{num}))};
    }

};

class Semi2k;

/// @struct isValidProtocolPlainType
/// @brief Check whether ptype is a valid plain type of protocol.
template <std::size_t K, bool S>
struct isValidProtocolPlainType< Semi2k, Z2<K,S> >: std::true_type {};

/// @struct convertProtocolPlainTypeToShareType
/// @brief Convert plain type to corresponding share type with member type "type".
template <typename ptype>
struct convertProtocolPlainTypeToShareType< Semi2k, ptype > { using type = ptype; };

/// @struct isValidProtocolPlainSharePair
/// @brief Check whether ptype and stype is match.
template <std::size_t K, bool S>
struct isValidProtocolPlainSharePair< Semi2k, Z2<K,S>, Z2<K,S> >: std::true_type {};

/// @class Semi2k
/// @brief A implementation of secure multi-party computation with protocol Semi2k.
class Semi2k: public mpc::Protocol {

public:

    using Preprocessing = Semi2kTriple;

    // commonly used plain type
    using pbit    = SignedZ2<1>;
    using pint8   = SignedZ2<8>;
    using pint16  = SignedZ2<16>;
    using pint32  = SignedZ2<32>;
    using pint64  = SignedZ2<64>;
    using pint128 = SignedZ2<128>;
    using pint192 = SignedZ2<192>;
    using pint256 = SignedZ2<256>;

    /// @brief Constructor for overall settings.
    /// @param netio Used network communication settings
    /// @param triples Multiplication triplet used in Semi2k protocol
    Semi2k(network::MultiPartyPlayer* netio, Semi2kTriple* triples=nullptr):
        playerid(netio->id()), n_players(netio->num_players()), mplayer(netio), triples(triples), parties(mplayer->all_but_me()) {}

    /// @brief Constructor for single player.
    /// @param playerid Player's id, from 0 to n_players - 1
    /// @param n_players Number of players.
    /// @param mplayer Implementation of features of multi player communication
    /// @param triples Multiplication triplet used in Semi2k protocol
    Semi2k(playerid_t playerid, size_t n_players, network::MultiPartyPlayer* mplayer, Semi2kTriple* triples = nullptr): 
        playerid(playerid), n_players(n_players), mplayer(mplayer), triples(triples), parties(mplayer->all_but_me()){};

    /// @brief Implementation of remote input for plain input from pid under the Semi2k protocol.
    /// @param pid Input from which player
    /// @param numel Number of input elements
    /// @return ArrayRef object of input data
    template <typename pdtype>
    ArrayRef<pdtype> input_p(playerid_t pid, int64_t numel)
    {
        static_assert( isValidProtocolPlainType<Semi2k, pdtype>::value, "invalid pdtype" );
        return core::make_array(pdtype{0}, numel, true);
    }

    /// @brief Implementation of local private input for plain input under the Semi2k protocol.
    /// @param in Local private input
    /// @return ArrayRef object of input data
    template <std::size_t K, bool S>
    ArrayRef<Z2<K,S>> input_p( ArrayRef<Z2<K,S>> const& in )
    {
        return in;
    }

    /// @brief Implementation of open share to each party for share input under the Semi2k protocol.
    /// @param in Share input
    /// @return ArrayRef object of input data
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> open_s(ArrayRef<Z2<K, Signed>> const& in)
    {
        ArrayRef<Z2<K, Signed>> ret(in);
        Serializer sr;
        sr << in;
        auto msgs = mplayer->mbroadcast_recv(parties, sr.finalize());
        ArrayRef<Z2<K, Signed>> tmp(in);
        for(const auto& pid: parties){
            Deserializer dr(std::move(msgs[pid]));
            dr >> tmp;
            ret = add_pp(ret, tmp);
        }
        return ret;
    }

    /// @brief Implementation of negation for plain input under the Semi2k protocol.
    /// @param in Plain input
    /// @return ArrayRef object of the result
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> neg_p(ArrayRef<Z2<K, Signed>> const& in)
    {
        return core::neg(in);
    }

    /// @brief Implementation of negation for share input under the Semi2k protocol.
    /// @param in Plain input
    /// @return ArrayRef object of the result
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> neg_s(ArrayRef<Z2<K, Signed>> const& in)
    {
        return core::neg(in);
    }

    /// @brief Implementation of addition between plain addend and plain addend under the Semi2k protocol.
    /// @param lhs First plain addend
    /// @param rhs Second plain addend
    /// @return ArrayRef object of the sum, lhs + rhs
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> add_pp(ArrayRef<Z2<K, Signed>> const& lhs, ArrayRef<Z2<K, Signed>> const& rhs)
    {
        return core::add(lhs, rhs);
    }

    /// @brief Implementation of addition between share addend and plain addend under the Semi2k protocol.
    /// @param lhs First share addend
    /// @param rhs Second plain addend
    /// @return ArrayRef object of the sum, lhs + rhs
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> add_sp(ArrayRef<Z2<K, Signed>> const& share, ArrayRef<Z2<K, Signed>> const& plain)
    {
        if(playerid == 0) 
        {
            return core::add(share, plain);
        }
        else 
        {
            return share;
        }
    }
    
    /// @brief Implementation of addition between share addend and share addend under the Semi2k protocol.
    /// @param lhs First share addend
    /// @param rhs Second share addend
    /// @return ArrayRef object of the sum, lhs + rhs
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> add_ss(ArrayRef<Z2<K, Signed>> const& lhs, ArrayRef<Z2<K, Signed>> const& rhs)
    {
        return core::add(lhs, rhs);
    }

    /// @brief Implementation of multiplication between plain multiplier and plain multiplier under the Semi2k protocol.
    /// @param lhs First plain multiplier
    /// @param rhs Second plain multiplier
    /// @return ArrayRef object of the product, lhs * rhs
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> mul_pp(ArrayRef<Z2<K, Signed>> const& lhs, ArrayRef<Z2<K, Signed>> const& rhs)
    {
        return core::mul(lhs, rhs);
    }

    /// @brief Implementation of multiplication between share multiplier and plain multiplier under the Semi2k protocol.
    /// @param lhs First share multiplier
    /// @param rhs Second plain multiplier
    /// @return ArrayRef object of the product, lhs * rhs
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> mul_sp(ArrayRef<Z2<K, Signed>> const& share, ArrayRef<Z2<K, Signed>> const& plain)
    {
        return core::mul(share, plain);
    }

    /// @brief Implementation of multiplication between share multiplier and share multiplier under the Semi2k protocol.
    /// @param lhs First share multiplier
    /// @param rhs Second share multiplier
    /// @return ArrayRef object of the product, lhs * rhs
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> mul_ss(ArrayRef<Z2<K, Signed>> const& lhs, ArrayRef<Z2<K, Signed>> const& rhs)
    {
        // ArrayRef<Z2<K, Signed>> us, vs, uvs;
        if(!triples)
        {
            throw std::runtime_error("Triples are not enough. ");
        }
        auto [us, vs, uvs] = triples->get_n_triple<K, Signed>(lhs.numel());
        auto a_u = add_pp(lhs, neg_p(us));
        auto b_v = add_pp(rhs, neg_p(vs));
        auto p_a_u = open_s(a_u);
        auto p_b_v = open_s(b_v);
        auto ret = add_ss(
                    add_sp(
                        add_ss(
                            mul_sp(us, p_b_v), 
                            mul_sp(vs, p_a_u)), 
                        mul_pp(p_a_u, p_b_v)), 
                    uvs);
        return ret;
    }

    /// @brief Implementation of the most significant bit for share input under the Semi2k protocol.
    /// @param in Share input
    /// @return ArrayRef object of the result, x < 0 -> 1, x >= 0 -> 0
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> msb_s(ArrayRef<Z2<K, Signed>> const& in)
    {

        if(Signed == true){
            ArrayRef<Z2<K, false>> unsigned_in = core::apply([](auto const& x){return Z2<K, false>(x);}, in);
            ArrayRef<Z2<K, false>> unsigned_res = msb_s(unsigned_in);
            ArrayRef<Z2<K, Signed>> signed_res = core::apply([](auto const& x){return Z2<K, Signed>(x);}, unsigned_res);
            return signed_res;
        }

        // Setp 1
        if(!triples)
        {
            throw std::runtime_error("Randbits are not enough. ");
        }
        auto b = triples->get_n_randbit<K, Signed>(in.numel());
        std::vector<ArrayRef<Z2<K, Signed>>> rs;
        for(int i = 0; i != K; ++i){
            rs.emplace_back(triples->get_n_randbit<K, Signed>(in.numel()));
        }
        ArrayRef<Z2<K, Signed>> r = core::make_array(std::vector<Z2<K, Signed>>(in.numel(), 0));
        for(int i = 0; i != in.numel(); ++i)
        {
            for(int j = 0; j != K; ++j)
            {
                r[i] = r[i] + (rs[j][i] << j);
            }
        }

        // Setp 2
        ArrayRef<Z2<K, Signed>> c = open_s(add_ss(in, r));

        // Setp 3
        ArrayRef<Z2<K, Signed>> cc(c);
        cc = rshift_p(lshift_p(c, 1), 1);
        ArrayRef<Z2<K, Signed>> rr = core::make_array(std::vector<Z2<K, Signed>>(in.numel(), 0));
        for(int i = 0; i != in.numel(); ++i)
        {
            for(int j = 0; j != K - 1; ++j)
            {
                rr[i] = rr[i] + (rs[j][i] << j);
            }
        }

        // Setp 4
        std::vector<ArrayRef<Z2<1, Signed>>> r2s = a2b(rs);
        
        // Setp 5
        ArrayRef<Z2<1, Signed>> u2 = bitlt_ps(cc, r2s);

        // Setp 6
        std::vector<ArrayRef<Z2<1, Signed>>> tmp;
        tmp.emplace_back(u2);
        ArrayRef<Z2<K, Signed>> u = b2a<K>(tmp)[0];

        // Setp 7
        ArrayRef<Z2<K, Signed>> aa = add_sp(
                                        add_ss(
                                            neg_s(rr), 
                                            lshift_s(u, K - 1)), 
                                        cc);
        ArrayRef<Z2<K, Signed>> d = add_ss(in, neg_s(aa));

        // Setp 8
        ArrayRef<Z2<K, Signed>> e = open_s(add_ss(
                                        lshift_s(b, K - 1), 
                                        d));
        ArrayRef<Z2<K, Signed>> e_msb = msb_p(e);
        
        // Setp 9
        auto tmpp = add_sp(
                        add_ss(
                            add_ss(
                                neg_s(mul_sp(b, e_msb)), 
                                neg_s(mul_sp(b, e_msb))), 
                            b), 
                        e_msb);
        
        return add_sp(neg_s(tmpp), core::make_array(std::vector<Z2<K, Signed>>(tmpp.numel(), 1)));
    }

    /// @brief Implementation of the most significant bit for plain input under the Semi2k protocol.
    /// @param in Plain input
    /// @return ArrayRef object of the result, x < 0 -> 1, x >= 0 -> 0
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> msb_p(ArrayRef<Z2<K, Signed>> const& in){
        return core::apply([](auto const& x){return Z2<K, Signed>(x.msb());}, in);
    }

    /// @brief Implementation of the left bit shift for plain input under the Semi2k protocol.
    /// @param in Plain input
    /// @param nbits Binary bits to shift
    /// @return ArrayRef object of the result, x << nbits
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> lshift_p(ArrayRef<Z2<K, Signed>> const& in, std::size_t nbits)
    {
        return core::apply([nbits](auto const& x){return x << nbits;}, in);
    }

    /// @brief Implementation of the left bit shift for share input under the Semi2k protocol.
    /// @param in Share input
    /// @param nbits Binary bits to shift
    /// @return ArrayRef object of the result, x << nbits
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> lshift_s(ArrayRef<Z2<K, Signed>> const& in, std::size_t nbits){
        return core::apply([nbits](auto const& x){return x << nbits;}, in);
    }

    /// @brief Implementation of the truncation for plain input under the Semi2k protocol.
    /// @param in Plain input
    /// @param nbits Binary bits to shift
    /// @return ArrayRef object of the result, x >> nbits
    // TODO Repeated, may delete this.
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> rshift_p(ArrayRef<Z2<K, Signed>> const& in, std::size_t nbits)
    {
        return core::apply([nbits](auto const& x){return x >> nbits;}, in);
    }

    /// @brief Implementation of the truncation for plain input under the Semi2k protocol.
    /// @param in Plain input
    /// @param nbits Binary bits to shift
    /// @return ArrayRef object of the result, x >> nbits
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> trunc_p(ArrayRef<Z2<K, Signed>> const& in, std::size_t nbits)
    {
        return core::apply([nbits](auto const& x){return x >> nbits;}, in);
    }

    /// @brief Implementation of the truncation for share input under the Semi2k protocol.
    /// @param in Share input
    /// @param nbits Binary bits to shift
    /// @return ArrayRef object of the result, x >> nbits
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> trunc_s(ArrayRef<Z2<K, Signed>> const& in, std::size_t nbits)
    {
        if( n_players == 2 )
        {
            // 2Party local truncation
            return core::apply([nbits](auto const& x){return x >> nbits;}, in);
        }
        else
        {
            // use general truncation protocol
            if(!triples)
            {
                throw std::runtime_error("Randbits are not enough. ");
            }

            auto r_and_rr = triples->get_r_and_rr<K, Signed>(in.numel());

            auto c = open_s(add_ss(r_and_rr.r ,neg_s(in)));
            auto ret = add_sp(r_and_rr.rr, neg_p(rshift_p(c, nbits)));

            return ret;
        }
    }

    /// @brief Implementation of the equal to zero for plain input under the Semi2k protocol.
    /// @param in Plain input
    /// @return ArrayRef object of the result, x = 0 -> 1, x != 0 -> 0
    template <std::size_t K, bool Signed>
    core::ArrayRef<Z2<K, Signed>> eqz_p(core::ArrayRef<Z2<K, Signed>> const& in)
    {
        return core::apply([](auto const& x){ return Z2<K, Signed>(x == 0); }, in);
    }

    /// @brief Implementation of the equal to zero for share input under the Semi2k protocol.
    /// @param in Share input
    /// @return ArrayRef object of the result, x = 0 -> 1, x != 0 -> 0
    template <std::size_t K, bool Signed>
    core::ArrayRef<Z2<K, Signed>> eqz_s(core::ArrayRef<Z2<K, Signed>> const& in)
    {

        if(Signed == true){
            ArrayRef<Z2<K, false>> unsigned_in = core::apply([](auto const& x){return Z2<K, false>(x);}, in);
            ArrayRef<Z2<K, false>> unsigned_res = eqz_s(unsigned_in);
            ArrayRef<Z2<K, Signed>> signed_res = core::apply([](auto const& x){return Z2<K, Signed>(x);}, unsigned_res);
            return signed_res;
        }

        // Step 1
        std::vector<ArrayRef<Z2<K, Signed>>> rs;
        for(int i = 0; i != K; ++i)
        {
            rs.emplace_back(triples->get_n_randbit<K, Signed>(in.numel()));
        }

        // Step 2
        ArrayRef<Z2<K, Signed>> r = core::make_array(Z2<K, Signed>{0}, in.numel(), true);
        for(int i = 0; i != K; ++i)
        {
            r = add_ss(r, lshift_s(rs[i], i));
        }

        // Step 3
        ArrayRef<Z2<K, Signed>> c = open_s(add_ss(in, r));
        std::vector<ArrayRef<Z2<K, Signed>>> cs = bitdec_p(c, K);

        // Step 4
        std::vector<ArrayRef<Z2<1, Signed>>> rsb = a2b(rs);

        // Step 5
        std::vector<ArrayRef<Z2<1, Signed>>> csb = a2b(cs);
        std::vector<ArrayRef<Z2<1, Signed>>> tmp;
        for(int i = 0; i != K; ++i)
        {
            tmp.emplace_back(add_sp(rsb[i], csb[i]));
        }
        ArrayRef<Z2<1, Signed>> b2 = tmp[0];
        for(int i = 1; i != K; ++i)
        {
            b2 = or_ss(b2, tmp[i]);
        }
        auto ones = core::make_array(std::vector<Z2<1, Signed>>(b2.numel(), 1));
        b2 = add_sp(b2, ones);

        // Step 6
        std::vector<ArrayRef<Z2<1, Signed>>> tmpp;
        tmpp.emplace_back(b2);
        ArrayRef<Z2<K, Signed>> b = b2a<K>(tmpp)[0];
        return b;
    }

    /// @brief Implementation of the bit decomposition for plain input under the Semi2k protocol.
    /// @param in Plain input
    /// @param nbits Maximum decomposed binary bits
    /// @return The decomposed vector of ArrayRef
    template <std::size_t K, bool Signed>
    std::vector<core::ArrayRef<Z2<K, Signed>>> bitdec_p(core::ArrayRef<Z2<K, Signed>> const& in, std::size_t nbits)
    {
        std::vector<core::ArrayRef<Z2<K, Signed>>> ans;
        ArrayRef<Z2<K, Signed>> zero = core::make_array(std::vector<Z2<K, Signed>>(in.numel(), 0));
        for(int i = 0; i < nbits; ++i) {
            if(i < K) {
                ans.emplace_back(
                core::apply(
                    [i](Z2<K, Signed> const& x){ return Z2<K, Signed>(x.bit(i)); },
                    in)
                );
            }
            else {
                if(K == 1) ans.emplace_back(zero);
                else ans.emplace_back(msb_p(in));
            }
        }
        return ans;
    }

    /// @brief Implementation of the bit decomposition for share input under the Semi2k protocol.
    /// @param in Share input
    /// @param nbits Maximum decomposed binary bits
    /// @return The decomposed vector of ArrayRef
    template <std::size_t K, bool Signed>
    std::vector<ArrayRef<Z2<K, Signed>>> bitdec_s(ArrayRef<Z2<K, Signed>> const& in, std::size_t nbits)
    {
        // Step 1
        std::vector<ArrayRef<Z2<K, Signed>>> rs;
        for(int i = 0; i != nbits; ++i)
        {
            rs.emplace_back(triples->get_n_randbit<K, Signed>(in.numel()));
        }
        ArrayRef<Z2<K, Signed>> r = core::make_array(Z2<K, Signed>{0}, in.numel(), true);
        for(int i = 0; i != nbits; ++i){
            r = add_ss(r, lshift_s(rs[i], i));
        }

        // Step 2
        ArrayRef<Z2<K, Signed>> c = open_s(add_ss(in, neg_s(r)));

        // Step 3
        std::vector<ArrayRef<Z2<1, Signed>>> rs2 = a2b(rs);

        // Step 4
        std::vector<ArrayRef<Z2<1, Signed>>> cs2 = a2b(bitdec_p(c, nbits));
        std::vector<ArrayRef<Z2<1, Signed>>> a0sb = add_pb(cs2, rs2);

        // Step 5
        std::vector<ArrayRef<Z2<K, Signed>>> a0s = b2a<K>(a0sb);

        return a0s;

    }

    /// @brief Implementation of the highest bit decomposition for plain input under the Semi2k protocol.
    /// @param in Plain input
    /// @param nbits Maximum decomposed binary bits
    /// @return The decomposed vector of ArrayRef
    template <std::size_t K, bool Signed>
    std::vector<core::ArrayRef<Z2<K, Signed>>> h1bitdec_p(core::ArrayRef<Z2<K, Signed>> const& in, std::size_t nbits)
    {
        auto h1b = core::apply([nbits](Z2<K, Signed> const& x){
            Z2<K, Signed> ans{0};
            char flag = 1;
            for(int i = nbits - 1; i >= 0; --i) {
                char b = x.bit(i);
                ans.bit(i) = b & flag;
                flag &= ~b;
            }
            return ans;
        }, in);
        auto ans = bitdec_p(h1b, nbits);
        return ans;
    }

    /// @brief Implementation of the highest bit decomposition for share input under the Semi2k protocol.
    /// @param in Share input
    /// @param nbits Maximum decomposed binary bits
    /// @return The decomposed vector of ArrayRef
    template <std::size_t K, bool Signed>
    std::vector<core::ArrayRef<Z2<K, Signed>>> h1bitdec_s(core::ArrayRef<Z2<K, Signed>> const& in, std::size_t nbits)
    {

        std::vector<core::ArrayRef<Z2<1, Signed>>> prefix_or_ans_dec = a2b(bitdec_s(in, nbits));

        for(int i = prefix_or_ans_dec.size() - 2; i >= 0; --i)
        {
            prefix_or_ans_dec[i] = or_ss(prefix_or_ans_dec[i], prefix_or_ans_dec[i + 1]);
        }

        ArrayRef<Z2<1, Signed>> ones = core::make_array(std::vector<Z2<1, Signed>>(in.numel(), 1));

        std::vector<ArrayRef<Z2<1, Signed>>> ret = add_pb(bitdec_p(ones, nbits), prefix_or_ans_dec, true);

        ret.erase(ret.begin());
  
        std::vector<ArrayRef<Z2<K, Signed>>> rett;
        std::vector<Z2<K, Signed>> tmp(ret[0].numel());
        for(int i = 0; i != ret.size(); ++i){
            for(int j = 0; j != tmp.size(); ++j){
                tmp[j] = static_cast<Z2<K, Signed>>(ret[i][j]);
            }
            rett.emplace_back(core::make_array(tmp));
        }

        return rett;
    }

    /// @brief Implementation of matrix multiplication between plain matrix multiplier and plain matrix multiplier under the Semi2k protocol.
    /// @param lhs First plain matrix multiplier
    /// @param rhs Second plain matrix multiplier
    /// @return ArrayRef object of the matrix product, (lhsrhs)
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> matmul_pp(ArrayRef<Z2<K, Signed>> const& lhs, ArrayRef<Z2<K, Signed>> const& rhs, int64_t M, int64_t N, int64_t KK){
        return core::matmul(lhs, rhs, M, N, KK);
    }

    /// @brief Implementation of matrix multiplication between share matrix multiplier and plain matrix multiplier under the Semi2k protocol.
    /// @param lhs First share matrix multiplier
    /// @param rhs Second plain matrix multiplier
    /// @return ArrayRef object of the matrix product, (lhsrhs)
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> matmul_sp(ArrayRef<Z2<K, Signed>> const& lhs, ArrayRef<Z2<K, Signed>> const& rhs, int64_t M, int64_t N, int64_t KK){
        return core::matmul(lhs, rhs, M, N, KK);
    }

    /// @brief Implementation of matrix multiplication between plain matrix multiplier and share matrix multiplier under the Semi2k protocol.
    /// @param lhs First plain matrix multiplier
    /// @param rhs Second share matrix multiplier
    /// @return ArrayRef object of the matrix product, (lhsrhs)
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> matmul_ps(ArrayRef<Z2<K, Signed>> const& lhs, ArrayRef<Z2<K, Signed>> const& rhs, int64_t M, int64_t N, int64_t KK){
        return core::matmul(lhs, rhs, M, N, KK);
    }

    /// @brief Implementation of matrix multiplication between share matrix multiplier and share matrix multiplier under the Semi2k protocol.
    /// @param lhs First share matrix multiplier
    /// @param rhs Second share matrix multiplier
    /// @return ArrayRef object of the matrix product, (lhsrhs)
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<K, Signed>> matmul_ss(ArrayRef<Z2<K, Signed>> const& lhs, ArrayRef<Z2<K, Signed>> const& rhs, int64_t M, int64_t N, int64_t KK){
        auto [us, vs, uvs] = triples->get_matrix_triple<K, Signed>(M, N, KK);

        auto a_u = add_pp(lhs, neg_p(us));
        auto b_v = add_pp(rhs, neg_p(vs));
        auto p_a_u = open_s(a_u);
        auto p_b_v = open_s(b_v);
        auto ret = add_ss(
                    add_sp(
                        add_ss(
                            matmul_sp(us, p_b_v, M, N, KK), 
                            matmul_ps(p_a_u, vs, M, N, KK)), 
                        matmul_pp(p_a_u, p_b_v, M, N, KK)), 
                    uvs);
        return ret;
    }

private:
    /// @brief Used in most significant bit function to calculate intermediate variable.
    /// @param lhs First plain input
    /// @param rhs Second share input
    /// @return Intermediate variable ArrayRef
    template <std::size_t K, bool Signed>
    ArrayRef<Z2<1, Signed>> bitlt_ps(const ArrayRef<Z2<K, Signed>>& lhs, const std::vector<ArrayRef<Z2<1, Signed>>>& rhs)
    {
        std::vector<ArrayRef<Z2<1, Signed>>> bb;
        auto fn = [](auto const& x){return x + 1;};
        for(int i = 0; i != rhs.size(); ++i)
        {
            bb.emplace_back(core::apply(fn, rhs[i]));
        }

        std::vector<ArrayRef<Z2<K, Signed>>> a = bitdec_p(lhs, K);

        std::vector<ArrayRef<Z2<1, Signed>>> aa;
        std::vector<Z2<1, Signed>> tmp(a[0].numel());
        for(int i = 0; i != a.size(); ++i){
            for(int j = 0; j != tmp.size(); ++j){
                tmp[j] = static_cast<Z2<1, Signed>>(a[i][j]);
            }
            aa.emplace_back(core::make_array(tmp));
        }

        if(playerid == 0)
        {
            ArrayRef<Z2<1, Signed>> c = core::make_array(std::vector<Z2<1, Signed>>(lhs.numel(), 1));

            return carry_pss(aa, bb, c);
        }
        else 
        {
            ArrayRef<Z2<1, Signed>> c = core::make_array(std::vector<Z2<1, Signed>>(lhs.numel(), 0));
            return carry_pss(aa, bb, c);
        }
    }

    /// @brief Convert Z2's bit size from K to 1
    /// @param in Input to be converted
    /// @return Converted Output
    template <std::size_t K, bool Signed>
    std::vector<ArrayRef<Z2<1, Signed>>> a2b(const std::vector<ArrayRef<Z2<K, Signed>>>& in)
    {
        std::vector<ArrayRef<Z2<1, Signed>>> ret;
        for(const auto& e: in)
        {
            ret.emplace_back(
                    core::apply(
                        [e](Z2<K, Signed> const& x){return Z2<1, Signed>(x.bit(0));}, 
                        e)
                );
        }
        return ret;
    }

    /// @brief Convert Z2's bit size from 1 to K
    /// @param in Input to be converted
    /// @return Converted Output
    template <std::size_t K, bool Signed>
    std::vector<ArrayRef<Z2<K, Signed>>> b2a(const std::vector<ArrayRef<Z2<1, Signed>>>& in)
    {
        std::vector<ArrayRef<Z2<K, Signed>>> r;
        for(int i = 0; i != in.size(); ++i)
        {
            r.emplace_back(triples->get_n_randbit<K, Signed>(in[0].numel()));
        }
        std::vector<ArrayRef<Z2<1, Signed>>> r2 = a2b(r);
        std::vector<ArrayRef<Z2<K, Signed>>> c;
        for(int i = 0; i != in.size(); ++i)
        {
            std::vector<Z2<K, Signed>> tmp1(in[i].numel());
            ArrayRef<Z2<1, Signed>> tmp2 = open_s(add_ss(in[i], r2[i]));
            for(int i = 0; i != tmp1.size(); ++i){
                tmp1[i] = static_cast<Z2<K, Signed>>(tmp2[i]);
            }
            ArrayRef<Z2<K, Signed>> tmp3 = core::make_array(tmp1);
            c.emplace_back(tmp3);
        }
        std::vector<ArrayRef<Z2<K, Signed>>> ret;
        ArrayRef<Z2<K, Signed>> twos = core::make_array(Z2<K, Signed>{2}, in[0].numel(), true);
        for(int i = 0; i != in.size(); ++i)
        {
            ret.emplace_back(add_sp(
                add_ss(
                    r[i], 
                    neg_s(
                        mul_sp(
                            r[i], 
                            mul_pp(twos, c[i])))), 
                c[i]));
        }

        return ret;
    }

    /// @brief Perform logical operation or between share input and share input.
    /// @param lhs First share input
    /// @param rhs Second share input
    /// @return ArrayRef object of the result, lhs | rhs
    template <bool Signed>
    ArrayRef<Z2<1, Signed>> or_ss(const ArrayRef<Z2<1, Signed>>& lhs, const ArrayRef<Z2<1, Signed>>& rhs)
    {

        ArrayRef<Z2<1, Signed>> ones = core::make_array<Z2<1, Signed>>(std::vector<Z2<1, Signed>>(lhs.numel(), 1));

        return add_sp(mul_ss(
                        add_sp(lhs, ones),
                        add_sp(rhs, ones)), 
                    ones);
    }

    /// @brief Implementation of addition between plain bit addend and plain bit addend under the Semi2k protocol.
    /// @param lhs First plain bit addend
    /// @param rhs Second plain bit addend
    /// @return ArrayRef object of the sum, lhs + rhs
    template <bool Signed>
    std::vector<ArrayRef<Z2<1, Signed>>> add_pb(const std::vector<ArrayRef<Z2<1, Signed>>>& lhs, const std::vector<ArrayRef<Z2<1, Signed>>>& rhs, bool save_carry = false)
    {
        std::vector<ArrayRef<Z2<1, Signed>>> ret;
        ArrayRef<Z2<1, Signed>> c = core::make_array(Z2<1, Signed>{0}, lhs[0].numel(), true);
        for(int i = 0; i != lhs.size(); ++i)
        {
            ret.emplace_back(add_sp(add_ss(rhs[i], c), lhs[i]));
            c = add_ss(mul_ss(add_sp(rhs[i], lhs[i]), c), mul_sp(rhs[i], lhs[i]));
        }
        if(save_carry){
            ret.emplace_back(c);
        }
        return ret;
    }

    /// @brief Used in most significant bit function to calculate intermediate variable.
    /// @param a First plain input
    /// @param b Second share input
    /// @param c Third share input
    /// @return Intermediate variable ArrayRef
    template <bool Signed>
    ArrayRef<Z2<1, Signed>> carry_pss(const std::vector<ArrayRef<Z2<1, Signed>>>& a, const std::vector<ArrayRef<Z2<1, Signed>>>& b, const ArrayRef<Z2<1, Signed>>& c)
    {
        ArrayRef<Z2<1, Signed>> ret = c;
        for(int i = 0; i != a.size(); ++i)
        {
            ret = add_ss(mul_ss(add_sp(a[i], b[i]), ret), mul_sp(b[i], a[i]));
        }
        return ret;
    }

protected:
    playerid_t playerid;
    Semi2kTriple* triples;
    size_t n_players;
    network::MultiPartyPlayer* mplayer;
    mplayerid_t parties;
};

}
