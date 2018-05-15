#pragma once

#include <kty/containers/allocator.hpp>
#include <kty/containers/string.hpp>
#include <kty/containers/stringpool.hpp>
#include <kty/machine_state.hpp>

using namespace kty;

test(machine_state_constructor)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test machine_state_constructor starting.");
    MachineState<> machineState;

    Test::min_verbosity = prevTestVerbosity;
}

test(machine_state_number)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test machine_state_number starting.");
    PoolString<> name("answer");
    machineState.reset();
    assertFalse(machineState.number_exists(name));
    assertEqual(machineState.get_number_value(name), 0);

    assertTrue(machineState.set_number(name, 42));
    assertTrue(machineState.number_exists(name));
    assertEqual(machineState.get_number_value(name), 42);

    assertTrue(machineState.set_number(name, 52));
    assertTrue(machineState.number_exists(name));
    assertEqual(machineState.get_number_value(name), 52);

    Test::min_verbosity = prevTestVerbosity;
}

test(machine_state_device)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test machine_state_device starting.");
    PoolString<> name("light");
    machineState.reset();
    assertFalse(machineState.device_exists(name));
    assertEqual(machineState.get_device_type(name), DeviceType::UNKNOWN_DEVICE);
    assertEqual(machineState.get_device_info(name, 0), -1);
    assertEqual(machineState.get_device_info(name, 1), -1);
    assertEqual(machineState.get_device_info(name, 2), -1);
    assertEqual(machineState.get_device_info(name, 3), -1);

    assertTrue(machineState.set_device(name, DeviceType::LED, -1, 13, 0));
    assertTrue(machineState.device_exists(name));
    assertEqual(machineState.get_device_type(name), DeviceType::LED);
    assertEqual(machineState.get_device_info(name, 0), -1);
    assertEqual(machineState.get_device_info(name, 1), 13);
    assertEqual(machineState.get_device_info(name, 2), 0);
    assertEqual(machineState.get_device_info(name, 3), -1);
    
    assertTrue(machineState.set_device(name, DeviceType::LED, -1, 13, 100));
    assertTrue(machineState.device_exists(name));
    assertEqual(machineState.get_device_type(name), DeviceType::LED);
    assertEqual(machineState.get_device_info(name, 0), -1);
    assertEqual(machineState.get_device_info(name, 1), 13);
    assertEqual(machineState.get_device_info(name, 2), 100);
    assertEqual(machineState.get_device_info(name, 3), -1);

    Test::min_verbosity = prevTestVerbosity;
}

test(machine_state_group)
{
    int prevTestVerbosity = Test::min_verbosity;
    
    Serial.println("Test machine_state_group starting.");
    PoolString<> name("blink");
    Deque<PoolString<>> commands;
    Deque<PoolString<>> receivedCommands;
    commands.push_back(PoolString<>("light MoveBy(100, 1000)"));
    commands.push_back(PoolString<>("light MoveBy(0, 1000)"));
    commands.push_back(PoolString<>("light"));
    machineState.reset();

    assertFalse(machineState.group_exists(name));
    receivedCommands = machineState.get_group_commands(name);
    assertEqual(receivedCommands.size(), 0);

    assertTrue(machineState.set_group(name ,commands));
    assertTrue(machineState.group_exists(name));
    receivedCommands = machineState.get_group_commands(name);
    assertEqual(commands.size(), receivedCommands.size());
    for (int i = 0; i < commands.size(); ++i) {
        assertEqual(commands[i].c_str(), receivedCommands[i].c_str(), "i = " << i);
    }

    commands.push_back(PoolString<>("light"));
    assertTrue(machineState.set_group(name, commands));
    assertTrue(machineState.group_exists(name));
    receivedCommands = machineState.get_group_commands(name);
    assertEqual(commands.size(), receivedCommands.size());
    for (int i = 0; i < commands.size(); ++i) {
        assertEqual(commands[i].c_str(), receivedCommands[i].c_str(), "i = " << i);
    }

    Test::min_verbosity = prevTestVerbosity;
}
