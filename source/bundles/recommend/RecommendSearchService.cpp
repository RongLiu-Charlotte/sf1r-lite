#include "RecommendSearchService.h"
#include "RecommendBundleConfiguration.h"
#include <recommend-manager/common/User.h>
#include <recommend-manager/common/ItemCondition.h>
#include <recommend-manager/common/RecommendParam.h>
#include <recommend-manager/common/TIBParam.h>
#include <recommend-manager/common/ItemBundle.h>
#include <recommend-manager/item/ItemManager.h>
#include <recommend-manager/item/ItemIdGenerator.h>
#include <recommend-manager/item/ItemContainer.h>
#include <recommend-manager/storage/UserManager.h>
#include <recommend-manager/recommender/RecommenderFactory.h>
#include <recommend-manager/recommender/Recommender.h>
#include <bundles/index/IndexSearchService.h>
#include <aggregator-manager/SearchWorker.h>
#include <search-manager/SearchManager.h>

#include <glog/logging.h>

namespace
{
const std::string DOCID("DOCID");
}

namespace sf1r
{

RecommendSearchService::RecommendSearchService(
    RecommendBundleConfiguration& bundleConfig,
    UserManager& userManager,
    ItemManager& itemManager,
    RecommenderFactory& recommenderFactory,
    ItemIdGenerator& itemIdGenerator,
    IndexSearchService* indexSearchService
)
    :bundleConfig_(bundleConfig)
    ,userManager_(userManager)
    ,itemManager_(itemManager)
    ,recommenderFactory_(recommenderFactory)
    ,itemIdGenerator_(itemIdGenerator)
    ,queryBuilder_(NULL)
{
    if (indexSearchService)
    {
        queryBuilder_ = indexSearchService->searchWorker_->
            searchManager_->getQueryBuilder();
    }
}

bool RecommendSearchService::getUser(const std::string& userId, User& user)
{
    return userManager_.getUser(userId, user);
}

bool RecommendSearchService::recommend(
    RecommendParam& param,
    std::vector<RecommendItem>& recItemVec
)
{
    if (!convertIds_(param))
        return false;

    if (bundleConfig_.searchNodeConfig_.isSingleNode_)
        param.enableItemCondition(&itemManager_, queryBuilder_);

    Recommender* recommender = recommenderFactory_.getRecommender(param.type);

    if (recommender && recommender->recommend(param, recItemVec))
    {
        RecommendItemContainer itemContainer(recItemVec);
        itemManager_.getItemProps(param.selectRecommendProps, itemContainer);

        // BAB need not reason results
        if (param.type != BUY_ALSO_BUY)
        {
            getReasonItems_(param.selectReasonProps, recItemVec);
        }

        return true;
    }

    return false;
}

bool RecommendSearchService::convertIds_(RecommendParam& param)
{
    return convertItemId_(param.inputItems, param.inputParam.inputItemIds) &&
           convertItemId_(param.includeItems, param.includeItemIds) &&
           convertItemId_(param.excludeItems, param.excludeItemIds);
}

bool RecommendSearchService::convertItemId_(
    const std::vector<std::string>& inputItemVec,
    std::vector<itemid_t>& outputItemVec
)
{
    itemid_t itemId = 0;

    for (std::size_t i = 0; i < inputItemVec.size(); ++i)
    {
        if (itemIdGenerator_.strIdToItemId(inputItemVec[i], itemId))
        {
            outputItemVec.push_back(itemId);
        }
    }

    return true;
}

void RecommendSearchService::getReasonItems_(
    const std::vector<std::string>& selectProps,
    std::vector<RecommendItem>& recItemVec
)
{
    for (std::vector<RecommendItem>::iterator recIt = recItemVec.begin();
        recIt != recItemVec.end(); ++recIt)
    {
        // empty doc means it has been removed
        if (recIt->item_.isEmpty())
            continue;

        ReasonItemContainer itemContainer(recIt->reasonItems_);
        itemManager_.getItemProps(selectProps, itemContainer);
    }
}

bool RecommendSearchService::topItemBundle(
    const TIBParam& param,
    std::vector<ItemBundle>& bundleVec
)
{
    TIBRecommender* recommender = recommenderFactory_.getTIBRecommender();
    if (recommender && recommender->recommend(param, bundleVec))
    {
        getBundleItems_(param.selectRecommendProps, bundleVec);
        return true;
    }

    return false;
}

void RecommendSearchService::getBundleItems_(
    const std::vector<std::string>& selectProps,
    std::vector<ItemBundle>& bundleVec
)
{
    for (std::vector<ItemBundle>::iterator bundleIt = bundleVec.begin();
        bundleIt != bundleVec.end(); ++bundleIt)
    {
        MultiItemContainer itemContainer(bundleIt->items);
        itemManager_.getItemProps(selectProps, itemContainer);
    }
}

RecommendType RecommendSearchService::getRecommendType(const std::string& typeStr) const
{
    return recommenderFactory_.getRecommendType(typeStr);
}

} // namespace sf1r
