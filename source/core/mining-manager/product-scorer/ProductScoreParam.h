/**
 * @file ProductScoreParam.h
 * @brief the parameters used to create ProductScorer instance.
 * @author Jun Jiang
 * @date Created 2012-12-22
 */

#ifndef SF1R_PRODUCT_SCORE_PARAM_H
#define SF1R_PRODUCT_SCORE_PARAM_H

#include "../group-manager/GroupParam.h"
#include <string>

namespace sf1r
{
class ProductScorer;
class PropSharedLockSet;

struct ProductScoreParam
{
    /** used to create @c CustomScorer and @c CategoryScorer */
    const std::string& query_;

    /** used to create @c CategoryScorer */
    const std::string& querySource_;

    /** used to create @c CategoryScorer */
    const faceted::GroupParam::GroupPathVec& boostGroupLabels_;

    /** for concurrent access on category data by @c CategoryScorer */
    PropSharedLockSet& propSharedLockSet_;

    /** the relevance scorer, it could be NULL if not existed */
    ProductScorer* relevanceScorer_;

    ProductScoreParam(
        const std::string& query,
        const std::string& querySource,
        const faceted::GroupParam::GroupPathVec& boostGroupLabels,
        PropSharedLockSet& propSharedLockSet,
        ProductScorer* relevanceScorer)
        : query_(query)
        , querySource_(querySource)
        , boostGroupLabels_(boostGroupLabels)
        , propSharedLockSet_(propSharedLockSet)
        , relevanceScorer_(relevanceScorer)
    {}
};

} // namespace sf1r

#endif // SF1R_PRODUCT_SCORE_PARAM_H
