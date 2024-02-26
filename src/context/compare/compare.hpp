#pragma once

#include "compare.h"

namespace pppu
{

/// @brief Perform logical operation negation.
/// @param ctx The calculation settings
/// @param in The input Value object
/// @return The negtive input Value object
template <typename Value>
Value logical_not(Context* ctx, Value const& in)
{
    Value ki_1 = make_constant<Value>(ctx, 1, in.shape());
    return sub(ctx, ki_1, in);
}

/// @brief Perform logical operation and.
/// @param ctx The calculation settings
/// @param lhs The first participant Value in the calculation
/// @param rhs The second participant Value in the calculation
/// @return The and operation result of lhs and rhs
template <typename Value>
Value logical_and(Context* ctx, Value const& lhs, Value const& rhs)
{
    return mul(ctx, lhs, rhs);
}

/// @brief Perform logical operation or.
/// @param ctx The calculation settings
/// @param lhs The first participant Value in the calculation
/// @param rhs The second participant Value in the calculation
/// @return The or operation result of lhs and rhs
template <typename Value>
Value logical_or(Context* ctx, Value const& lhs, Value const& rhs)
{
    Value n_lhs = logical_not(ctx, lhs);
    Value n_rhs = logical_not(ctx, rhs);
    Value n_ans = logical_and(ctx, n_lhs, n_rhs);
    Value ans   = logical_not(ctx, n_ans);
    return ans;
}

/// @brief Used to get two mutually exclusive parts of two Value.
/// @details Used to get two mutually exclusive parts of two Value, where the first participant's 
///        part is the corresponding part in cond's corresponding position which is 1, and the 
///        second participant's part is the part in cond's corresponding position which is 0. 
///        Finally, the combination of mutually exclusive parts between the two participants
///        will be returned. (cond must be either 0 or 1)
/// @param ctx The calculation settings
/// @param cond The conditional Value in the calculation
/// @param v0 The first participant Value in the calculation
/// @param v1 The second participant Value in the calculation
/// @return The result, cond == 0 ? v0 : v1
template <typename Value>
Value conditional(Context* ctx, Value const& cond, Value const& v0, Value const& v1)
{
    Value ans = add(ctx, v0, mul(ctx, cond, sub(ctx, v1, v0)));
    return ans;
}

/// @brief Get the indicator vector of lesser value of corresponding position of the two inputs.
/// @details Judge one by one whether the value of the corresponding position of the first 
///        participant is less than the value of the corresponding position of the second 
///        participant. If it is less than, the result of the corresponding position will 
///        be set to 1, and vice versa, will be set to 0 (The shape needs to be the same)
/// @param ctx The calculation settings
/// @param lhs The first participant Value in the calculation
/// @param rhs The second participant Value in the calculation
/// @return The less operation result of lhs and rhs.
template <typename Value>
Value less(Context* ctx, Value const& lhs, Value const& rhs)
{
    Value diff = sub(ctx, lhs, rhs);
    Value lt   = msb(ctx, diff);
    return lt;
}

/// @brief Get the indicator vector of greater value of corresponding position of the two inputs.
/// @details Judge one by one whether the value of the corresponding position of the first 
///        participant is greater than the value of the corresponding position of the second 
///        participant. If it is greater than, the result of the corresponding position will 
///        be set to 1, and vice versa, will be set to 0 (The shape needs to be the same)
/// @param ctx The calculation settings
/// @param lhs The first participant Value in the calculation
/// @param rhs The second participant Value in the calculation
/// @return The greater operation result of lhs and rhs
template <typename Value>
Value greater(Context* ctx, Value const& lhs, Value const& rhs)
{
    return less(ctx, rhs, lhs);
}

/// @brief Get the indicator vector of lesser than or equal to value of corresponding position of the two inputs.
/// @details Judge one by one whether the value of the corresponding position of the first participant 
///        is less than or equal to the value of the corresponding position of the second 
///        participant. If it is less than or equal to, the result of the corresponding position 
///        will be set to 1, and vice versa, will be set to 0 (The shape needs to be the same)
/// @param ctx The calculation settings
/// @param lhs The first participant Value in the calculation
/// @param rhs The second participant Value in the calculation
/// @return The less_equal operation result of lhs and rhs
template <typename Value>
Value less_equal(Context* ctx, Value const& lhs, Value const& rhs)
{
    Value gt = greater(ctx, lhs, rhs);
    Value le = logical_not(ctx, gt);
    return le;
}

/// @brief Get the indicator vector of greater than or equal to value of corresponding position of the two inputs.
/// @details Judge one by one whether the value of the corresponding position of the first participant 
///        is greater than or equal to the value of the corresponding position of the second 
///        participant. If it is greater than or equal to, the result of the corresponding position 
///        will be set to 1, and vice versa, will be set to 0 (The shape needs to be the same)
/// @param ctx The calculation settings
/// @param lhs The first participant Value in the calculation
/// @param rhs The second participant Value in the calculation
/// @return The greater_equal operation result of lhs and rhs
template <typename Value>
Value greater_equal(Context* ctx, Value const& lhs, Value const& rhs)
{
    Value lt = less(ctx, lhs, rhs);
    Value ge = logical_not(ctx, lt);
    return ge;
}

/// @brief Judge one's corresponding position is equal to anothor.
/// @details Judge one by one whether the value of the corresponding position of the first 
///        participant is equal to the value of the corresponding position of the second 
///        participant. If it is equal to, the result of the corresponding position will 
///        be set to 1, and vice versa, will be set to 0 (The shape needs to be the same)
/// @param ctx The calculation settings
/// @param lhs The first participant Value in the calculation
/// @param rhs The second participant Value in the calculation
/// @return The equal_to operation result of lhs and rhs
template <typename Value>
Value equal_to(Context* ctx, Value const& lhs, Value const& rhs)
{
    Value diff = sub(ctx, lhs, rhs);
    Value eq   = eqz(ctx, diff);
    return eq;
}

/// @brief Judge one's corresponding position is not equal to anothor.
/// @details Judge one by one whether the value of the corresponding position of the first 
///        participant is not equal to the value of the corresponding position of the second 
///        participant. If it is not equal to, the result of the corresponding position will 
///        be set to 1, and vice versa, will be set to 0 (The shape needs to be the same)
/// @param ctx The calculation settings
/// @param lhs The first participant Value in the calculation
/// @param rhs The second participant Value in the calculation
/// @return The not_equal_to operation result of lhs and rhs
template <typename Value>
Value not_equal_to(Context* ctx, Value const& lhs, Value const& rhs)
{
    Value eq  = equal_to(ctx, lhs, rhs);
    Value neq = logical_not(ctx, eq);
    return neq;
}

/// @brief Get the lesser value of corresponding position of the two inputs.
/// @details Judge one by one whether the value of the corresponding position of the first
///        participant is less than the value of the corresponding position of the second
///        participant. If it is less than, the result of the corresponding position will
///        be set to the value of the corresponding position of the first participant, and
///        vice versa, will be set to the value of the corresponding position of the second
///        participant (The shape needs to be the same)
/// @param ctx The calculation settings
/// @param lhs The first participant Value in the calculation
/// @param rhs The second participant Value in the calculation
/// @return The min operation result of lhs and rhs
template <typename Value>
Value min(Context* ctx, Value const& lhs, Value const& rhs)
{
    Value gt  = greater(ctx, lhs, rhs);
    Value min = conditional(ctx, gt, lhs, rhs);
    return min;
}

/// @brief Get the greater value of corresponding position of the two inputs.
/// @details Judge one by one whether the value of the corresponding position of the first
///        participant is greater than the value of the corresponding position of the second
///        participant. If it is greater than, the result of the corresponding position will
///        be set to the value of the corresponding position of the first participant, and
///        vice versa, will be set to the value of the corresponding position of the second
///        participant (The shape needs to be the same)
/// @param ctx The calculation settings
/// @param lhs The first participant Value in the calculation
/// @param rhs The second participant Value in the calculation
/// @return The max operation result of lhs and rhs
template <typename Value>
Value max(Context* ctx, Value const& lhs, Value const& rhs)
{
    Value lt  = less(ctx, lhs, rhs);
    Value max = conditional(ctx, lt, lhs, rhs);
    return max;
}

} // namespace pppu
