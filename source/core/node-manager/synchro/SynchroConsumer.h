/**
 * @file SynchroConsumer.h
 * @author Zhongxia Li
 * @date Oct 23, 2011
 * @brief Distributed data synchronizer component
 */

#ifndef SYNCHRO_CONSUMER_H_
#define SYNCHRO_CONSUMER_H_

#include <node-manager/NodeDef.h>
#include <node-manager/ZooKeeperManager.h>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>


namespace sf1r{

/**
 * Consumer consumes stuff produced by Producer
 */
class SynchroConsumer : public ZooKeeperEventHandler
{
public:
    typedef boost::function< bool( const std::string& dataPath ) > callback_on_produced_t;

    enum ConsumerStatusType
    {
        CONSUMER_STATUS_INIT,
        CONSUMER_STATUS_WATCHING,
        CONSUMER_STATUS_CONSUMING
    };

public:
    SynchroConsumer(
            const std::string& zkHosts,
            int zkTimeout,
            const std::string zkSyncNodePath,
            replicaid_t replicaId,
            nodeid_t nodeId
            );

    void watchProducer(
            const std::string& producerID,
            callback_on_produced_t callback_on_produced,
            bool replyProducer = true);

public:
    virtual void process(ZooKeeperEvent& zkEvent);

    virtual void onDataChanged(const std::string& path);
    virtual void onChildrenChanged(const std::string& path);

    virtual void onMonitor();

private:
    void doWatchProducer();

    void resetWatch();

private:
    replicaid_t replicaId_;
    nodeid_t nodeId_;

    ZooKeeperClientPtr zookeeper_;

    std::string syncNodePath_;
    std::string producerNodePath_;

    ConsumerStatusType consumerStatus_;

    boost::mutex mutex_;
    std::string producerRealNodePath_;
    std::string consumerRealNodePath_;

    callback_on_produced_t callback_on_produced_;
    bool replyProducer_;

    boost::thread thread_;
};

typedef boost::shared_ptr<SynchroConsumer> SynchroConsumerPtr;

}

#endif /* SYNCHRO_CONSUMER_H_ */
