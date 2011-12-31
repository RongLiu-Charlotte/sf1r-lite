/**
 * \file CollectionController.h
 * \brief 
 * \date Dec 20, 2011
 * \author Xin Liu
 */

#ifndef PROCESS_CONTROLLERS_COLLECTION_CONTROLLER_H_
#define PROCESS_CONTROLLERS_COLLECTION_CONTROLLER_H_

#include <util/driver/Controller.h>
#include <common/Keys.h>
#include <util/driver/value/types.h>
#include <string>

namespace sf1r
{
using driver::Keys;
using namespace ::izenelib::driver;

/// @addtogroup controllers
/// @{

/**
 * @brief Controller \b collection
 *
 * Start or stop a collection
 */
class CollectionController : public ::izenelib::driver::Controller
{
public:
    void start_collection();

    void stop_collection();
};

/// @}

} // namespace sf1r

#endif /* PROCESS_CONTROLLERS_COLLECTION_CONTROLLER_H_ */