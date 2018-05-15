#pragma once

#include <kty/containers/allocator.hpp>
#include <kty/containers/deque.hpp>
#include <kty/containers/deque_of_deque.hpp>
#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/types.hpp>

namespace kty {

/** The various device types possible */
enum DeviceType {
    LED,
    UNKNOWN_DEVICE,
};

/*!
    @brief  Class that contains information about the current machine state.
            Information stored includes device names and information,
            and group names and information.
*/
template <typename GetAllocFunc = decltype(get_alloc), typename GetPoolFunc = decltype(get_stringpool), typename PoolString = PoolString<>>
class MachineState {

public:
    /*!
        @brief  MachineState constructor.

        @param  getAllocFunc
                A function that returns a allocator pointer when called.

        @param  getPoolFunc
                A function that returns a pointer to a string pool when called.
    */
    MachineState(GetAllocFunc & getAllocFunc = get_alloc, GetPoolFunc & getPoolFunc = get_stringpool)
        : getAllocFunc_(&getAllocFunc), getPoolFunc_(&getPoolFunc),
          deviceNames_(getAllocFunc), deviceTypes_(getAllocFunc), 
          deviceInfo_0_(getAllocFunc), deviceInfo_1_(getAllocFunc), deviceInfo_2_(getAllocFunc),
          groupNames_(getAllocFunc), groupCommands_(getAllocFunc, getPoolFunc) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
    }

    /*!
        @brief  Resets the state, clearing all memory.
    */
    void reset() {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        deviceNames_.clear();
        deviceTypes_.clear();
        deviceInfo_0_.clear();
        deviceInfo_1_.clear();
        deviceInfo_2_.clear();
        groupNames_.clear();
        groupCommands_.clear();
    }

    /*!
        @brief  Checks if a number with the given name exists.
        
        @param  name
                The name of the number.
        
        @return True if the number exists, false otherwise.
    */
    bool number_exists(PoolString const & name) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        for (typename Deque<PoolString>::Iterator it = numberNames_.begin(); it != numberNames_.end(); ++it) {
            if (*it == name) {
                 return true;
            }
        }
        return false;
    }

    /*!
        @brief  Gets the value of a number.

        @param  name
                The name of the number.
        
        @return The number value, if it exists.
                Otherwise 0 is returned.
    */
    int get_number_value(PoolString const & name) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        typename Deque<PoolString>::Iterator nameIter = numberNames_.begin();
        typename Deque<int>::Iterator valueIter = numberValues_.begin();
        for ( ; nameIter != numberNames_.end() && valueIter != numberValues_.end(); ++nameIter, ++valueIter) {
            if (*nameIter == name) {
                return *valueIter;
            }
        }
        return 0;
    }

    /*!
        @brief  Sets the number.

        @param  name
                The name of the number.
        
        @param  value
                The value of the number.

        @return True if the set was successful, false otherwise.
    */
    bool set_number(PoolString const & name, int const & value) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        typename Deque<PoolString>::Iterator nameIter = numberNames_.begin();
        typename Deque<int>::Iterator valueIter = numberValues_.begin();
        for ( ; nameIter != numberNames_.end() && valueIter != numberValues_.end(); ++nameIter, ++valueIter) {
            if (*nameIter == name) {
                *valueIter = value;
                return true;
            }
        }
        bool result = true;
        result = numberNames_.push_front(name) && result;
        result = numberValues_.push_front(value) && result;
        return result;
    }

    /*!
        @brief  Checks if a device with the given name exists.
        
        @param  name
                The name of the device.
        
        @return True if the device exists, false otherwise.
    */
    bool device_exists(PoolString const & name) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        for (typename Deque<PoolString>::Iterator it = deviceNames_.begin(); it != deviceNames_.end(); ++it) {
            if (*it == name) {
                 return true;
            }
        }
        return false;
    }

    /*!
        @brief  Gets the type of a device.

        @param  name
                The name of the device.
        
        @return The device type, if it exists.
                Otherwise the unknown device type is returned.
    */
    DeviceType get_device_type(PoolString const & name) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        typename Deque<PoolString>::Iterator nameIter = deviceNames_.begin();
        typename Deque<DeviceType>::Iterator typeIter = deviceTypes_.begin();
        for ( ; nameIter != deviceNames_.end() && typeIter != deviceTypes_.end(); ++nameIter, ++typeIter) {
            if (*nameIter == name) {
                return *typeIter;
            }
        }
        return DeviceType::UNKNOWN_DEVICE;
    }

    /*!
        @brief  Gets the info of a device.

        @param  name
                The name of the device.

        @param  idx
                The information index.
        
        @return The device info, if it exists.
                Otherwise -1 is returned.
    */
    int get_device_info(PoolString const & name, int const & idx) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        typename Deque<PoolString>::Iterator nameIter = deviceNames_.begin();
        typename Deque<int>::Iterator info0Iter = deviceInfo_0_.begin();
        typename Deque<int>::Iterator info1Iter = deviceInfo_1_.begin();
        typename Deque<int>::Iterator info2Iter = deviceInfo_2_.begin();
        for ( ; nameIter != deviceNames_.end() && 
                info0Iter != deviceInfo_0_.end() &&
                info1Iter != deviceInfo_1_.end() &&
                info2Iter != deviceInfo_2_.end(); ++nameIter, ++info0Iter,  ++info1Iter, ++info2Iter) {
            if (*nameIter == name) {
                switch (idx) {
                case 0:
                    return *info0Iter;
                case 1:
                    return *info1Iter;
                case 2:
                    return *info2Iter;
                };
                return -1;
            }
        }
        return -1;
    }

    /*!
        @brief  Sets the device.

        @param  name
                The name of the device.

        @param  type
                The type of the device.

        @param  info0
                The info 0 of the device.
        
        @param  info1
                The info 1 of the device.

        @param  info2
                The info 2 of the device.
        
        @return True if the set was successful, false otherwise.
    */
    bool set_device(PoolString const & name, DeviceType type, int const & info0, int const & info1, int const & info2) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        typename Deque<PoolString>::Iterator nameIter = deviceNames_.begin();
        typename Deque<DeviceType>::Iterator typeIter = deviceTypes_.begin();
        typename Deque<int>::Iterator info0Iter = deviceInfo_0_.begin();
        typename Deque<int>::Iterator info1Iter = deviceInfo_1_.begin();
        typename Deque<int>::Iterator info2Iter = deviceInfo_2_.begin();
        for ( ; nameIter != deviceNames_.end() && 
                typeIter != deviceTypes_.end() &&
                info0Iter != deviceInfo_0_.end() &&
                info1Iter != deviceInfo_1_.end() &&
                info2Iter != deviceInfo_2_.end(); ++nameIter, ++typeIter, ++info0Iter, ++info1Iter, ++info2Iter) {
            if (*nameIter == name) {
                *typeIter = type;
                *info0Iter = info0;
                *info1Iter = info1;
                *info2Iter = info2;
                return true;
            }
        }
        bool result = true;
        result = deviceNames_.push_front(name) && result;
        result = deviceTypes_.push_front(type) && result;
        result = deviceInfo_0_.push_front(info0) && result;
        result = deviceInfo_1_.push_front(info1) && result;
        result = deviceInfo_2_.push_front(info2) && result;
        return result;
    }

    /*!
        @brief  Checks if a group with the given name exists.
        
        @param  name
                The name of the group.
        
        @return True if the group exists, false otherwise.
    */
    bool group_exists(PoolString const & name) {
        Log.verbose(F("%s\n"), PRINT_FUNC);
        for (typename Deque<PoolString>::Iterator it = groupNames_.begin(); it != groupNames_.end(); ++it) {
            if (*it == name) {
                 return true;
            }
        }
        return false;
    }

    /*!
        @brief  Gets the commands within a group.

        @param  name
                The name of the group.

        @return The commands within the group.
                If the group does not exist, an empty deque is returned.
    */
    Deque<PoolString> get_group_commands(PoolString const & name) {
        Deque<PoolString> commands;
        int i = 0;
        for (typename Deque<PoolString>::Iterator it = groupNames_.begin(); it != groupNames_.end(); ++it, ++i) {
            if (*it == name) {
                for (int j = 0; j < groupCommands_.size(i); ++j) {
                    commands.push_back(groupCommands_.get_str(i, j));
                }
            }
        }
        return commands;
    }

    /*!
        @brief  Sets a group.

        @param  name
                The name of the group.

        @param  commands
                The commands for the group.
        
        @return True if the set was successful, false otherwise.
    */
    bool set_group(PoolString const & name, Deque<PoolString> const & commands) {
        int i = 0;
        bool result = true;
        for (typename Deque<PoolString>::Iterator it = groupNames_.begin(); it != groupNames_.end(); ++it, ++i) {
            if (*it == name) {
                groupCommands_.clear(i);
                for (typename Deque<PoolString>::ConstIterator cmdIt = commands.begin(); cmdIt != commands.end(); ++cmdIt) {
                    result = groupCommands_.push_back(i, *cmdIt) && result;
                }
                return result;
            }
        }
        result = groupNames_.push_front(name) && result;
        groupCommands_.push_front();
        for (typename Deque<PoolString>::ConstIterator cmdIt = commands.begin(); cmdIt != commands.end(); ++cmdIt) {
            result = groupCommands_.push_back(0, *cmdIt) && result;
        }
        return result;
    }

private:
    GetAllocFunc * getAllocFunc_;
    GetPoolFunc * getPoolFunc_;

    Deque<PoolString> numberNames_;
    Deque<int>        numberValues_;
    Deque<PoolString> deviceNames_;
    Deque<DeviceType> deviceTypes_;
    Deque<int>        deviceInfo_0_;
    Deque<int>        deviceInfo_1_;
    Deque<int>        deviceInfo_2_;

    Deque<PoolString>      groupNames_;
    DequeDequePoolString<> groupCommands_;

};

} // namespace kty
